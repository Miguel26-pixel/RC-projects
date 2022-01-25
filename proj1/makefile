# Name for our executable
RECEIVER_TARGET := receiver
EMITTER_TARGET := emitter

# Output build files
BUILD_DIR := ./build
RECEIVER_BUILD_DIR := ${BUILD_DIR}/receiver
EMITTER_BUILD_DIR := ${BUILD_DIR}/emitter
APPLICATION_LAYER_BUILD_DIR := ${BUILD_DIR}/application_layer
DATA_LINK_LAYER_BUILD_DIR := ${BUILD_DIR}/data_link_layer
GUI_BUILD_DIR := ${BUILD_DIR}/gui

# Source, include and library directories
RECEIVER_SRC_DIRS := ./src/application_layer/receiver
EMITTER_SRC_DIRS := ./src/application_layer/emitter
APPLICATION_LAYER_SRC_DIRS = ./src/application_layer/common
DATA_LINK_LAYER_SRC_DIRS = ./src/data_link_layer
GUI_SRC_DIRS = ./src/gui

# Compiler
CC := gcc

# Find all C source files (*.c) in our source directories
RECEIVER_SRCS := $(shell find ${RECEIVER_SRC_DIRS} -name *.c)
EMITTER_SRCS := $(shell find ${EMITTER_SRC_DIRS} -name *.c)
APPLICATION_LAYER_SRCS := $(shell find ${APPLICATION_LAYER_SRC_DIRS} -name *.c)
DATA_LINK_LAYER_SRCS := $(shell find ${DATA_LINK_LAYER_SRC_DIRS} -name *.c)
GUI_SRCS := $(shell find ${GUI_SRC_DIRS} -name *.c)

# Name object files after source files (append .o)
RECEIVER_OBJS := ${RECEIVER_SRCS:%=${RECEIVER_BUILD_DIR}/%.o}
EMITTER_OBJS := ${EMITTER_SRCS:%=${EMITTER_BUILD_DIR}/%.o}
APPLICATION_LAYER_OBJS := ${APPLICATION_LAYER_SRCS:%=${APPLICATION_LAYER_BUILD_DIR}/%.o}
DATA_LINK_LAYER_OBJS := ${DATA_LINK_LAYER_SRCS:%=${DATA_LINK_LAYER_BUILD_DIR}/%.o}
GUI_OBJS := ${GUI_SRCS:%=${GUI_BUILD_DIR}/%.o}

# Name dependencies after object files (append .d)
DEPENDENCIES := \
	${RECEIVER_OBJS:.o=.d} \
	${EMITTER_OBJS:.o=.d} \
	${APPLICATION_LAYER_OBJS:.o=.d} \
	${DATA_LINK_LAYER_OBJS:.o=.d} \
	${GUI_OBJS:.o=.d}

# Preprocessor flags
CPPFLAGS := -MMD -MP

# Compiler flags
CFLAGS := -Wall -Werror -Wpedantic -std=gnu11

# Linker flags
LDFLAGS :=

.DEFAULT_TARGET: all
all: ${RECEIVER_TARGET} ${EMITTER_TARGET}

${RECEIVER_BUILD_DIR}/%.c.o: %.c
	@echo "Building: " $<
	@mkdir -p ${dir $@}
	@${CC} ${CPPFLAGS} ${CFLAGS} -c $< -o $@

${EMITTER_BUILD_DIR}/%.c.o: %.c
	@echo "Building: " $<
	@mkdir -p ${dir $@}
	@${CC} ${CPPFLAGS} ${CFLAGS} -c $< -o $@

${APPLICATION_LAYER_BUILD_DIR}/%.c.o: %.c
	@echo "Building: " $<
	@mkdir -p ${dir $@}
	@${CC} ${CPPFLAGS} ${CFLAGS} -c $< -o $@

${DATA_LINK_LAYER_BUILD_DIR}/%.c.o: %.c
	@echo "Building: " $<
	@mkdir -p ${dir $@}
	@${CC} ${CPPFLAGS} ${CFLAGS} -c $< -o $@

${GUI_BUILD_DIR}/%.c.o: %.c
	@echo "Building: " $<
	@mkdir -p ${dir $@}
	@${CC} ${CPPFLAGS} ${CFLAGS} -c $< -o $@

${RECEIVER_TARGET}: ${RECEIVER_OBJS} ${APPLICATION_LAYER_OBJS} ${DATA_LINK_LAYER_OBJS} ${GUI_OBJS}
	@echo "Linking: " $@
	@${CC} ${RECEIVER_OBJS} ${APPLICATION_LAYER_OBJS} ${DATA_LINK_LAYER_OBJS} ${GUI_OBJS} -o $@ ${LDFLAGS}

${EMITTER_TARGET}: ${EMITTER_OBJS} ${APPLICATION_LAYER_OBJS} ${DATA_LINK_LAYER_OBJS} ${GUI_OBJS}
	@echo "Linking: " $@
	@${CC} ${EMITTER_OBJS} ${APPLICATION_LAYER_OBJS} ${DATA_LINK_LAYER_OBJS} ${GUI_OBJS} -o $@ ${LDFLAGS}

.PHONY: clean
clean:
	@rm -rf ${BUILD_DIR} ${RECEIVER_TARGET} ${EMITTER_TARGET} *.log
-include ${DEPENDENCIES}