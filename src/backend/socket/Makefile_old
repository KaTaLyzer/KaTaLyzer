Pokus: pokus.o ksocket.o
	gcc -g -Wall ksocket.o pokus.o -o pokus

ksocket.o: ksocket.h ksocket.c
	gcc -g -Wall -c ksocket.c -o ksocket.o
	
pokus.o: pokus.c
	gcc -g -Wall -c pokus.c -o pokus.o
	
clean:
	rm -f *.o 
	rm -f pokus