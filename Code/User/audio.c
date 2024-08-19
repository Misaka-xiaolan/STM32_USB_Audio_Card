//
// Created by Misaka on 24-7-29.
//

#include "audio.h"
#include "usbd_audio_if.h"
#include "i2s.h"
#include "arm_const_structs.h"
#include "oled.h"
#include "tim.h"

//float FFT_input[FFT_SIZE * 2];
//float hanning_window[FFT_SIZE];
//float FFT_temp[FFT_SIZE] = {0};
//float FFT_windowing_data[FFT_SIZE] = {0};
//float FFT_output[FFT_SIZE] = {0};
volatile uint32_t g_audio_size = 0;

float FFT_input[FFT_SIZE];
float hanning_window[FFT_SIZE];
float FFT_windowing_data[FFT_SIZE] = {0};
float complex FFT_output[FFT_SIZE + 1] = {0};
int16_t *audio_buffer = NULL;
volatile uint8_t g_buffer_ready = 0;

float falling_point[128] = {0};
volatile uint8_t g_display_mode = 0;
volatile uint8_t g_wave_frame_cplt = 0;

extern uint8_t g_usbd_audio_dataout_en;

void BSP_Audio_Init(uint32_t volume)
{
    BSP_Audio_Set_Volume(volume);
//    i2s_tx_callback = BSP_Audio_I2S_DMA_CallBack;
}

void BSP_Audio_Play(uint16_t *pData, uint16_t Size)
{
//    g_buffer_ready = 1;
//    audio_buffer = (int16_t *) pData;
//    g_audio_size = Size;
    audio_buffer = (int16_t *) pData;
    HAL_I2S_Transmit_DMA(&hi2s2, pData, Size);
}

void BSP_Audio_ChangeBuffer(uint16_t *pData, uint16_t Size)
{
    if (g_usbd_audio_dataout_en == 0)  /* 没有收到任何数据,全部填充0 */
    {
        memset(pData, 0, AUDIO_TOTAL_BUF_SIZE);
        g_usbd_audio_dataout_en = 2;        /* 标记已经清零了buffer,停止数据输出 */
    }
    else if (g_usbd_audio_dataout_en == 1)       /* USB有音频数据输出 */
    {
        g_usbd_audio_dataout_en = 0;        /* 清除标记位 */
    }


    g_buffer_ready = 1;
    g_audio_size = Size;
    HAL_I2S_Transmit_DMA(&hi2s2, pData, Size);
}

void BSP_Audio_Set_Volume(uint16_t volume)
{
    g_volume = volume;
    CS4398_HPvol_Set(g_volume);
}

//void Audio_Signal_FFT(uint16_t* pbuf, uint32_t Size)
//{
//    g_audio_size = Size / 2;
//    arm_hanning_f32(hanning_window, FFT_SIZE);
//    for (uint16_t i = 0; i < FFT_SIZE; i++)
//    {
//        FFT_temp[i] = (float)pbuf[2 * i] / 1000.f;
//    }
//    arm_mult_f32(FFT_temp, hanning_window, FFT_windowing_data, FFT_SIZE);
//    for (uint16_t i = 0; i < FFT_SIZE; i++)
//    {
//        FFT_input[2 * i] = FFT_windowing_data[i];
//        FFT_input[2 * i + 1] = 0;
//    }
//    arm_cfft_f32(&arm_cfft_sR_f32_len256, FFT_input, 0, 1);
//    arm_cmplx_mag_f32(FFT_input, FFT_output, FFT_SIZE / 2);
//    u8g2_ClearDisplay(&u8g2);
//    for (int i = 0; i < 128; i++)
//    {
//        u8g2_DrawVLine(&u8g2, i, 0, (uint16_t) (FFT_output[i]));
//    }
//    u8g2_SendBuffer(&u8g2);
//
//    __NOP();
//}

//void Audio_FFT_DSP(void)
//{
//    arm_mult_f32(FFT_temp, hanning_window, FFT_windowing_data, FFT_SIZE);
//    for (uint16_t i = 0; i < FFT_SIZE; i++)
//    {
//        FFT_input[2 * i] = FFT_windowing_data[i];
//        FFT_input[2 * i + 1] = 0;
//    }
//    arm_cfft_f32(&arm_cfft_sR_f32_len256, FFT_input, 0, 1);
//    arm_cmplx_mag_f32(FFT_input, FFT_output, FFT_SIZE);
//    u8g2_ClearBuffer(&u8g2);
//    for (int i = 0; i < FFT_SIZE / 2; i++)
//    {
//        u8g2_DrawHVLine(&u8g2, i, 63, (uint16_t) (FFT_output[i]), 3);
//    }
//    char temp[20] = {0};
//    sprintf(temp, "FPS:%d", g_fps_real);
//    u8g2_SetFont(&u8g2, u8g2_font_4x6_tr);
//    u8g2_DrawStr(&u8g2, 105,6,temp);
//    u8g2_SendBuffer(&u8g2);
//}

