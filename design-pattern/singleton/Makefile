# INCLUDE = *.h
# SRC = *.cpp
# TARGET = main

# main: main.o
# 	g++ main.o -o ${TARGET}

# main.o:
# 	g++ ${INCLUDE} ${SRC} -c main.o

# clean:
# 	rm ${TARGET} *.o *.gch

INCLUDE = .  # 头文件搜索路径（当前目录）
SRC = $(wildcard *.cpp)  # 自动获取所有cpp文件
OBJ = $(SRC:.cpp=.o)     # 自动生成对应的目标文件列表
TARGET = main

# 最终目标依赖所有目标文件
$(TARGET): $(OBJ)
	g++ $(OBJ) -o $@

# 每个.o文件依赖对应的.cpp文件和所有头文件
%.o: %.cpp $(wildcard *.h)
	g++ -I$(INCLUDE) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJ) *.gch