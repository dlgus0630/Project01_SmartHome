/*
 * config.h - 프로젝트 공통 설정
 *
 * F_CPU는 util/delay.h 사용 전에 반드시 정의되어야 한다.
 * 툴체인(-DF_CPU=...)에서 이미 정의된 경우를 대비해 #ifndef 가드를 둔다.
 */
#ifndef CONFIG_H_
#define CONFIG_H_

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#endif /* CONFIG_H_ */
