cmake_minimum_required(VERSION 3.10.0)

project(atmega128a
  LANGUAGES ASM C
)

set(EXECUTABLE ${PROJECT_NAME}.elf)

# 소스 파일 수집 (3계층 구조)
file(GLOB SRC_FILES CONFIGURE_DEPENDS
  *.c
)
file(GLOB_RECURSE SRC_FILES_RECURSE CONFIGURE_DEPENDS
  drivers/*.c
  devices/*.c
  app/*.c
)

# 실행 파일 생성
add_executable(${EXECUTABLE}
  ${SRC_FILES}
  ${SRC_FILES_RECURSE}
)

# 컴파일 타임 매크로
target_compile_definitions(${EXECUTABLE} PRIVATE
  -DF_CPU=16000000UL
)

# include 디렉토리 설정
target_include_directories(${EXECUTABLE} PRIVATE
  .
  drivers
  devices
  app
)

# 컴파일러 옵션
target_compile_options(${EXECUTABLE} PRIVATE
  -mmcu=atmega128
  -fdata-sections
  -ffunction-sections
  -MMD
  -flto
  -fno-fat-lto-objects
  -Wall
  -Os
  -g3
)

# 링커 옵션
target_link_options(${EXECUTABLE} PRIVATE
  -mmcu=atmega128
  -flto
  -fuse-linker-plugin
  -lm
  -Wl,-Map=${CMAKE_BINARY_DIR}/${PROJECT_NAME}.map,--cref
  -Wl,--gc-sections
  -Xlinker -print-memory-usage -Xlinker
)

# 바이너리 파일 생성 (.hex)
add_custom_command(TARGET ${EXECUTABLE}
  POST_BUILD
  COMMAND ${CMAKE_OBJCOPY} -O ihex -R .eeprom ${EXECUTABLE} ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.hex
  COMMENT "Generating HEX file"
)

# EEPROM 파일 생성 (.eep)
add_custom_command(TARGET ${EXECUTABLE}
  POST_BUILD
  COMMAND ${CMAKE_OBJCOPY} -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 ${EXECUTABLE} ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.eep
  COMMENT "Generating EEPROM file"
)

# 바이너리 사이즈 출력
add_custom_command(TARGET ${EXECUTABLE}
  POST_BUILD
  COMMAND ${CMAKE_SIZE_UTIL} ${EXECUTABLE}
  COMMENT "Firmware Size:"
)
