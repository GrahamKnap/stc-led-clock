SDCC ?= sdcc
SDCCOPTS=--data-loc 0x30 --debug
OBJECTS=build/test.rel build/adc.rel build/config.rel build/display.rel build/ds1302.rel build/utility.rel
HEADERS=$(wildcard ../src/*.h)

.PHONY: build test

all: test

build:
	mkdir -p build

build/adc.rel: ../src/adc.c $(HEADERS) build
	$(SDCC) $(SDCCOPTS) -o $@ -c $<

build/config.rel: ../src/config.c $(HEADERS) build
	$(SDCC) $(SDCCOPTS) -o $@ -c $<

build/display.rel: ../src/display.c $(HEADERS) build
	$(SDCC) $(SDCCOPTS) -o $@ -c $<

build/ds1302.rel: ../src/ds1302.c $(HEADERS) build
	$(SDCC) $(SDCCOPTS) -o $@ -c $<

build/utility.rel: ../src/utility.c $(HEADERS) build
	$(SDCC) $(SDCCOPTS) -o $@ -c $<

build/test.rel: test.c $(HEADERS) build
	$(SDCC) $(SDCCOPTS) -o $@ -c $<

build/test.hex: $(OBJECTS)
	$(SDCC) $(SDCCOPTS) -o $@ $^

test: build/test.hex
	s51 -I if=xram[0xffff] -G $<
	@echo
	
clean:
	rm -rf build/*

