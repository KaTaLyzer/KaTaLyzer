KaTaLyzer
==============

**!!! THIS README IS CURRENTLY WORK IN PROGRESS !!!** Please contact me <a href="mailto://roman.bronis@gmail.com">here</a> if you are interested in _KaTaLyzer_.

What is KaTaLyzer?
----------------------
_KaTaLyzer_ is a network traffic analyzer for Linux based operating systems (routers, servers and desktops). 
It offers full network traffic monitoring. Data are displayed in graphs which can be filtered based on protocol 
as well as communioating nodes. It supports widely used protocols (Ethernet, IP, TCP, UDP, HTTP, SSH, SIP, etc.).


![Image](http://www.katalyzer.sk/images/slide1.png)

_KaTaLyzer_ was tested on Debian, Ubuntu, Arch and Gentoo operating systems.

License:

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This package is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.

Install
----------
**Backend:**
```
cd src/backend
make
```

**Frontend:**
Make frontend folder accessible from your Apache server with soemthing like:
```
cp -r frontend /srv/http/katalyzer
		password: (password is not set)
```
Default login:
```
		login: admin
		password: (password is not set)
```

News
----------
**2014-10-23**
Work in progress!!!
- capturing selection rewrite (again)
- DB with logins as a TABLE
- fixes

**2014-10-07**
- capturing selection rewrite
- initial support for SLL
- revert of change of database name from 2013
```
	Previous style: database-name_iface-name
	New style: database-name
```
- additional modules removal (CDP, SNMP SFLOW, NFLOW, SIP)

**2013**
- support IPv6
- new type of installation
- use socket to capture packet from network, no pcap libraries ( we can still used it)
- change database name.
```
      Old style: database-name
      New style: database-name_interface-name
```

	  
Modules
-----------------------
dead branch
not developed, yet
