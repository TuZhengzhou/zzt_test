SRC = $(wildcard *.cpp)
HEADERS = $(wildcard *.h)
TARGET = test1
CC = g++
FLAGS = -Wall -g

test1: ucontext_test.cpp uthread.cpp uthread.h
	$(CC) ucontext_test.cpp uthread.cpp uthread.h $(FLAGS) -o $@ -lpthread


.PHONY:clean 
clean:  
	-rm -rf $(TARGET)

# $(TARGETS):%:%.cpp
# 	$(CC) $(FLAGS) $< -o $@

# .PHONY:clean all  
# clean:  
# 	-rm -rf $(TARGETS)