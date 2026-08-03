#include "button.h"

void Button_init(BUTTON *button, volatile uint8_t *ddr,
                 volatile uint8_t *pin, uint8_t pinNum) {
    button->ddr = ddr;
    button->pin = pin;
    button->btnPin = pinNum;
    button->prevState = RELEASED;            /* 초기 상태: 뗌 */
    *button->ddr &= ~(1 << button->btnPin);  /* 입력 설정 */
}

uint8_t Button_getState(BUTTON *button) {
    uint8_t curState = *button->pin & (1 << button->btnPin);

    if ((curState == PUSHED) && (button->prevState == RELEASED)) {
        _delay_ms(50); /* 디바운싱 */
        button->prevState = PUSHED;
        return ACT_PUSH;
    }
    else if ((curState != PUSHED) && (button->prevState == PUSHED)) {
        _delay_ms(50);
        button->prevState = RELEASED;
        return ACT_RELEASE;
    }
    return NO_ACT;
}
