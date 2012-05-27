SUBDIRS = src/backend
dist_doc_DATA = README

install-data-local:
	cp src/backend/start_katalyzer /etc/init.d/katalyzer
	chmod 755 /etc/init.d/katalyzer
	mysql -u root -p < ./SQL/frontend.sql
	cp -r ./src/frontend /var/www/katalyzer
	ln -s /etc/katalyzer/config.conf /var/www/katalyzer/configs/config.conf
uninstall-local:
	rm /etc/init.d/katalyzer
	rm -rf /var/www/katalyzer
