#! /bin/bash
bison -y -d calc3.y
flex calc3.l
gcc -c y.tab.c lex.yy.c
gcc y.tab.o lex.yy.o calc3b.c -o calc3b.exe