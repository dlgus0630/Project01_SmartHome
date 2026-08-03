/*
 * lcd_hw_test.c - LCD 하드웨어 점검용 코드 (빌드 대상 아님)
 *
 * main.c에 주석으로 있던 LCD 테스트 코드를 보관용으로 분리했다.
 * 사용법: 이 파일의 main을 프로젝트의 main.c 대신 빌드하면 된다.
 */
#include "../config.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "../devices/lcd1602.h"

int main(void) {
    /* 1. LCD 초기화 (내부적으로 I2C 통신도 함께 초기화됨) */
    LCD_init();

    /* 2. 1행에 고정 메시지 출력 */
    LCD_string(0, 0, "LCD Test Mode!!");

    uint16_t counter = 0;
    char buffer[17]; /* 16글자 + 널 문자 */

    while (1) {
        /* 3. 2행에 1초마다 올라가는 카운터 출력 (보드 생존 확인용) */
        sprintf(buffer, "Counter: %04d   ", counter);
        LCD_string(1, 0, buffer);

        counter++;
        _delay_ms(1000);
    }

    return 0;
}
