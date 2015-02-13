COMPILER = gcc
all: BookOrder

debug:
	make DEBUG=TRUE

BookOrder: BookOrder.o
	$(COMPILER) -o BookOrder -pthread BookOrder.o 
BookOrder.o: BookOrder.c BookOrder.h 
	$(COMPILER) -c BookOrder.c
clean:
	rm -rf BookOrder
	rm -rf *.o

rebuild: clean all
