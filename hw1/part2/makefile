all: a.out
	./a.out

a.out: main.cpp thread_pool.hpp makefile
	g++ -std=c++20 -pthread -Ddebug -Wall -Wextra -Wpedantic -Wshadow -g main.cpp
# 	g++ -std=c++20 -pthread -O2 main.cpp

