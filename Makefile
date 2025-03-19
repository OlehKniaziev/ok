SMOKE_TEST = tests/smoke.cpp
TEST_FILES = tests/arena.test.o tests/string-view.test.o tests/string.test.o tests/fixed-buffer-allocator.test.o tests/to-string.test.o tests/list.test.o tests/hash.test.o tests/file.test.o

CXXFLAGS += -Wall -Wextra -Werror -pedantic

.PHONY: test smoke-test

%.test.o: %.cpp ok.hpp
	$(CXX) $(CXXFLAGS) -o $@ $<
	@ echo Running test $@...
	@ ./$@
	@ rm $@

test: smoke-test $(TEST_FILES)
	@ echo All tests passed.

smoke-test: $(SMOKE_TEST) ok.hpp
	@ $(CXX) $(CXXFLAGS) -o smoke.test.o $<
	@ rm smoke.test.o
	@ echo Passed the smoke test
