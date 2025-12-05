SOURCE = $(wildcard *.cpp)  
TARGETS = $(patsubst %.cpp, %, $(SOURCE))
DATAS = $(wildcard example.txt[0-9]*)

CC = g++
CFLAGS = -Wall -g  

all:$(TARGETS)  

$(TARGETS):%:%.cpp 
	$(CC) $< $(CFLAGS) -o $@ -lpthread

.PHONY:clean all  
clean:  
	-rm -rf $(TARGETS) $(DATAS)