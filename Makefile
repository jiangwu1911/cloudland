CPP = g++
CPPFLAGS = -g -Wall -Icommon -Ifilter -I/opt/sci/include
LDFLAGS = -Lcommon -lcmn -L/opt/sci/lib64 -lsci
ARFLAGS = -rv

all: libcmn.a cloudland cloudlet cloudctl sendmsg
	
libcmn.a:
	$(MAKE) -C common || exit 1

cloudland: cloudland.o netlayer.o handler.o worker.o
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

cloudctl: cloudctl.o
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

sendmsg: sendmsg.o
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

cloudlet: cloudlet.o netlayer.o handler.o
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

cloudland.o: cloudland.cpp
	$(CPP) $(CPPFLAGS) -c -o $@ $< $(LDFLAGS)

cloudctl.o: cloudctl.cpp
	$(CPP) $(CPPFLAGS) -c -o $@ $< $(LDFLAGS)

sendmsg.o: sendmsg.cpp
	$(CPP) $(CPPFLAGS) -c -o $@ $< $(LDFLAGS)

worker.o: worker.cpp
	$(CPP) $(CPPFLAGS) -c -o $@ $< $(LDFLAGS)

cloudlet.o: cloudlet.cpp
	$(CPP) $(CPPFLAGS) -c -o $@ $< $(LDFLAGS)

handler.o: handler.cpp
	$(CPP) $(CPPFLAGS) -c -o $@ $< $(LDFLAGS)

clean:
	rm -f *.o common/*.o common/*.a filter/*.so filter/*.o cloudland cloudlet cloudctl sendmsg

install:
	cp cloudland cloudlet cloudctl sendmsg /opt/cloudland/bin
