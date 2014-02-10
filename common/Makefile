CPP = g++
AR = ar
CPPFLAGS = -g -Wall -Icommon
LDFLAGS = -Lcommon
ARFLAGS = -rv

libcmn.a: log.o unixsock.o exception.o
	$(AR) $(ARFLAGS) -o $@ $^

log.o: log.cpp
	$(CPP) $(CPPFLAGS) -c -o $@ $<

unixsock.o: unixsock.cpp
	$(CPP) $(CPPFLAGS) -c -o $@ $<

exception.o: exception.cpp
	$(CPP) $(CPPFLAGS) -c -o $@ $<

clean:
	rm -f libcmn.a *.o
