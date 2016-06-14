CC=gcc
CFLAGS=-g 

ZOOKEEPER_INSTALL=/usr/local
ZOOKEEPER_INC=-I${ZOOKEEPER_INSTALL}/include/zookeeper
ZOOKEEPER_LIB= -L${ZOOKEEPER_INSTALL}/lib -lzookeeper_mt

APP=producer consumer
all:
	${CC} producer.c -DTHREAD ${CFLAGS} ${ZOOKEEPER_INC} ${ZOOKEEPER_LIB} -o producer
	${CC} consumer.c -DTHREAD ${CFLAGS} ${ZOOKEEPER_INC} ${ZOOKEEPER_LIB} -o consumer
clean:
	rm -f ${APP}
