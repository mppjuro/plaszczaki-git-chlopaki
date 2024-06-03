all: bin/problem2 bin/generator

prob1: bin/problem1 bin/generator
	bin/problem1

prob2: bin/problem2 bin/generator
	bin/problem2

bin/problem2: src/problem2-v10.cpp
	g++ -O3 -std=c++17 -o $@ $<

bin/generator: src/generator-v4.cpp
	g++ -O3 -std=c++17 -o $@ $< 
	bin/generator

clean:
	rm bin/* junk/* input/* results/*
	