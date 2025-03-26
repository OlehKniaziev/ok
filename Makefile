SMOKE_TEST = tests/smoke.cpp
TEST_FILES = tests/arena.test.o tests/string-view.test.o tests/string.test.o tests/fixed-buffer-allocator.test.o tests/to-string.test.o tests/list.test.o tests/hash.test.o tests/file.test.o tests/parse-int64.test.o tests/optional.test.o tests/align.test.o

CXXFLAGS += -std=c++20 -Og -g -Wall -Wextra -Werror -pedantic

.PHONY: test smoke-test clean

%.test.o: %.cpp ok.hpp
	$(CXX) $(CXXFLAGS) -o $@ $<
	@ echo Running test $@...
	@ ./$@

test: smoke-test $(TEST_FILES)
	@ echo All tests passed.

smoke-test: $(SMOKE_TEST) ok.hpp
	@ $(CXX) $(CXXFLAGS) -o smoke.test.o $<
	@ echo Passed the smoke test

clean:
	$(shell rm *.o)
	$(shell rm tests/*.o)
