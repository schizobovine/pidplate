PROG=pidsim
CC=g++
CFLAGS+=-std=gnu++11 -g -pipe -Wall -g -O0
CXXFLAGS+=-std=gnu++11 -g -pipe -Wall -g -O0
LDFLAGS+=-lm

.PHONY: all

$(PROG): main.o PID_Controller.o PID_Simulator.o
	$(CC) $(CFLAGS) -o $@ $(LDFLAGS) $^

.PHONY: clean
clean:
	rm -f *.o $(PROG)
