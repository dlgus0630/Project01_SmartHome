/*
 * main.c - ATmega128A 스마트 홈 제어
 *
 * [계층 구조]
 *   drivers/ : MCU 주변장치 (ADC, I2C, GPIO 버튼)
 *   devices/ : 부품 (LCD1602, DHT11)
 *   app/     : 제어 정책 (조명, 커튼, 팬)
 *
 * [메인 루프 구성] 루프 1회 ≒ 1ms (_delay_ms(1) 기준)
 *   - 조도 측정(16회 평균) → 버튼 처리 → 조명/커튼 갱신
 *   - 2초 주기: DHT11 측정 + 자동 팬 제어
 *   - 1초 주기: LCD 갱신
 */
#include "config.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#include "drivers/adc.h"
#include "drivers/button.h"
#include "devices/lcd1602.h"
#include "app/light_control.h"
#include "app/curtain.h"
#include "app/fan_control.h"

/* ------------------- 버튼 핀 배치 ------------------- */
#define BTN_MODE_PIN     2  /* PG2: 조명 모드 전환 */
#define BTN_AUTO_MANUAL  3  /* PA3: 팬 자동/수동 전환 */
#define BTN_FAN_ONOFF    4  /* PA4: 팬 ON/OFF */
#define BTN_SPIN_ONOFF   5  /* PA5: 회전 ON/OFF */

/* ------------------- 주기 상수 (루프 횟수 기준) ------------------- */
#define DHT_PERIOD_TICKS 2000  /* 약 2초 */
#define LCD_PERIOD_TICKS 1000  /* 약 1초 */

static BUTTON btnMode, btnAutoToggle, btnFanToggle, btnSpinToggle;
static uint16_t dht_timer = 0;
static uint16_t lcd_timer = 0;

/* 모든 시스템 초기화 (Light -> Curtain 순서는 Timer1 공유 때문에 유지) */
static void System_init(void) {
    LCD_init();
    ADC_init();
    Light_init();
    Curtain_init();
    Fan_init();

    Button_init(&btnMode, &DDRG, &PING, BTN_MODE_PIN);
    PORTG |= (1 << BTN_MODE_PIN);

    Button_init(&btnAutoToggle, &DDRA, &PINA, BTN_AUTO_MANUAL);
    Button_init(&btnFanToggle, &DDRA, &PINA, BTN_FAN_ONOFF);
    Button_init(&btnSpinToggle, &DDRA, &PINA, BTN_SPIN_ONOFF);
    PORTA |= (1 << BTN_AUTO_MANUAL) | (1 << BTN_FAN_ONOFF) | (1 << BTN_SPIN_ONOFF);
}

/* 조도 센서(ADC0) 16회 평균 측정 */
static uint16_t read_light_average(void) {
    uint32_t sum = 0;
    for (int i = 0; i < 16; i++) sum += ADC_read(0);
    return sum / 16;
}

/* 사용자 버튼 입력 처리 */
static void process_buttons(void) {
    /* 조명 모드 전환 (흰색 -> 노란색 -> 꺼짐) */
    if (Button_getState(&btnMode) == ACT_PUSH) {
        mode = (mode + 1) % 3;
    }

    /* 팬 자동/수동 전환 */
    if (Button_getState(&btnAutoToggle) == ACT_PUSH) {
        fan_mode = (fan_mode == FAN_MANUAL) ? FAN_AUTO : FAN_MANUAL;
        if (fan_mode == FAN_AUTO) {
            dht_timer = 0;
            Fan_processAuto(); /* 전환 즉시 1회 측정/적용 */
        } else {
            Fan_setRelay(FAN_OFF);
            spin_state = SPIN_OFF;
        }
    }

    /* 회전 토글 (수동 모드에서만) */
    if (fan_mode == FAN_MANUAL) {
        if (Button_getState(&btnSpinToggle) == ACT_PUSH) {
            spin_state = (spin_state == SPIN_ON) ? SPIN_OFF : SPIN_ON;
        }
    }

    /* 팬 ON/OFF 토글 (자동 모드였다면 수동으로 전환) */
    if (Button_getState(&btnFanToggle) == ACT_PUSH) {
        if (fan_mode == FAN_AUTO) fan_mode = FAN_MANUAL;
        Fan_setRelay((fan_state == FAN_ON) ? FAN_OFF : FAN_ON);
    }
}

/* LCD 화면 갱신 (온습도/조도 + 각 장치 상태) */
static void update_lcd(uint16_t current_light) {
    char line1[30] = {0, };
    char line2[30] = {0, };

    sprintf(line1, "t:%02d h:%02d l:%04d", current_temp, current_humi, current_light);

    char c_fan = ((PORTA & (1 << PA0)) == 0) ? 'x' : 'o';
    char c_led = (led_state == 1) ? 'o' : 'x';
    char c_ct  = (curtain_state == 1) ? 'o' : 'x';

    sprintf(line2, "fan:%c led:%c ct:%c", c_fan, c_led, c_ct);

    LCD_string(0, 0, line1);
    LCD_string(1, 0, line2);
}

int main(void) {
    System_init();

    while (1) {
        /* [1] 센서 측정 */
        uint16_t current_light = read_light_average();

        /* [2] 사용자 입력 */
        process_buttons();

        /* [3] 조명/커튼 갱신 */
        Light_update(current_light, mode);
        Curtain_update(current_light);

        /* [4] 2초 주기: DHT11 측정 (+ 자동 모드일 때 팬 제어) */
        dht_timer++;
        if (dht_timer >= DHT_PERIOD_TICKS) {
            dht_timer = 0;
            Fan_processAuto();
        }

        /* [5] 회전 서보 (논블로킹) */
        Fan_processServo();

        /* [6] 1초 주기: LCD 갱신 */
        lcd_timer++;
        if (lcd_timer >= LCD_PERIOD_TICKS) {
            lcd_timer = 0;
            update_lcd(current_light);
        }

        _delay_ms(1);
    }

    return 0;
}
