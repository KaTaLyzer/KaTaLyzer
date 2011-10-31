# obj-m += buffer.o
# buffer:
# 	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
# clean:
# 	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

# buffer: buffer.h buffer.c
# 	gcc -g -Wstrict-prototypes -Wmissing-prototypes -o buffer.o -c -I. -I/lib/modules/$(shell uname -r)/build/include buffer.c

Pokus: pokus.o capture.o
	gcc -g -Wall capture.o pokus.o -o pokus

capture.o: capture.h capture.c
	gcc -g -Wall -c capture.c -o capture.o
	
pokus.o: pokus.c
	gcc -g -Wall -c pokus.c -o pokus.o
	
clean:
	rm -f *.o 
	rm -f pokus