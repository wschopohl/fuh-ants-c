GCC=gcc
CFLAGS = -Wall -std=c99 -D_DEFAULT_SOURCE -MD -Wno-missing-braces -Wunused-result
INCLUDE=-I$(RAYLIB_PATH)/src
LDFLAGS=-L$(RAYLIB_PATH)/src
BUILD_DIR=build

LDLIBS_OSX=-lraylib -lpthread -framework OpenGL -framework Cocoa -framework IOKit -framework CoreAudio -framework CoreVideo 
LDLIBS=$(LDLIBS_OSX)

ifeq ($(BUILD_MODE),DEBUG)
    CFLAGS += -g -D_DEBUG
endif

SOURCES=main.c config.c render_engine.c simulation.c map.c world.c ant.c

OBJECTS = $(addprefix $(BUILD_DIR)/, $(SOURCES:.c=.o))
DEPS = $(addprefix $(BUILD_DIR)/,$(SOURCES:.c=.d))

EXECUTABLE=$(BUILD_DIR)/ants

all: $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS)
	$(GCC) $(OBJECTS) $(LDFLAGS) $(LDLIBS) -o $@

$(BUILD_DIR)/%.o: %.c
	$(GCC) -c $(CFLAGS) $(INCLUDE) $< -o $@

clean:
	rm build/*

-include $(OBJECTS:.o=.d)