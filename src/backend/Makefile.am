bin_PROGRAMS = katalyzer
katalyzer_SOURCES = cdp.c cronovanie.c nflow_sflow.c processing.c read_conf.c sip.c snmpp.c variables.c wait_function.c socket/ksocket.c katalyzer.c
katalyzer_LDADD = $(PCAP_LIBS)
katalyzer_LDFLAGS = '-lpthread' '-lmysqlclient' '-I/usr/include/mysql'

katalyzerconfdir=/etc/katalyzer
katalyzerconf_DATA = files/config.conf
