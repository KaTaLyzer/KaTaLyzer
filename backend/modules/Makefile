obj-m += buffer.o
buffer:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

buffer: buffer.h buffer.c
	gcc -g -Wstrict-prototypes -Wmissing-prototypes -o buffer.o -c -I. -I/lib/modules/$(shell uname -r)/build/include buffer.c
