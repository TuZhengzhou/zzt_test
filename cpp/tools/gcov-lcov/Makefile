SRC = $(wildcard *.cpp)
TARGETS = $(patsubst %.cpp, %, $(SRC))
GCDAS = $(patsubst %, %.gcda, $(TARGETS))
GCNOS = $(patsubst %, %.gcno, $(TARGETS))
GCOVS = $(patsubst %, %.gcov, $(SRC))
INFO = coverage.info
RES = res

all: $(TARGETS)

CC=g++
CFLAGS = -Wall -g --coverage

$(TARGETS):%:%.cpp
	$(CC) $< $(CFLAGS) -o $@

gcov:
	gcov-13 example.cpp

lcov:
	lcov --capture --directory . --output-file coverage.info --gcov-tool gcov-13



clean:
	rm -rf $(TARGETS) $(GCDAS) $(GCNOS) $(GCOVS) $(INFO) $(RES)