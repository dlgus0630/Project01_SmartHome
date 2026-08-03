# ATmega128A Smart Home Controller - Makefile
# Usage: make              (컴파일)
#        make flash        (보드에 플래시)
#        make clean        (빌드 산출물 삭제)
#        make size         (메모리 사용량 확인)

# ====== 설정 ======
MCU        = atmega128
F_CPU      = 16000000UL
BAUD       = 115200
AVRDUDE_MCU = m128

TARGET     = smart_home
OBJDIR     = obj
DEPDIR     = $(OBJDIR)/.deps

# ====== 소스 파일 (자동 수집) ======
MAIN_SRC   = main.c
DRIVER_SRC = $(wildcard drivers/*.c)
DEVICE_SRC = $(wildcard devices/*.c)
APP_SRC    = $(wildcard app/*.c)

SOURCES    = $(MAIN_SRC) $(DRIVER_SRC) $(DEVICE_SRC) $(APP_SRC)
OBJECTS    = $(addprefix $(OBJDIR)/,$(SOURCES:.c=.o))

# ====== 컴파일러 & 도구 ======
CC         = avr-gcc
OBJCOPY    = avr-objcopy
OBJDUMP    = avr-objdump
SIZE       = avr-size
AVRDUDE    = avrdude

# ====== 컴파일 옵션 ======
CFLAGS     = -mmcu=$(MCU)
CFLAGS    += -DF_CPU=$(F_CPU)
CFLAGS    += -Os -Wall -Wextra
CFLAGS    += -ffunction-sections -fdata-sections
CFLAGS    += -MD -MP -MF $(DEPDIR)/$(@F).d

LDFLAGS    = -mmcu=$(MCU)
LDFLAGS   += -Wl,--gc-sections
LDFLAGS   += -Wl,--print-memory-usage

# ====== 기본 타겟 ======
.PHONY: all flash clean size help

all: $(OBJDIR) $(TARGET).hex
	@echo "✓ Build complete: $(TARGET).hex"

# ====== 디렉토리 생성 ======
$(OBJDIR):
	@mkdir -p $(OBJDIR) $(OBJDIR)/drivers $(OBJDIR)/devices $(OBJDIR)/app $(DEPDIR)

# ====== 컴파일 ======
$(OBJDIR)/%.o: %.c | $(OBJDIR)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@
	@echo "  CC  $<"

# ====== 링크 ======
$(TARGET).elf: $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@
	@echo "  LD  $(TARGET).elf"

# ====== HEX 생성 ======
$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex $< $@
	@echo "  HEX $(TARGET).hex"

# ====== 메모리 사용량 표시 ======
size: $(TARGET).elf
	$(SIZE) -A -d $<

# ====== 플래시 프로그래밍 (avrdude 사용) ======
# 주의: 보드 연결 및 /dev/ttyUSB0 경로는 환경에 맞춰 수정 필요
flash: $(TARGET).hex
	$(AVRDUDE) -c usbtiny -p $(AVRDUDE_MCU) -U flash:w:$(TARGET).hex:i
	@echo "✓ Flash complete"

# ====== 정리 ======
clean:
	rm -rf $(OBJDIR) $(TARGET).elf $(TARGET).hex
	@echo "✓ Clean complete"

# ====== 의존성 포함 ======
-include $(shell find $(DEPDIR) -name "*.d" 2>/dev/null)

# ====== 도움말 ======
help:
	@echo "ATmega128A Smart Home Controller - Build Targets"
	@echo ""
	@echo "  make              Compile all sources"
	@echo "  make size         Show memory usage"
	@echo "  make flash        Program to board (requires avrdude + usbtiny)"
	@echo "  make clean        Remove build artifacts"
	@echo "  make help         Show this message"
	@echo ""
	@echo "Configuration:"
	@echo "  MCU:      $(MCU)"
	@echo "  F_CPU:    $(F_CPU) Hz"
	@echo "  Compiler: $(CC)"
