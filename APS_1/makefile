CC = gcc
CFLAGS = -Wall -Wextra
INCLUDES = -Iinclude -Iheader -I/usr/include/X11/
LIBS = -lX11

# Source files
SRC_DIR = src
APP_DIR = app
SRCS = $(wildcard $(SRC_DIR)/*.c) $(wildcard $(APP_DIR)/*.c)

# Object files will be placed in build directory
OBJS = $(patsubst $(SRC_DIR)/%.c, build/%.o, $(wildcard $(SRC_DIR)/*.c)) \
       $(patsubst $(APP_DIR)/%.c, build/%.o, $(wildcard $(APP_DIR)/*.c))

# Executable
TARGET = build/ranking

# Ensure build directory structure exists
BUILD_DIRS = build

.PHONY: all clean

all: $(BUILD_DIRS) $(TARGET)

$(BUILD_DIRS):
	mkdir -p $@

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $(OBJS) $(LIBS)

build/%.o: src/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

build/%.o: app/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf build/*