# Name for our executable
TARGET := download

# Output build files
BUILD_DIR := ./build

# Source, include and library directories
SRC_DIRS := ./src/

# Compiler
CC := gcc

# Find all C source files (*.c) in our source directories
SRCS := $(shell find ${SRC_DIRS} -name *.c)

# Name object files after source files (append .o)
OBJS := ${SRCS:%=${BUILD_DIR}/%.o}

# Name dependencies after object files (append .d)
DEPENDENCIES := ${OBJS:.o=.d} \

# Preprocessor flags
CPPFLAGS := -MMD -MP

# Compiler flags
CFLAGS := -Wall -Werror -Wpedantic -std=gnu11

# Linker flags
LDFLAGS :=

.DEFAULT_TARGET: all
all: ${TARGET}

${BUILD_DIR}/%.c.o: %.c
	@echo "Building: " $<
	@mkdir -p ${dir $@}
	@${CC} ${CPPFLAGS} ${CFLAGS} -c $< -o $@

${TARGET}: ${OBJS} 
	@echo "Linking: " $@
	@${CC} ${OBJS} -o $@ ${LDFLAGS}

.PHONY: clean
clean:
	@rm -rf ${BUILD_DIR} ${TARGET}
-include ${DEPENDENCIES}
