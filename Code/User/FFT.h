//
// Created by Misaka on 24-8-1.
//

#ifndef AMPLIFIER_FFT_H
#define AMPLIFIER_FFT_H

#include "main.h"
#include "complex.h"

#define FFT_SIZE 512

void FFT(const float *fftraw, float complex *fftout, uint32_t N);
void FFT_Modulo(float complex *fftout, uint32_t N);

#endif //AMPLIFIER_FFT_H
