dir :=socket

all: variables.o katalyzer.h katalyzer.c processing.o wait_function.o read_conf.o cronovanie.o ksocket.o
	gcc -g variables.o processing.o wait_function.o read_conf.o cronovanie.o  ksocket.o katalyzer.c -L/usr/lib/mysql -lpcap -lmysqlclient -lpthread -o katalyzer > ilog/ilog_katalyzer

katalyzer: variables.o katalyzer.h katalyzer.c processing.o wait_function.o read_conf.o cronovanie.o
	gcc -g variables.o processing.o wait_function.o read_conf.o cronovanie.o  katalyzer.c -L/usr/lib/mysql -lpcap -lmysqlclient -lpthread -o katalyzer > ilog/ilog_katalyzer

katalyzer_sock: variables.o katalyzer.h katalyzer.c processing.o wait_function.o read_conf.o cronovanie.o ksocket.o
	gcc -g -Wall -D_CAPTURE variables.o processing.o wait_function.o read_conf.o cronovanie.o ksocket.o katalyzer.c -L/usr/lib/mysql -lmysqlclient -lpthread -o katalyzer > ilog/ilog_katalyzer

wait_function.o: wait_function.c wait_function.h
	gcc -g -c wait_function.c -o wait_function.o > ilog/ilog_wait_function.o

processing.o: processing.c
	gcc -g -c processing.c -o processing.o > ilog/ilog_processing.o

variables.o: variables.c variables.h
	gcc -g -c variables.c -o variables.o > ilog/ilog_variables.o

read_conf.o: read_conf.c read_conf.h
	gcc -g -c read_conf.c -o read_conf.o > ilog/ilog_read_conf.o

cronovanie.o: cronovanie.c cronovanie.h
	gcc -g -c cronovanie.c -o cronovanie.o > ilog/ilog_cronovanie.o

ksocket.o : ksocket.c ksocket.h
	gcc -g -c ksocket.c -o ksocket.o

clean:
	rm -f *.o > ilog/ilog_clean
	rm -f katalyzer >> ilog/ilog_clean

install:
	cp ./katalyzer /usr/local/bin/katalyzer
	cp ./start_katalyzer /etc/init.d/katalyzer
	chmod 755 /etc/init.d/katalyzer
	chmod 555 /usr/local/bin/katalyzer
	mkdir -p /etc/katalyzer
	cp ./files/my_config.conf /etc/katalyzer/config.conf
	mysql -u root -p < ../SQL/frontend.sql
	cp -r ../frontend /var/www/katalyzer
	ln -s /etc/katalyzer/config.conf /var/www/katalyzer/configs/config.conf

uninstall:
	rm /usr/local/bin/katalyzer
	rm /etc/init.d/katalyzer
	rm -rf /etc/katalyzer
	rm -rf /var/www/katalyzer
