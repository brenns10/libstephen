.PHONY: release debug test doc cov

release:
	cmake -Brelease -H.
	cmake --build release

debug:
	cmake -Bdebug -H.
	cmake --build debug

test: debug
	valgrind debug/test_libstephen

doc:
	doxygen
	make -C doc html

cov: $(BINARY_DIR)/$(CFG)/$(TEST_TARGET)
	# Run a special CMake build for coverage.
	cmake -Bcoverage -H. -DCMAKE_BUILD_TYPE=COVERAGE
	cmake --build coverage
	# Run the built tests.
	coverage/test_libstephen
	# Create report.
	lcov -c -d coverage/CMakeFiles/stephen.dir/src/ -b src -o coverage.info
	lcov -e coverage.info "`pwd`/src/*" -o coverage.info
	genhtml coverage.info -o cov
	# Cleanup.
	rm -rf coverage.info coverage
