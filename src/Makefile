all: P2P_node find store logger send_successor bootstrap ping

%: %.cc
	g++ -std=c++11 -pthread $< -o $@

clean:
	rm $(ls --hide=*.* --hide=Makefile)