//void Audio_FFT_Transport_Buffer_DSP(const int16_t *pbuf)
//{
//    for (int i = 0; i < FFT_SIZE; ++i)
//    {
//        FFT_temp[i] = (float)pbuf[2 * i] / 500.f;
//    }
//    g_buffer_ready = 1;
//}

void Audio_FFT(void)
{
    if (audio_buffer == NULL)
    {
        return;
    }
    uint8_t fft_time = g_audio_size / (FFT_SIZE * 2);
    for (uint8_t i = 0; i < 5; i++)
    {
        for (int k = 0; k < FFT_SIZE; k++)
        {
            FFT_input[k] = (float) audio_buffer[(2 * i * FFT_SIZE) + (2 * k)] / 500.f;
        }
        arm_mult_f32(FFT_input, hanning_window, FFT_windowing_data, FFT_SIZE);
        FFT(FFT_windowing_data, FFT_output, FFT_SIZE);
        FFT_Modulo(FFT_output, FFT_SIZE);
        if (g_display_mode == 0)
        {
            Audio_FFT_Display();
        }
    }
}

void Audio_FFT_Transport_Buffer(const int16_t *pbuf)
{
    for (int i = 0; i < FFT_SIZE; ++i)
    {
        FFT_input[i] = (float) pbuf[2 * i] / 500.f;
    }
    g_buffer_ready = 1;
}

void Audio_FFT_Display(void)
{
    uint8_t column_height;
    u8g2_ClearBuffer(&u8g2);
    for (uint16_t x = 0; x < 128; x++)
    {
        if ((uint16_t) (20 * log10f(FFT_output[x] + 1)) > 63)
        {
            column_height = 63;
        }
        else
        {
            column_height = (uint8_t) (20 * log10f(FFT_output[x] + 1));
        }
        u8g2_DrawHVLine(&u8g2, x, 63, column_height, 3);

        if (falling_point[x] > 63 - column_height)
        {
            falling_point[x] = 63 - column_height;
        }
        else
        {
            falling_point[x] += 0.7f;
            if (falling_point[x] > 63.f)
            {
                falling_point[x] = 63.f;
            }
            u8g2_DrawPixel(&u8g2, x, (uint8_t) falling_point[x]);
        }
    }
    char temp[20] = {0};
    sprintf(temp, "FPS:%d", g_fps_real);
    u8g2_SetFont(&u8g2, u8g2_font_4x6_tr);
    u8g2_DrawStr(&u8g2, 105, 6, temp);
    u8g2_SendBuffer(&u8g2);
}

void Audio_Wave_Display(void)
{
    uint8_t display_time_total = g_audio_size / 128;
    HAL_TIM_Base_Start_IT(&htim7);
    for (uint8_t i = 0; i < display_time_total / 2; i++)
    {
        u8g2_ClearBuffer(&u8g2);
        for (uint8_t j = 0; j < 128; j++)
        {
            int8_t temp = (int8_t) (audio_buffer[2 * i * 128 + (2 * j)] / 100 + 32);
            if (temp > 63)
            {
                temp = 63;
            }
            else if (temp < 0)
            {
                temp = 0;
            }
            u8g2_DrawPixel(&u8g2, j, temp);
        }
        char temp[20] = {0};
        sprintf(temp, "FPS:%d", g_fps_real);
        u8g2_SetFont(&u8g2, u8g2_font_4x6_tr);
        u8g2_DrawStr(&u8g2, 105, 6, temp);
        u8g2_SendBuffer(&u8g2);
        while (g_wave_frame_cplt == 0);
        g_wave_frame_cplt = 0;
    }
    HAL_TIM_Base_Stop_IT(&htim7);
}


//void BSP_Audio_I2S_DMA_CallBack(void)
//{
//    TransferComplete_CallBack_FS();
//}

void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
    HalfTransfer_CallBack_FS();
}

void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s)
{
    TransferComplete_CallBack_FS();
}

