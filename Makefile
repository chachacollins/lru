CXX=clang++
CFLAGS=-Wall -Wextra -std=c++20 -O3 -ggdb

all: lev

lev: lev.cpp
	$(CXX) $(CFLAGS) lev.cpp -o lev

# lru: lru.cpp
# 	$(CXX) $(CFLAGS) lru.cpp -o lru
