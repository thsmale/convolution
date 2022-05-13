CC=gcc
CFLAGS=-Wall

#HFILES= fio.h convolution.h debug.h
#CFILES= main.c fio.c convolution.c debug.c

#SRCS= ${HFILES} ${CFILES}
#OBJS= ${CFILES:.c=.o}

#PRODUCT= main fio convolution debug
#all: $(PRODUCT)

main: main.c fio.h convolution.h debug.h /usr/local/lib/libjpeg.a
	$(CC) $(CFLAGS) -o main main.c fio.c convolution.c debug.c /usr/local/lib/libjpeg.a

fio: fio.c fio.h debug.h /usr/local/lib/libjpeg.a
	$(CC) $(CFLAGS) -o fio fio.c debug.c /usr/local/lib/libjpeg.a

convolution: convolution.c convolution.h debug.h /usr/local/lib/libjpeg.a
	$(CC) $(CFLAGS) -o convolution convolution.c debug.c /usr/local/lib/libjpeg.a

debug: debug.c debug.h /usr/local/lib/libjpeg.a
	$(CC) $(CFLAGS) -o debug debug.c /usr/local/lib/libjpeg.a

clean:
	-rm -f *.o *.d
	-rm -f ${PRODUCT}
