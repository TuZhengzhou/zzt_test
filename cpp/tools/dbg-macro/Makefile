SRC = $(wildcard *.cpp)

# 作用：按 shell 通配符展开匹配的文件列表。
# $(wildcard *.cpp)：会在当前目录下找出所有 .cpp 文件，并把它们的文件名组成一个列表赋给 SRC。

TARGETS = $(patsubst %.cpp, %, $(SRC))

# patsubst 是 pattern substitute（模式替换）函数。
# 语法：$(patsubst pattern, replacement, text)
# 在 text 中，对每个单词，如果匹配 pattern，则用 replacement 替换。

CC=g++
CFLAGS = -Wall -g
CFLAGS += -DDBG_MACRO_NO_WARNING

all: $(TARGETS)

$(TARGETS):%:%.cpp
	$(CC) $< $(CFLAGS) -o $@ -lpthread

.PHONY:clean all  
clean:  
	-rm -rf $(TARGETS)