/*
 * adc.h - ATmega128A 내장 ADC 드라이버
 *
 * [drivers 계층] MCU 주변장치 레지스터를 직접 제어한다.
 * 특정 센서에 대한 지식 없이 "채널을 읽는다"는 기능만 제공한다.
 */
#ifndef ADC_H_
#define ADC_H_

#include <avr/io.h>

void     ADC_init(void);
uint16_t ADC_read(uint8_t channel);

#endif /* ADC_H_ */
