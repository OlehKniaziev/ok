CXXFLAGS += -Wall -Wextra -Werror -pedantic

.PHONY: test smoke-test

test: smoke-test

smoke-test: tests/smoke.cpp
	$(CXX) $(CXXFLAGS) -o tmp $<
	rm tmp
