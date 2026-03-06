all: t_sem_open.cpp t_sem_unlink.cpp
	g++ t_sem_open.cpp -o t_sem_open -lpthread
	g++ t_sem_unlink.cpp -o t_sem_unlink -lpthread

PHONY: clean

clean:
	rm -rf t_sem_open t_sem_unlink
