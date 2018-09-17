#/bin/sh

make clean
make
cp -r ./Debug/ftgm ftgm-50
atftp 20.20.20.195 -p -l ftgm-50
