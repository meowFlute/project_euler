# Manual Inputs
CC=gcc
TARGET_EXEC=project_euler
BUILD_DIR=./build
SRC_DIR=./src
INC_DIRS := $(SRC_DIR) $(SRC_DIR)/problems
LDFLAGS := -lgmp

# Find all the files we want to compile, without folder names
BASE_SRCS := $(wildcard $(SRC_DIR)/*.c) 
PROBLEM_SRCS := $(wildcard $(SRC_DIR)/problems/*.c)

# Generate Build Folder Targets
BASE_OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(BASE_SRCS))
PROBLEM_OBJS := $(patsubst $(SRC_DIR)/problems/%.c,$(BUILD_DIR)/problems/%.o,$(PROBLEM_SRCS))
OBJS := $(BASE_OBJS) $(PROBLEM_OBJS)

# Add a prefix to INC_DIRS
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

# Compiler flags
CFLAGS := $(INC_FLAGS) -Wall -Wextra -g

# make all will also run the compiledb and ctags commands
all: post_build

# this is the one that I'll run as part of my automated checking workflow
check: $(TARGET_EXEC)
	$(info This is where I'll add some checks like unit tests but I haven't yet)

# The final build step.
$(TARGET_EXEC): build_dir $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

build_dir:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BUILD_DIR)/problems

# Build step for all source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/problems/%.o: $(SRC_DIR)/problems/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Post-build running of ctags and compiledb
post_build: $(TARGET_EXEC)
	compiledb -n make
	ctags

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(TARGET_EXEC)
