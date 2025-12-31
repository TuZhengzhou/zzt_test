SRC = $(wildcard *.cpp)
TARGET = $(patsubst %.cpp, %, $(SRC))

all: $(TARGET)

$(TARGET):%:%.cpp
	g++ $< -o $@ -lpthread -lrt

PHONY: clean

clean:
	rm -rf $(TARGET)
