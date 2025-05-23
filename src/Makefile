#DEBUG_FLAGS = -Os -g
#DEBUG=1
BOARD_TAG = nano
BOARD_SUB = atmega328
#BOARD_TAG = mega
#BOARD_SUB = atmega2560
MONITOR_BAUDRATE=115200
#MONITOR_BAUDRATE=9600
TARGET = NanoHomeFORTH
ISP_PROG=usbasp
USER_LIB_PATH= ../libs/
# USER_LIB_PATH= $(ARDMK_DIR)/libs/

DEFINES += -DOUTPUT_TARGET=OUTPUT_TARGET_terminal
CFLAGS += $(DEFINES)
CXXFLAGS += $(DEFINES)
ASFLAGS += $(DEFINES)
include ../Makefile
# include $(ARDMK_DIR)/Arduino.mk


$(TARGET_ELF): $(VERSION_HEADER)

asm.S: words.inc
words.inc: words.4th
	./forth2inc.py
