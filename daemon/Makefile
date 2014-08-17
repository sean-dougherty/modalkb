DIR_BLD=../bld/daemon
DIR_BIN=../bin

CPPS=$(shell ls *.cpp)
OBJS=$(CPPS:%.cpp=../bld/daemon/%.o)
HEADERS=$(shell ls *.h)

DAEMON=${DIR_BIN}/modalkb

CC_FLAGS=-g -fno-inline -ggdb -Wall -std=c++0x
LD_FLAGS=-L/usr/lib/x86_64-linux-gnu -lX11

default: ${DAEMON}

clean:
	rm -rf ${DIR_BLD}
	rm -f ${DAEMON}

install: ${DAEMON}
	@if pgrep modalkb > /dev/null; then pkill modalkb; fi
	cp ${DAEMON} ~/bin

${DAEMON}: ${OBJS} ${HEADERS} Makefile
	@mkdir -p ${DIR_BIN}
	g++ ${OBJS} -o $@ ${LD_FLAGS}

${DIR_BLD}/%.o: ./%.cpp
	@mkdir -p ${DIR_BLD}
	g++ -c -o $@ ${CC_FLAGS} $<
