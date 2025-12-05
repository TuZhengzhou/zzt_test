SRC = $(wildcard *.cpp)
TARGETS = $(patsubst %.cpp, %, $(SRC))
CC = g++
CFLAGS = -Wall -g
LINK = -lpthread

all:$(TARGETS)

$(TARGETS):%:%.cpp
	$(CC) $< $(CFLAGS) -o $@ $(LINK)

clean:
	rm -rf $(TARGETS)