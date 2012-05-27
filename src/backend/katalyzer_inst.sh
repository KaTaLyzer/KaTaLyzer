#!/bin/bash
# KaTaLyzer backend installation script v0.4 2009/05/05
# Stanislav bocinec (svacko@gmail.com)

PATH=/usr/bin:/bin:/usr/sbin:/sbin

VERSION="v0.1"

CONF_DIR=/etc/katalyzer
CONF_NAME="my_config.conf"
INIT_DIR=/etc/init.d
INST_DIR_DFLT=/usr/local/bin

INPUT=""
DIR_OK=0
START=0

echo -e "\n\t*** KaTaLyzer backend installation ***\n"
echo -e "\tThis will install KaTaLyzer backend $VERSION on your computer. Press ENTER to continue."
echo -e "\tPress Ctrl+c to cancel installation at any time."
read continue < /dev/tty

if [[ $EUID -ne 0 ]]; then
	echo "KaTaLyzer backend installation script must be run as ROOT, exitting" 1>&2
	exit 1
fi

check_inst_dir () {
	CHK_DIR="$1"
	if [ -d "$CHK_DIR" ]; then
		if [ ! -d "$CHK_DIR/katalyzer" ]; then
			if [ -w "$CHK_DIR" ]; then
				echo "ok"
				return	
			else
				echo "not-writable"
				return
			fi
		else
			echo "exists"
			return
		fi
	else 
		echo "not-exists"
		return
	fi
}

fix_dir () {
	FIX_DIR="$1"
	FIRSTCHAR=`expr "$FIX_DIR" : '\(.\).*'`
	if [ "$FIRSTCHAR" != '/' ]; then
		currentdir=`pwd`
		echo "$currentdir/$FIX_DIR"
	else
		echo "$1"
	fi
}

get_install_dir () {
	echo "Please enter KaTaLyzer backend installation path [$INST_DIR_DFLT]: "
	read INPUT
	if [ -z $INPUT ]; then
		INST_DIR=$INST_DIR_DFLT
	else
		INST_DIR=$INPUT
	fi
}

while [ "$DIR_OK" -ne 1 ]
do 
	get_install_dir
	INST_DIR=`fix_dir $INST_DIR`
	CHK_STAT=`check_inst_dir $INST_DIR`
	case $CHK_STAT in
	  not-exists)
		echo "Directory $INST_DIR does not exist. Please specify new installation path"
		;;
	  exists)
		echo "File $INST_DIR/katalyzer already exists. Please remove it and specify new installation path"
		;;
	  not-writable)
		echo "Directory $INST_DIR is not writable, please change permissions or specify new directory"
		;;
	  ok)
		INST_DIR=$INST_DIR
		DIR_OK=1
	esac
done

echo "---"
echo "KaTaLyzer backend installation script will proceed following installations steps:"
echo "1.Compile KaTaLyzer backend from the source"
echo "2.Install KaTaLyzer backend files to following locations:"
echo -e "\tcompiled binaries -> $INST_DIR/"
echo -e "\tsample configuration file -> $CONF_DIR/$CONF_NAME"
echo "---"
echo "Continue? [y/n]"
while [ $START -ne 1 ]
do
	read INPUT
	if [ "$INPUT" == "y" ]; then
		START=1
	elif [ "$INPUT" == "n" ]; then
		exit 0
	else
		echo "Continue? [y/n]"
	fi
done

echo "---"
echo "Building KaTaLyzer..."
make

if [ "$?" -ne 0 ]; then
	echo "Unable to build KaTaLyzer. Read the output error and try to fix the problem. If you are not able to solve the problem, please contact developer"
	exit 1
fi

echo "OK"
echo "---"
mkdir -p $INST_DIR
trap 'rm -rf ${INST_DIR}; exit 1' HUP INT QUIT TERM
echo -n "Installing KaTaLyzer binary to $INST_DIR... "
cp -f ./katalyzer $INST_DIR
if [ $? -eq 0 ]; then
	echo OK
else
	echo -n "\nError during copying KaTaLyzer backend  binary to $INST_DIR. Check the destination and try to copy the binary manually."
fi
chmod 750 $INST_DIR/katalyzer 
echo "---"
echo -n "Installing KaTaLyzer backend configuration file $CONF_NAME to $CONF_DIR... "

if [ ! -d "$CONF_DIR" ]; then 
	mkdir /etc/katalyzer
fi
cp -f files/$CONF_NAME $CONF_DIR/
if [ $? -eq 0 ]; then
	echo OK
else
	echo -n "\nError during copying KaTaLyzer backend configuration file to $CONF_DIR. Check the destination and try to copy example configuration file manually"
fi
chmod 644 $CONF_DIR/$CONF_NAME
echo "---"
echo "Installation of KaTaLyzer backend was successful"
echo "Edit the configuration file before running KaTaLyzer backend!"
echo "---"
exit 0
