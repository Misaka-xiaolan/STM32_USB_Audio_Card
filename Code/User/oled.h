//
// Created by Misaka on 24-7-31.
//

#ifndef AMPLIFIER_OLED_H
#define AMPLIFIER_OLED_H
#include "main.h"

extern u8g2_t u8g2;
extern uint16_t g_fps_real;
extern uint16_t g_fps_count;
extern const uint8_t welcome_frame[];

uint8_t u8x8_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
uint8_t u8x8_byte_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
void u8g2_Init(u8g2_t *_u8g2);
void draw(u8g2_t *u8g2);
#endif //AMPLIFIER_OLED_H
