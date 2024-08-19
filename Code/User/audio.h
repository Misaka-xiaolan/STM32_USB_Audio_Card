//
// Created by Misaka on 24-7-29.
//

#ifndef AMPLIFIER_AUDIO_H
#define AMPLIFIER_AUDIO_H

#include "main.h"
#include "FFT.h"


void BSP_Audio_Init(uint32_t volume);
void BSP_Audio_Play(uint16_t *pData, uint16_t Size);
void BSP_Audio_ChangeBuffer(uint16_t *pData, uint16_t Size);
void BSP_Audio_Set_Volume(uint16_t volume);
void BSP_Audio_I2S_DMA_CallBack(void);
//void Audio_Signal_FFT(uint16_t* pbuf, uint32_t Size);
void Audio_FFT(void);
void Audio_FFT_Display(void);
void Audio_Wave_Display(void);
void Audio_FFT_DSP(void);

extern float hanning_window[FFT_SIZE];
extern volatile uint8_t g_buffer_ready;
extern volatile uint8_t g_display_mode;
extern volatile uint8_t g_wave_frame_cplt;


#endif //AMPLIFIER_AUDIO_H
