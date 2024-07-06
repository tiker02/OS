CC=gcc
CCOPTS=--std=gnu99 -g -Wall 

BINS=serial_client

HEADERS = serial_client.h
#disastros_test

.phony: clean all


all:	$(BINS)

serial_client:		serial_client.c $(LIBS)
	$(CC) $(CCOPTS) -o $@ $^

clean:
	rm -rf *.o *~ $(LIBS) $(BINS)