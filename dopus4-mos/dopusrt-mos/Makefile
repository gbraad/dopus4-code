CC = gcc -noixemul
STRIP = strip

CFLAGS = -O3 -Wall -I../include-mos -DUSE_INLINE_STDARG
LDFLAGS =

DOpusRT: main.o
	$(CC) -o $@.debug $^ $(LDFLAGS)
	$(STRIP) --remove-section=.comment $@.debug -o $@

clean:
	@rm -f *.o DOpusRT DOpusRT.debug
