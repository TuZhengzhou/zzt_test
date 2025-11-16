SRCE = observer_v1.cpp
INCLUDE = $(wildcard *.h)
TARGET = v1 v2
OBJ = $(SRC:.cpp=.o)

v1: observer_v1.cpp $(INCLUDE)
	g++ ${INCLUDE} observer_v1.cpp -o $@

v2: general_main.cpp $(INCLUDE)
	g++ ${INCLUDE} general_main.cpp -o $@

clean:
	rm -f $(TARGET) $(OBJ) *.gch