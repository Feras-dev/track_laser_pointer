INCLUDE_DIRS = -I/usr/include/opencv4
LIB_DIRS = 
CC=g++

CDEFS=
CFLAGS= -O0 -g $(INCLUDE_DIRS) $(CDEFS)
LIBS= -lrt
CPPLIBS= -L/usr/lib -lopencv_core -lopencv_flann -lopencv_video -lstdc++fs

HFILES= 
CFILES= 
CPPFILES= lock_on_target.cpp

SRCS= ${HFILES} ${CFILES}
CPPOBJS= ${CPPFILES:.cpp=.o}

all:	lock_on_target

clean:
	-rm -f *.o *.d cvtest*.ppm cvtest*.pgm test*.ppm test*.pgm
	-rm -f lock_on_target

distclean:
	-rm -f *.o *.d
	
lock_on_target: lock_on_target.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $@.o `pkg-config --libs opencv4` $(CPPLIBS)

depend:

.c.o:
	$(CC) $(CFLAGS) -c $<

.cpp.o:
	$(CC) $(CFLAGS) -c $<
