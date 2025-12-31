TARGETS = main main-jdbc

main: main.cpp
	g++ -o main main.cpp -lmysqlclient

main-jdbc: main-jdbc.cpp
	g++ -g -o main-jdbc main-jdbc.cpp -lmysqlclient -lmysqlcppconn

.PHONY: clean
clean:
	rm -f $(TARGETS)