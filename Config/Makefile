
CC=g++
ZOOKEEPER_INSTALL=/usr/local
ZOOKEEPER_INC=-I${ZOOKEEPER_INSTALL}/include/zookeeper
ZOOKEEPER_LIB= -L${ZOOKEEPER_INSTALL}/lib -lzookeeper_mt

CPPFLAGS=-Isrc/ -Dprotected=public -Dprivate=public
all: testcase 

testcase:
	$(CC) $(CPPFLAGS) ${ZOOKEEPER_INC} -g -c inifile.cpp
	$(CC) $(CPPFLAGS) ${ZOOKEEPER_INC} -g -c stringutil.cpp
	$(CC) $(CPPFLAGS) ${ZOOKEEPER_INC} -g -c test.cpp
	$(CC) ${CPPFLAGS} ${ZOOKEEPER_INC} ${ZOOKEEPER_LIB} -DTHREAD -g -o testcase  test.o inifile.o stringutil.o
clean:	
	rm -f *.o testcase

