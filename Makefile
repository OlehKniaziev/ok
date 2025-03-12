SMOKE_TEST = tests/smoke.cpp
TEST_FILES = tests/arena.test.o tests/string-view.test.o tests/string.test.o

CXXFLAGS += -Wall -Wextra -Werror -pedantic

.PHONY: test smoke-test

%.test.o: %.cpp compartment.hpp
	$(CXX) $(CXXFLAGS) -o $@ $<
	@ echo Running test $@...
	@ ./$@
	@ rm $@

test: smoke-test $(TEST_FILES)
	@ echo All tests passed.

smoke-test: $(SMOKE_TEST) compartment.hpp
	$(CXX) $(CXXFLAGS) -o tmp $<
	rm tmp
