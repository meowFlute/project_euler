# Manual Inputs
CC=gcc
TARGET_EXEC=project_euler
BUILD_DIR=./build
SRC_DIR=./src
INC_DIRS := $(SRC_DIR)
#LDFLAGS := -lm

# Find all the files we want to compile, without folder names
SRCS := $(wildcard $(SRC_DIR)/*.c)

# Generate Build Folder Targets
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

# Add a prefix to INC_DIRS
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

# Compiler flags
CFLAGS := $(INC_FLAGS) -Wall -Wextra

# make all will also run the compiledb and ctags commands
all: post_build

# The final build step.
$(TARGET_EXEC): build_dir $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

build_dir:
	mkdir -p $(BUILD_DIR)

# Build step for all source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Post-build running of ctags and compiledb
post_build: $(TARGET_EXEC)
	compiledb -n make
	ctags

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(TARGET_EXEC)
