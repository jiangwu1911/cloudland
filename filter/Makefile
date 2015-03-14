CPP = g++
AR = ar
CPPFLAGS = -g -Wall -I../common -I/home/cloud/sci/include -shared -fPIC
LDFLAGS = -L../common -lcmn -L/home/cloud/sci/libsci/.libs -lsci 

all: scheduler.so

scheduler.so: scheduler.o rcmanager.o
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

scheduler.o: scheduler.cpp rcmanager.o
	$(CPP) $(CPPFLAGS) -c -o $@ $<

rcmanager.o: rcmanager.cpp
	$(CPP) $(CPPFLAGS) -c -o $@ $< $(LDFLAGS)

clean:
	rm -f *.so *.o

install:
	cp *.so /opt/cloudland/lib64
