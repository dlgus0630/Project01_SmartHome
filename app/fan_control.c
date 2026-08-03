#include "fan_control.h"
#include "../devices/dht11.h"

uint8_t fan_mode   = FAN_MANUAL;
uint8_t fan_state  = FAN_OFF;
uint8_t spin_state = SPIN_OFF;

uint8_t current_temp = 0;
uint8_t current_humi = 0;

void Fan_init(void) {
    /* 1. DC 팬 릴레이 (PA0) — Active-Low (LOW = 접점 ON) */
    DDRA |= (1 << PA0);
    PORTA |= (1 << PA0); /* 초기: 릴레이 OFF (팬 정지) */

    /* 2. 선풍기 회전 서보 (PE3, Timer3 Fast PWM Mode 14, 분주비 64) */
    DDRE |= (1 << PE3);
    TCCR3A = (1 << COM3A1) | (1 << WGM31);
    TCCR3B = (1 << WGM33) | (1 << WGM32) | (1 << CS31) | (1 << CS30);
    ICR3 = 4999;  /* 20ms 주기 (16MHz 기준) */
    OCR3A = 375;  /* 서보 모터 중앙 정렬 */

    /* 3. DHT11 센서 핀 초기화 */
    DHT11_init();
}

void Fan_setRelay(uint8_t state) {
    fan_state = state;
    if (state == FAN_ON) {
        PORTA &= ~(1 << PA0);
    } else {
        PORTA |= (1 << PA0);
    }
}

/* 논블로킹 회전 서보 제어 (메인 루프에서 매회 호출) */
void Fan_processServo(void) {
    static uint16_t timeCount = 0;
    static uint16_t currentAngle = 375;
    static int16_t direction = 2;  /* 부드러운 미세 이동 */

    if (spin_state == SPIN_ON) {
        timeCount++;
        if (timeCount >= 5) {
            timeCount = 0;
            currentAngle += direction;

            if (currentAngle >= 650) {
                currentAngle = 650;
                direction = -2;
            } else if (currentAngle <= 100) {
                currentAngle = 100;
                direction = 2;
            }

            OCR3A = currentAngle;
        }
    } else {
        timeCount = 0;
    }
}

/*
 * DHT11 측정값 기반 조건 판별
 *   0: 측정 실패, 1: ON 조건, 2: OFF 조건, 3: 유지(히스테리시스)
 * 측정 성공 시 current_temp / current_humi가 항상 갱신된다 (LCD 표시용).
 */
static uint8_t Fan_checkCondition(void) {
    uint8_t temp, humi;

    if (!DHT11_read(&temp, &humi)) {
        return 0; /* 측정 실패 */
    }

    current_temp = temp;
    current_humi = humi;

    /* 팬 ON 조건: 온도 29℃ 이상 또는 습도 61% 이상 */
    if (current_temp >= 29 || current_humi >= 61)
        return 1;
    /* 팬 OFF 조건: 온도 23℃ 이하 이고 습도 40% 이하 */
    if (current_temp <= 23 && current_humi <= 40)
        return 2;

    return 3; /* 히스테리시스 (상태 유지) */
}

/*
 * 주기적 자동 제어 처리.
 * 측정(current_temp/humi 갱신)은 모드와 무관하게 항상 수행하고,
 * 팬 제어 적용은 자동 모드일 때만 수행한다.
 */
void Fan_processAuto(void) {
    uint8_t condition = Fan_checkCondition();

    if (fan_mode != FAN_AUTO) return;

    if (condition == 1) {                          /* ON 조건 */
        Fan_setRelay(FAN_ON);
        spin_state = SPIN_ON;
    } else if (condition == 2 || condition == 0) { /* OFF 또는 측정 실패 */
        Fan_setRelay(FAN_OFF);
        spin_state = SPIN_OFF;
    }
    /* condition == 3: 상태 유지 */
}
