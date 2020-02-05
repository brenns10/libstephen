.PHONY: release debug test doc cov

build:
	meson build

release: build
	ninja -C build

debug: build
	ninja -C build

test: build
	ninja -C build test

doc:
	doxygen
	make -C doc html

cov:
	true # we're not doing this for now
