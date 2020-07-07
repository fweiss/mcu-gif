SDL2HOME := /Library/Frameworks/SDL2.framework

CFLAGS :=  --std=c99

demo: build/demo/gd.o build/demo/main.o build/demo/reader.o build/main/gd.o
	$(CC) -o $@ $^ $(SDL2HOME)/SDL2

build/demo/%.o: demo-sdl2/%.c
	$(CC) -c -o $@ $^ -I$(SDL2HOME)/Headers -Isrc --std=c99

build/demo/gd.o: src/gd.c
	$(CC) -c -o $@ $^ -Isrc --std=c99
	