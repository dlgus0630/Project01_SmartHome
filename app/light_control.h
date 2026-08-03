/*
 * light_control.h - RGB LED 조명 제어 (정책 계층)
 *
 * [app 계층] 조도값과 모드에 따라 LED 색/밝기를 결정한다.
 * ADC 읽기는 drivers/adc, PWM 출력 핀은 PB4(R)/PB5(G)/PB6(B).
 *
 * 주의: 녹색/청색 PWM(OCR1A/OCR1B)은 Timer1을 사용하며,
 *       Timer1 설정 자체는 커튼 서보와 공유되어 app/curtain.c에서 수행된다.
 *       따라서 초기화 순서는 Light_init() -> Curtain_init() 이어야 한다.
 */
#ifndef LIGHT_CONTROL_H_
#define LIGHT_CONTROL_H_

#include <avr/io.h>

/* 조명 모드: 0=흰색, 1=노란색, 2=꺼짐 */
extern uint8_t mode;
extern uint8_t led_state; /* 0=꺼짐, 1=켜짐 (LCD 표시용) */

void Light_init(void);
void Light_update(uint16_t raw_light, uint8_t current_mode);
void Light_setColor(uint8_t r, uint8_t g, uint8_t b);

#endif /* LIGHT_CONTROL_H_ */
