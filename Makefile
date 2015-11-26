P = sharedPtr
F = -g -Wall -std=c++0x -pthread
C = g++

all: 
	$C $F -o $P SharedPtr_test.cpp

self_test:
	$C $F -o $P self_test.cpp

clean:
	rm *.o $P
