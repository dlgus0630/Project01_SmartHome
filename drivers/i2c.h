/*
 * i2c.h - ATmega128A TWI(I2C) 마스터 드라이버
 *
 * [drivers 계층] LCD 등 특정 장치와 무관한 범용 I2C 통신 기능.
 * 다른 I2C 장치를 추가할 때 이 드라이버를 그대로 재사용할 수 있다.
 */
#ifndef I2C_H_
#define I2C_H_

#include <avr/io.h>

void I2C_init(void);
void I2C_start(void);
void I2C_write(uint8_t data);
void I2C_stop(void);

#endif /* I2C_H_ */
