#include "dht11.h"

void DHT11_init(void) {
    /* 입력 + 내부 풀업 (버스 idle HIGH) */
    DDRA &= ~(1 << DHT11_PIN);
    PORTA |= (1 << DHT11_PIN);
}

uint8_t DHT11_read(uint8_t *temp, uint8_t *humi) {
    uint8_t data[5] = {0, 0, 0, 0, 0};
    uint8_t i, j;
    uint16_t timeout;

    /* 마이크로초 타이밍이 꼬이지 않도록 모든 인터럽트 일시 중지 */
    cli();

    /* MCU의 Start Signal 전송 */
    DDRA |= (1 << DHT11_PIN);    /* 출력 모드로 설정 */
    PORTA &= ~(1 << DHT11_PIN);  /* LOW 신호 전송 */
    _delay_ms(18);               /* 18ms 유지 */

    /* 버스 충돌 방지를 위해 즉시 입력 모드로 전환하여 센서에게 양보 */
    DDRA &= ~(1 << DHT11_PIN);
    PORTA |= (1 << DHT11_PIN);   /* 내부 풀업 활성화 */

    /* 센서가 응답(LOW)할 때까지 대기 (약 20~40us 소요) */
    timeout = 10000;
    while (PINA & (1 << DHT11_PIN)) {
        if (--timeout == 0) { sei(); return 0; }
    }

    /* 응답 HIGH 대기 */
    timeout = 10000;
    while (!(PINA & (1 << DHT11_PIN))) {
        if (--timeout == 0) { sei(); return 0; }
    }

    /* 응답 LOW 대기 (데이터 시작 직전) */
    timeout = 10000;
    while (PINA & (1 << DHT11_PIN)) {
        if (--timeout == 0) { sei(); return 0; }
    }

    /* 데이터 40비트(5바이트) 읽기 */
    for (i = 0; i < 5; i++) {
        for (j = 0; j < 8; j++) {
            /* 비트 전송 시작(LOW 50us)이 끝날 때까지 대기 */
            timeout = 10000;
            while (!(PINA & (1 << DHT11_PIN))) {
                if (--timeout == 0) { sei(); return 0; }
            }

            /* '0'(28us)과 '1'(70us)을 구분하기 위해 40us 대기 */
            _delay_us(40);

            /* 40us 후에도 HIGH라면 '1' */
            if (PINA & (1 << DHT11_PIN)) {
                data[i] |= (1 << (7 - j));
            }

            /* 해당 비트의 HIGH 구간이 끝날 때까지 대기 */
            timeout = 10000;
            while (PINA & (1 << DHT11_PIN)) {
                if (--timeout == 0) { sei(); return 0; }
            }
        }
    }

    /* 수신 완료, 인터럽트 재활성화 */
    sei();

    /* Checksum 검증 */
    if (data[4] == (uint8_t)(data[0] + data[1] + data[2] + data[3])) {
        *humi = data[0];
        *temp = data[2];
        return 1;
    }

    return 0;
}
