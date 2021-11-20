# Name for our executable
RECEIVER_EXECUTABLE := receiver
EMITTER_EXECUTABLE := emitter

# Output build files
BUILD_DIR := ./build
RECEIVER_BUILD_DIR := $(BUILD_DIR)/receiver
EMITTER_BUILD_DIR := $(BUILD_DIR)/emitter
APPLICATION_LAYER_COMMON_BUILD_DIR := $(BUILD_DIR)/application_layer_common
DATA_LINK_LAYER_BUILD_DIR := $(BUILD_DIR)/data_link_layer
GUI_BUILD_DIR := $(BUILD_DIR)/gui

# Source, include and library directories
RECEIVER_SOURCE_DIRS := ./src/application_layer/receiver
EMITTER_SOURCE_DIRS := ./src/application_layer/emitter
APPLICATION_LAYER_COMMON_SOURCE_DIRS = ./src/application_layer/common
DATA_LINK_LAYER_SOURCE_DIRS = ./src/data_link_layer
GUI_SOURCE_DIRS = ./src/gui

# Compiler
CC := gcc

# Find all C source files (*.c) in our source directories
RECEIVER_SOURCE_FILES := $(shell find $(RECEIVER_SOURCE_DIRS) -name *.c)
EMITTER_SOURCE_FILES := $(shell find $(EMITTER_SOURCE_DIRS) -name *.c)
APPLICATION_LAYER_COMMON_SOURCE_FILES := $(shell find $(APPLICATION_LAYER_COMMON_SOURCE_DIRS) -name *.c)
DATA_LINK_LAYER_SOURCE_FILES := $(shell find $(DATA_LINK_LAYER_SOURCE_DIRS) -name *.c)
GUI_SOURCE_FILES := $(shell find $(GUI_SOURCE_DIRS) -name *.c)

# Name object files after source files (append .o)
RECEIVER_OBJECT_FILES := $(RECEIVER_SOURCE_FILES:%=$(RECEIVER_BUILD_DIR)/%.o)
EMITTER_OBJECT_FILES := $(EMITTER_SOURCE_FILES:%=$(EMITTER_BUILD_DIR)/%.o)
APPLICATION_LAYER_COMMON_OBJECT_FILES := $(APPLICATION_LAYER_COMMON_SOURCE_FILES:%=$(APPLICATION_LAYER_COMMON_BUILD_DIR)/%.o)
DATA_LINK_LAYER_OBJECT_FILES := $(DATA_LINK_LAYER_SOURCE_FILES:%=$(DATA_LINK_LAYER_BUILD_DIR)/%.o)
GUI_OBJECT_FILES := $(GUI_SOURCE_FILES:%=$(GUI_BUILD_DIR)/%.o)

# Name dependencies after object files (append .d)
DEPENDENCIES := \
	$(RECEIVER_OBJECT_FILES:.o=.d) \
	$(EMITTER_OBJECT_FILES:.o=.d) \
	$(APPLICATION_LAYER_COMMON_OBJECT_FILES:.o=.d) \
	$(DATA_LINK_LAYER_OBJECT_FILES:.o=.d) \
	$(GUI_OBJECT_FILES:.o=.d)

# Preprocessor flags
CPPFLAGS := -MMD -MP

# Compiler flags
CFLAGS := -Wall -Werror -Wpedantic -std=gnu11

# Linker flags
LDFLAGS :=

.DEFAULT_TARGET: all
all: $(RECEIVER_EXECUTABLE) $(EMITTER_EXECUTABLE)

$(RECEIVER_BUILD_DIR)/%.c.o: %.c
	@echo "Building: " $<
	@mkdir -p $(dir $@)
	@$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@ -g

$(EMITTER_BUILD_DIR)/%.c.o: %.c
	@echo "Building: " $<
	@mkdir -p $(dir $@)
	@$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@ -g

$(APPLICATION_LAYER_COMMON_BUILD_DIR)/%.c.o: %.c
	@echo "Building: " $<
	@mkdir -p $(dir $@)
	@$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@ -g

$(DATA_LINK_LAYER_BUILD_DIR)/%.c.o: %.c
	@echo "Building: " $<
	@mkdir -p $(dir $@)
	@$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@ -g

$(GUI_BUILD_DIR)/%.c.o: %.c
	@echo "Building: " $<
	@mkdir -p $(dir $@)
	@$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@ -g

$(RECEIVER_EXECUTABLE): $(RECEIVER_OBJECT_FILES) $(APPLICATION_LAYER_COMMON_OBJECT_FILES) $(DATA_LINK_LAYER_OBJECT_FILES) $(GUI_OBJECT_FILES)
	@echo "Linking: " $@
	@$(CC) $(RECEIVER_OBJECT_FILES) $(APPLICATION_LAYER_COMMON_OBJECT_FILES) $(DATA_LINK_LAYER_OBJECT_FILES) $(GUI_OBJECT_FILES) -o $@ $(LDFLAGS) -g

$(EMITTER_EXECUTABLE): $(EMITTER_OBJECT_FILES) $(APPLICATION_LAYER_COMMON_OBJECT_FILES) $(DATA_LINK_LAYER_OBJECT_FILES) $(GUI_OBJECT_FILES)
	@echo "Linking: " $@
	@$(CC) $(EMITTER_OBJECT_FILES) $(APPLICATION_LAYER_COMMON_OBJECT_FILES) $(DATA_LINK_LAYER_OBJECT_FILES) $(GUI_OBJECT_FILES) -o $@ $(LDFLAGS) -g

.PHONY: clean
clean:
	@rm -rf $(BUILD_DIR) $(RECEIVER_EXECUTABLE) $(EMITTER_EXECUTABLE) *.log
-include $(DEPENDENCIES)