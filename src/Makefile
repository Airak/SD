all: server_rpc find store logger

%: %.cc
	g++ -std=c++11 $< -o $@

clean:
	find . -type f  ! -name "*.*" -o -name "Makefile"  -delete
