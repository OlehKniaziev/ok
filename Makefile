SMOKE_TEST = tests/smoke.cpp
TEST_FILES = tests/arena.test.o

CXXFLAGS += -Wall -Wextra -Werror -pedantic

.PHONY: test smoke-test

%.test.o: %.cpp compartment.hpp
	$(CXX) $(CXXFLAGS) -o $@ $<
	@ echo Running test $@...
	@ ./$@
	@ rm $@

test: $(SMOKE_TEST) $(TEST_FILES)
	@ echo All tests passed.

smoke-test: tests/smoke.cpp compartment.hpp
	$(CXX) $(CXXFLAGS) -o tmp $<
	rm tmp
