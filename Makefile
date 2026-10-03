MODE     := dev
ifeq ($(MODE), dev)
	OFLAGS := -g -O0 -DDEBUG
else
	OFLAGS := -O3 -march=native
endif
CPPFLAGS :=
CFLAGS   := -std=c++17 -pedantic -Wall $(OFLAGS)
LDFLAGS  :=

CC = g++

TARGET_EXE := schat
BUILD_DIR := build
SRC_DIR   := src
INC_DIR   := src

CPPFLAGS += $(addprefix -I,$(INC_DIR))

SRC := $(shell find $(SRC_DIR) -name '*.cpp')
OBJ := $(addprefix $(BUILD_DIR)/, $(notdir $(SRC:.cpp=.o)))

all: $(BUILD_DIR) $(BUILD_DIR)/$(TARGET_EXE)

$(BUILD_DIR)/$(TARGET_EXE): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -r $(BUILD_DIR)

.PHONY: all clean
