/*
 * lcd1602.h - I2C 백팩(PCF8574) 기반 1602 문자 LCD
 *
 * [devices 계층] drivers/i2c 위에서 동작하는 부품 단위 모듈.
 */
#ifndef LCD1602_H_
#define LCD1602_H_

#include "../config.h"
#include <avr/io.h>
#include <util/delay.h>

#define LCD_I2C_ADDR 0x27

void LCD_init(void);
void LCD_command(uint8_t command);
void LCD_data(uint8_t data);
void LCD_string(uint8_t row, uint8_t col, char *string);
void LCD_clear(void);

#endif /* LCD1602_H_ */
