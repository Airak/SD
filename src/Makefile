all: server client

%: %.cc
	g++ -std=c++11 $< -o $@

%: %.c
	gcc $< -o $@
