ordering: ordering.cpp
	gcc -o ordering -O2 ordering.cpp -lpthread

asm2: 
	gcc -O2 -c -S -masm=intel  ordering.cpp

asm:
	gcc -O2 -c -g -Wa,-a,-ad -masm=intel ordering.cpp > ordering.lst
