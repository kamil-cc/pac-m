# Demo program using ncursesw, boost and boost threads, winsock2/Unix sockets

Simple pac-man like game, using standard GNU/Linux terminal and network (sockets) comunication.

# Build
Please use Eclipse Mars 2 or older with g++ or MinGW. Don't forget to check if your compiler supports `-std=gnu++11` option.

# Platform
On GNU/Linux you will need to have `ncursesw` libraries and headers. On Windows (as far as I remember) it is possible to build some version of `ncursesw`. You will have to do it on your own.
Also boost libraries with thread support are needed. 

# Custom Multithreads FIFO
Application relies on custom implementation of multithread FIFO queues. All comunications between threads is based on multithreaded single-reader FIFOs.

# Licesne
All rights reserved. You can not redistribute or sell any part of this code.
However you can clone and build this repo for personal use only.
Details can be found in file: LICENSE.txt
