//
// Created by Misaka on 24-8-1.
//

#include "FFT.h"


static void Step(const float *fftin, float complex *fftout, uint16_t N);

//#pragma GCC push_options
//#pragma GCC optimize("Ofast")

/**
 * @brief λ���򣨶����Ƶ�λ��
 * @param a ��������
 * @param N ���ݳ���
 * @param n �����־��nΪ0���ȡ��)
 * @return ����������
 */
static uint32_t Rebit(uint32_t a, uint16_t N, uint16_t n)
{
    uint32_t num;
    uint32_t sum = 0;
    uint16_t i = 0;

    for (i = 0; i < N; i++)
    {
        num = a >> i;
        num &= 1;
        if (n == 0)
        {
            num ^= 1;
        }
        num <<= (N - 1 - i);
        sum |= num;
    }
    return sum;
}

/**
 * @brief ��ż����㷨
 * @param fftin ʱ������
 * @param fftout Ƶ��������
 * @param N ���ݳ���
 */
static void Step(const float *fftin, float complex *fftout, uint16_t N)
{
    uint16_t n;
    uint8_t m = log2(N);
    volatile float temp1, temp2;
    for (n = 1; n < (N / 2); n++)
    {
        temp1 = fftin[Rebit(n, m, 1)];
        fftout[n + 1] = temp1;
        temp2 = fftin[Rebit(n, m, 0)];
        fftout[N - n] = temp2;
    }
    fftout[N] = fftin[N - 1];
    fftout[1] = fftin[0];
}

/**
 * @brief ���ٸ���Ҷ�任
 * @param fftout Ƶ��������
 * @param N ���ݳ���
 */
void FFT(const float *fftraw, float complex *fftout, uint32_t N)
{
    uint32_t L, B, M, j, k, p, i, r;
    float complex a = 0;
    float complex temp = 0;
    M = log2(N);

    Step(fftraw, fftout, N);
    for (L = 1; L <= M; L++)
    {
        B = (uint32_t) powf(2, L - 1);
        for (j = 0; j <= B - 1; j++)
        {
            k = (uint32_t) powf(2, M - L);
            p = j * k;

            for (i = 0; i <= k - 1; i++)
            {
                r = j + 2 * B * i;
                temp = fftout[r + 1];
                a = fftout[r + 1 + B] * cexpf((-2) * (PI / N) * p * _Complex_I);
                fftout[r + 1] = fftout[r + 1] + a;
                fftout[r + 1 + B] = temp - a;
            }
        }
    }
}

//#pragma GCC pop_options

/**
 * @brief ȡģ
 * @param fftout Ƶ��������
 * @param N ���ݳ���
 */
void FFT_Modulo(float complex *fftout, uint32_t N)
{
    uint32_t j;
    for (j = 0; j <= N; j++)
    {
        fftout[j] = cabsf(fftout[j]);
    }
}


//void arm_hanning_f32(
//        float32_t * pDst,
//        uint32_t blockSize)
//{
//    float32_t k = 2.0f / ((float32_t) blockSize);
//    float32_t w;
//
//    for(uint32_t i=0;i<blockSize;i++)
//    {
//        w = PI * i * k;
//        w = 0.5f * (1.0f - cosf (w));
//        pDst[i] = w;
//    }
//}
