#!/bin/bash

apt-get install autoconf
apt-get install m4
apt-get install automake
apt-get install libtool
#install libunwind
tar zxvf libunwind-1.1.tar.gz 
cd libunwind-1.1 
./configure --prefix=/usr/local 
make 
make install #it will be installed at /usr/local/lib
cd ../

#install google-perftools
tar zxvf gperftools-2.7.tar.gz
cd gperftools-2.7
./configure --prefix=/usr/local
make 
make install #it will be installed /usr/local/lib

echo "/usr/local/lib" > /etc/ld.so.conf 
ldconfig
