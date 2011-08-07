katalyzer: variables.o katalyzer.h katalyzer.c processing.o wait_function.o read_conf.o cronovanie.o cdp.o snmpp.o sip.o
	gcc -g variables.o processing.o wait_function.o read_conf.o cronovanie.o cdp.o snmpp.o sip.o katalyzer.c -L/usr/lib/mysql -lpcap -lmysqlclient -lpthread -o katalyzer > ilog/ilog_katalyzer

wait_function.o: wait_function.c wait_function.h
	gcc -g -c wait_function.c -o wait_function.o > ilog/ilog_wait_function.o

processing.o: processing.c
	gcc -g -c processing.c -o processing.o > ilog/ilog_processing.o

variables.o: variables.c variables.h
	gcc -g -c variables.c -o variables.o > ilog/ilog_variables.o

read_conf.o: read_conf.c read_conf.h
	gcc -g -c read_conf.c -o read_conf.o > ilog/ilog_read_conf.o

cronovanie.o: cronovanie.c
	gcc -g -c cronovanie.c -o cronovanie.o > ilog/ilog_cronovanie.o

cdp.o: cdp.c
	gcc -g -c cdp.c -o cdp.o > ilog/ilog_cdp.o

snmpp.o: snmpp.c
	gcc -g -c snmpp.c -o snmpp.o > ilog/ilog_snmpp.o

sip.o: sip.c sip.h
	gcc -g -c sip.c -o sip.o > ilog/ilog_sip.o

clean:
	rm -f *.o > ilog/ilog_clean
	rm -f katalyzer >> ilog/ilog_clean

install:
	cp ./katalyzer /usr/local/bin/katalyzer
	chmod 555 /usr/local/bin/katalyzer
	cp ./my_config.conf /etc/katalyzer/my_config.conf
	cp ../frontend /var/www/katalyzer
	ln -s /etc/katalyzer/my_config.conf /var/www/katalyzer/configs/my_config.conf
