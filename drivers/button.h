/*
 * button.h - GPIO 버튼 입력 드라이버 (엣지 검출 + 디바운싱)
 *
 * [drivers 계층] 어떤 포트/핀이든 BUTTON 구조체로 추상화하여 처리한다.
 * 실제 핀 배치는 앱(main.c)에서 결정한다.
 */
#ifndef BUTTON_H_
#define BUTTON_H_

#include "../config.h"
#include <avr/io.h>
#include <util/delay.h>

enum { PUSHED, RELEASED };          /* 물리 상태 (풀업이므로 눌림 = 0) */
enum { NO_ACT, ACT_PUSH, ACT_RELEASE }; /* 이벤트 */

typedef struct {
    volatile uint8_t *ddr;
    volatile uint8_t *pin;
    uint8_t btnPin;
    uint8_t prevState;
} BUTTON;

void    Button_init(BUTTON *button, volatile uint8_t *ddr,
                    volatile uint8_t *pin, uint8_t pinNum);
uint8_t Button_getState(BUTTON *button);

#endif /* BUTTON_H_ */
