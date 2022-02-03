#!/bin/sh
lcc -c first.c -v
echo calling linker...
ld -s /usr/lib/default.lnk -L/usr/lib -o first first.o /usr/lib/crt0.o -lc