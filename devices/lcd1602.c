#include "lcd1602.h"
#include "../drivers/i2c.h"

/* 4비트 모드 전송: 상위/하위 니블을 EN 펄스와 함께 전송 */
static void LCD_send(uint8_t value, uint8_t rs_mode) {
    uint8_t high_nib = value & 0xF0;
    uint8_t low_nib  = (value << 4) & 0xF0;
    uint8_t flag = rs_mode | 0x08; /* 백라이트 ON */

    I2C_start();
    I2C_write(LCD_I2C_ADDR << 1);

    I2C_write(high_nib | flag | 0x04);
    I2C_write(high_nib | (flag & ~0x04));

    I2C_write(low_nib | flag | 0x04);
    I2C_write(low_nib | (flag & ~0x04));

    I2C_stop();
    _delay_us(50);
}

void LCD_command(uint8_t command) {
    LCD_send(command, 0);
}

void LCD_data(uint8_t data) {
    LCD_send(data, 1);
}

void LCD_init(void) {
    I2C_init();
    _delay_ms(50);

    /* HD44780 4비트 모드 진입 시퀀스 (타이밍 원본 유지) */
    LCD_send(0x30, 0); _delay_ms(5);
    LCD_send(0x30, 0); _delay_us(150);
    LCD_send(0x30, 0); _delay_us(150);
    LCD_send(0x20, 0); _delay_ms(5);

    LCD_command(0x28); /* 4비트, 2라인, 5x8 폰트 */
    LCD_command(0x08); /* 디스플레이 OFF */
    LCD_command(0x01); /* 화면 클리어 */
    _delay_ms(2);
    LCD_command(0x06); /* 커서 우측 이동 */
    LCD_command(0x0C); /* 디스플레이 ON, 커서 OFF */
}

void LCD_string(uint8_t row, uint8_t col, char *string) {
    uint8_t address = (row == 0) ? 0x80 : 0xC0;
    address += col;

    LCD_command(address);

    while (*string) {
        LCD_data(*string++);
    }
}

void LCD_clear(void) {
    LCD_command(0x01);
    _delay_ms(2);
}
