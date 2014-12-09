all: pcap sock

preilog:
	mkdir -p ilog

pcap: preilog pvariables.o katalyzer.h katalyzer.c processing.o wait_function.o read_conf.o cronovanie.o
	gcc -g -DPCAP -Wall pvariables.o processing.o wait_function.o read_conf.o cronovanie.o  katalyzer.c -L/usr/local/lib/mysql -lpcap -lmysqlclient -lpthread -o katalyzer_pcap > ilog/ilog_katalyzer_pcap

sock: preilog svariables.o katalyzer.h katalyzer.c processing.o wait_function.o read_conf.o cronovanie.o ksocket.o
	gcc -g -DSOCK -Wall svariables.o processing.o wait_function.o read_conf.o cronovanie.o ksocket.o katalyzer.c -L/usr/lib/mysql -lmysqlclient -lpthread -o katalyzer_sock > ilog/ilog_katalyzer_sock

wait_function.o: wait_function.c wait_function.h
	gcc -g -Wall -c wait_function.c -o wait_function.o > ilog/ilog_wait_function

processing.o: processing.c
	gcc -g -Wall -c processing.c -o processing.o > ilog/ilog_processing

pvariables.o: variables.c variables.h
	gcc -g -DPCAP -Wall -c variables.c -o pvariables.o > ilog/ilog_variables

svariables.o: variables.c variables.h
	gcc -g -DSOCK -Wall -c variables.c -o svariables.o > ilog/ilog_variables	
read_conf.o: read_conf.c read_conf.h
	gcc -g -Wall -c read_conf.c -o read_conf.o > ilog/ilog_read_conf

cronovanie.o: cronovanie.c cronovanie.h
	gcc -g -Wall -c cronovanie.c -o cronovanie.o > ilog/ilog_cronovanie

ksocket.o : ksocket.c ksocket.h
	gcc -g -Wall -c ksocket.c -o ksocket.o > ilog/ilog_ksocket

clean:
	rm -f *.o
	rm -f ilog/*
	touch ilog/empty.file
	rm -f katalyzer_pcap
	rm -f katalyzer_sock

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
