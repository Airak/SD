all: P2P_node find store logger send_successor bootstrap

%: %.cc
	g++ -std=c++11 $< -o $@

clean:
	rm $(ls --hide=*.* --hide=Makefile)
