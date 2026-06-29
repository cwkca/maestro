.PHONY: all native web play run-native run-web

CFLAGS=-ansi -Iinclude `sdl2-config --cflags` -D_POSIX_C_SOURCE=200809L
BUILD_PATH=build

all: native web

#
# Native
#

CC=clang
LDFLAGS = `sdl2-config --libs` -lSDL2_ttf -lSDL2_image
NATIVE_BUILD=$(BUILD_PATH)/native
NATIVE_OBJ_PATH=$(NATIVE_BUILD)/obj
NATIVE_OBJS = $(shell ls src | sed 's|\(.*\)\.c|$(NATIVE_OBJ_PATH)/\1.o|g')

native: $(NATIVE_BUILD)/synth
play: $(NATIVE_BUILD)/play

$(NATIVE_OBJ_PATH):
	mkdir -p $(NATIVE_OBJ_PATH)

$(NATIVE_OBJ_PATH)/%.o: src/%.c | $(NATIVE_OBJ_PATH)
	$(CC) $(CFLAGS) -c $< -o $@

$(NATIVE_OBJ_PATH)/main.o: main/native.c | $(NATIVE_OBJ_PATH)
	$(CC) $(CFLAGS) -c $< -o $@

$(NATIVE_OBJ_PATH)/play.o: main/play.c | $(NATIVE_OBJ_PATH)
	$(CC) $(CFLAGS) -c $< -o $@

$(NATIVE_BUILD)/synth: $(NATIVE_OBJS) $(NATIVE_OBJ_PATH)/main.o
	$(CC) -o $(NATIVE_BUILD)/synth $(NATIVE_OBJS) $(NATIVE_OBJ_PATH)/main.o $(LDFLAGS)

$(NATIVE_BUILD)/play: $(NATIVE_OBJS) $(NATIVE_OBJ_PATH)/play.o
	$(CC) -o $(NATIVE_BUILD)/play $(NATIVE_OBJS) $(NATIVE_OBJ_PATH)/play.o $(LDFLAGS)

run-native: $(NATIVE_BUILD)/synth
	$(NATIVE_BUILD)/synth


#
# Web (via emscripten)
#

EMFLAGS = -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='["png"]' -s USE_SDL_TTF=2
WEB_BUILD=$(BUILD_PATH)/web
WEB_OBJ_PATH=$(WEB_BUILD)/obj
WEB_OBJS = $(shell ls src | sed 's|\(.*\)\.c|$(WEB_OBJ_PATH)/\1.o|g') $(WEB_OBJ_PATH)/main.o

web: $(WEB_BUILD)/synth.js

$(WEB_BUILD)/synth.js: $(WEB_OBJS) $(shell find assets -type f)
	emcc -o $(WEB_BUILD)/synth.js $(WEB_OBJS) $(EMFLAGS) --preload-file assets
	cp assets/web/* $(WEB_BUILD)

$(WEB_OBJ_PATH)/%.o: src/%.c | $(WEB_OBJ_PATH)
	emcc -g $(CFLAGS) -c $< -o $@

$(WEB_OBJ_PATH)/main.o: main/web.c | $(WEB_OBJ_PATH)
	emcc -g $(CFLAGS) -c $< -o $@

$(WEB_OBJ_PATH):
	mkdir -p $(WEB_OBJ_PATH)

run-web: web
	echo "\nServing at http://localhost:8000\n"
	python3 -m http.server --directory $(WEB_BUILD)


#
# Util
#

clean:
	rm -rf $(BUILD_PATH)
