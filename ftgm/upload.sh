#/bin/sh

make clean
make
cp -r ./Debug/ftgm ftgm-50
atftp 10.0.1.90 -p -l ftgm-50
