#include "light_control.h"

uint8_t mode = 0;
uint8_t led_state = 0;

void Light_init(void) {
    /* JTAG 비활성화 (PF 핀을 ADC로 사용하기 위함, 4클럭 내 2회 기록 필요) */
    MCUCSR |= (1 << JTD); MCUCSR |= (1 << JTD);

    DDRB |= (1 << PB4) | (1 << PB5) | (1 << PB6);

    /* Timer0 Fast PWM: 적색(OCR0, PB4) */
    TCCR0 = (1 << WGM00) | (1 << WGM01) | (1 << COM01) | (1 << CS02);

    OCR0  = 0;  /* R */
    OCR1A = 0;  /* G - Timer1 설정은 Curtain_init()에서 수행됨 */
    OCR1B = 0;  /* B */
}

void Light_setColor(uint8_t r, uint8_t g, uint8_t b) {
    OCR0  = r;
    OCR1A = g;
    OCR1B = b;

    /* 밝기 값이 모두 0이면 꺼짐, 하나라도 있으면 켜짐 */
    led_state = (r == 0 && g == 0 && b == 0) ? 0 : 1;
}

void Light_update(uint16_t raw_light, uint8_t current_mode) {
    uint8_t val = raw_light / 4;

    if (val < 3) val = 3;

    switch (current_mode) {
        case 0: /* 흰색 */
            Light_setColor(val, val, val);
            break;

        case 1: /* 노란색 */
            Light_setColor(val, val, 0);
            break;

        case 2: /* 꺼짐 */
            Light_setColor(0, 0, 0);
            break;

        default:
            Light_setColor(0, 0, 0);
            break;
    }
}
