IDIR0 = ./
IDIR1 = ./hwy
IDIR2 = ./hwy/ops

all: main.js main.wasm

main.js main.wasm: main.cpp
	emcc $< -I$(IDIR0) -I$(IDIR1) -I$(IDIR2) -std=c++17 -s WASM=1 -s EXPORTED_FUNCTIONS='["_myAdd", "_pocHwy"]' -o $@

clean:
	rm -f main.js main.wasm