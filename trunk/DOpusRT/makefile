CPU = -m68020

All: ../../DOpusRT

../../DOpusRT: DOpusRT.o
	gcc -o ../../DOpusRT DOpusRT.o $(CPU) -noixemul -resident -s

DOpusRT.o: DOpusRT.c
	gcc -c DOpusRT.c -O3 -Wall -I../include $(CPU) -fomit-frame-pointer -fbaserel -msmall-code

