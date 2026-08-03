/*
 * fan_control.h - 선풍기(릴레이 + 회전 서보) 제어 (정책 계층)
 *
 * [app 계층] DHT11 측정값을 바탕으로 팬 ON/OFF와 회전을 결정한다.
 *   - ON  조건: 온도 >= 29℃ 또는 습도 >= 61%
 *   - OFF 조건: 온도 <= 23℃ 그리고 습도 <= 40%
 *   - 그 사이: 상태 유지 (히스테리시스)
 *
 * 릴레이 핀: PA0 (Active-Low, LOW = 접점 ON)
 * 회전 서보 핀: PE3 (Timer3, 50Hz PWM)
 */
#ifndef FAN_CONTROL_H_
#define FAN_CONTROL_H_

#include <avr/io.h>

enum { FAN_MANUAL, FAN_AUTO };
enum { FAN_OFF, FAN_ON };
enum { SPIN_OFF, SPIN_ON };

extern uint8_t fan_mode;
extern uint8_t fan_state;
extern uint8_t spin_state;
extern uint8_t current_temp; /* LCD 표시용 최신 측정값 */
extern uint8_t current_humi;

void Fan_init(void);
void Fan_setRelay(uint8_t state);
void Fan_processServo(void);   /* 논블로킹 회전 서보 처리 (메인 루프에서 매회 호출) */
void Fan_processAuto(void);    /* DHT11 측정 + 자동 모드일 때만 팬 제어 적용 */

#endif /* FAN_CONTROL_H_ */
