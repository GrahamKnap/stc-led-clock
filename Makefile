SDCC ?= sdcc
STCCODESIZE ?= 8000
SDCCOPTS ?= --iram-size 256 --code-size $(STCCODESIZE) --data-loc 0x30
OBJECTS=$(patsubst src%.c,build%.rel,$(wildcard src/*.c))
HEADERS=$(wildcard src/*.h)

all: clock.hex

build/%.rel: src/%.c $(HEADERS)
	mkdir -p $(dir $@)
	$(SDCC) $(SDCCOPTS) -o $@ -c $<

clock.hex: $(OBJECTS)
	$(SDCC) -o build/$@ $(SDCCOPTS) $^
	cp build/$@ .
	gawk -f lastadr.awk $@

clean:
	rm -f *.hex
	rm -rf build/*

