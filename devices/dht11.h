/*
 * dht11.h - DHT11 온습도 센서 (1-Wire 유사 프로토콜)
 *
 * [devices 계층] 센서에서 온도/습도를 읽어오는 역할만 한다.
 * "몇 도면 팬을 켤 것인가" 같은 정책은 app 계층(fan_control)이 담당한다.
 *
 * 연결 핀: PA1 (내부 풀업 사용)
 */
#ifndef DHT11_H_
#define DHT11_H_

#include "../config.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define DHT11_PIN PA1

void    DHT11_init(void);
/* 성공 시 1 반환 및 *temp, *humi 갱신 / 실패(타임아웃, 체크섬 오류) 시 0 */
uint8_t DHT11_read(uint8_t *temp, uint8_t *humi);

#endif /* DHT11_H_ */
