all: dlink

dlink: dlink.cpp
	g++ -Wall -g -O4 dlink.cpp -o dlink

dlink-dev: dlink.cpp
	g++ -Wall -g -O0 dlink.cpp -o dlink-dev

test: dlink test.txt
	./dlink < test.txt

iq: dlink iq.txt
	./dlink < iq.txt > /dev/null

valgrind: dlink-dev
	valgrind ./dlink-dev < test.txt

clean:
	rm -f dlink

.PHONY: test iq valgrind clean
