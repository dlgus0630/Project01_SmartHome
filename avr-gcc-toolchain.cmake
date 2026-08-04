# AVR 크로스 컴파일 툴체인 설정
# 사용: cmake -B build -DCMAKE_TOOLCHAIN_FILE=avr-gcc-toolchain.cmake

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR avr)

# 툴체인 실행 파일
set(CMAKE_C_COMPILER   avr-gcc)
set(CMAKE_ASM_COMPILER avr-gcc)
set(CMAKE_OBJCOPY      avr-objcopy)
set(CMAKE_OBJDUMP      avr-objdump)
set(CMAKE_SIZE_UTIL    avr-size)

# 타겟 환경에서 실행 파일을 만들 수 없으므로 컴파일러 검사를 정적 라이브러리로 수행
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
