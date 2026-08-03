/*
 * curtain.h - 커튼(서보) 자동 개폐 제어 (정책 계층)
 *
 * [app 계층] 조도값에 따라 커튼을 열고 닫는다. (열림: >300, 닫힘: <200 히스테리시스)
 * 서보 핀: PB7 (Timer1 OCR1C, 50Hz PWM)
 *
 * 주의: 여기서 설정하는 Timer1은 RGB LED의 G/B 채널(OCR1A/OCR1B)과 공유된다.
 */
#ifndef CURTAIN_H_
#define CURTAIN_H_

#include <avr/io.h>

extern uint8_t curtain_state; /* 0: 닫힘, 1: 열림 */

void Curtain_init(void);
void Curtain_update(uint16_t current_light);
void Curtain_setManual(uint8_t state);

#endif /* CURTAIN_H_ */
