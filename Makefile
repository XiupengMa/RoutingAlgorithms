all:linkstate distvec
linkstate:linkstate.cpp
	g++ linkstate.cpp -o linkstate
distvec:distvec.cpp
	g++ distvec.cpp -o distvec
clean:
	rm -f distvec linkstate
