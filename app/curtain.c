#include "curtain.h"

uint8_t curtain_state = 0;             /* 0: 닫힘, 1: 열림 */
static uint16_t current_angle = 160;   /* 서보모터의 '현재' 각도 */
static uint16_t target_angle  = 160;   /* 서보모터가 '가야 할' 목표 각도 */
static uint16_t move_tick = 0;         /* 스무스 이동 속도 조절용 */

void Curtain_init(void) {
    DDRB |= (1 << PB7);

    /* Timer1 Fast PWM Mode 14, 분주비 64, 20ms 주기
       COM1A/COM1B는 RGB LED(G/B), COM1C는 커튼 서보가 사용 */
    TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << COM1C1) | (1 << WGM11);
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11) | (1 << CS10);
    ICR1 = 4999;

    OCR1C = current_angle; /* 시작 시 현재 각도로 초기화 */
    curtain_state = 0;
}

void Curtain_update(uint16_t current_light) {
    /* 1. 센서 값 판별: 모터를 직접 돌리지 않고 "목표치"만 설정 (히스테리시스) */
    if (current_light > 300 && curtain_state == 0) {
        target_angle = 375;  /* 목표: 열기 (90도) */
        curtain_state = 1;
    }
    else if (current_light < 200 && curtain_state == 1) {
        target_angle = 150;  /* 목표: 닫기 (0도 부근) */
        curtain_state = 0;
    }

    /* 2. 스무스(Smooth) 이동: 5루프에 1번씩만 각도를 1씩 변경하여 관성 제거 */
    if (move_tick % 5 == 0) {
        if (current_angle < target_angle) {
            current_angle++;
            OCR1C = current_angle;
        }
        else if (current_angle > target_angle) {
            current_angle--;
            OCR1C = current_angle;
        }
    }

    move_tick++;
    if (move_tick >= 1000) move_tick = 0;
}

void Curtain_setManual(uint8_t state) {
    if (state == 1) {
        target_angle = 375; /* 열기 */
        curtain_state = 1;
    } else {
        target_angle = 150; /* 닫기 */
        curtain_state = 0;
    }
}
