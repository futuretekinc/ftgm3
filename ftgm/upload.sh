#/bin/sh

make clean
make
cp -r ./Debug/ftgm ftgm-100
atftp 10.0.1.90 -p -l ftgm-100
