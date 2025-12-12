#--- Pi server ---#

### Project variables
NAME := pi-server
SRC_DIR := ./src
BUILD_DIR_ROOT := ./build
INCLUDE_DIR := ./include

### Compilation variables
CC := gcc
BASE_FLAGS := -std=c99 -Wall -Wextra -Werror -fopenmp
DEBUG_FLAGS := -Og -g -ggdb -fsanitize=address
RELEASE_FLAGS := -O3 -march=native
LDFLAGS := -lmpfr -lgmp -lquadmath -flto

### Target-specific variables
ifeq ($(filter debug release,$(MAKECMDGOALS)),release)
	# Build directory
	BUILD_DIR := $(BUILD_DIR_ROOT)/release
	
	# Flags
	CFLAGS := $(RELEASE_FLAGS)
else
	BUILD_DIR := $(BUILD_DIR_ROOT)/debug
	CFLAGS := $(DEBUG_FLAGS)
endif

# Output
TARGET := $(BUILD_DIR)/$(NAME)

### Default rule
.PHONY: all
all: debug

### Files
SRCS := $(shell find $(SRC_DIR) -name '*.c')
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:%.o=%.d)
HEADERS := $(shell find $(INCLUDE_DIR) -name '*.h')

INCLUDE_FLAGS := $(addprefix -I,$(INCLUDE_DIR))
CFLAGS += $(BASE_FLAGS) $(INCLUDE_FLAGS) -MMD -MP

### Rules
$(TARGET): $(OBJS)
	@$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.c.o: %.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@ -DHIGH_PRECISION=$(HIGH_PRECISION)

.PHONY: debug
debug: $(TARGET)

.PHONY: release
release: $(TARGET)
	@strip $(TARGET)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR_ROOT)

### Include the .d dependencies
-include $(DEPS)
