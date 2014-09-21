# gnu make syntax

exe:=step
srcs:=$(wildcard *.cpp)
objs:=$(srcs:.cpp=.o)
hdrs:=$(wildcard *.h)
LFLAGS:=$(shell pkg-config --libs clanApp-3.0 clanDisplay-3.0 clanCore-3.0 clanGL-3.0 clanSWRender-3.0 clanPhysics2D-3.0)
CFLAGS:=$(shell pkg-config --cflags clanApp-3.0 clanDisplay-3.0 clanCore-3.0 clanGL-3.0 clanSWRender-3.0 clanPhysics2D-3.0)
CFLAGS+=-g -Wall
# CFLAGS+=-std=c++11
# CFLAGS+=-I/usr/src/ClanLib-3.0.1/Sources/Physics2D
PREH := precomp.h
PREHC := $(PREH).gch
CHFLAGS := $(CFLAGS)
CFLAGS+=-include $(PREH)

$(warning sources: $(srcs))

$(exe): $(objs)
	g++ -o $@ $(objs) $(LFLAGS) 

$(PREHC): $(PREH) defs.h Makefile
	g++ $(CHFLAGS) -pthread $(PREH)
%.o:%.cpp $(hdrs) Makefile $(PREHC)
	g++ $(CFLAGS) -pthread -c $< -o $@
.PHONY: clean
clean:
#	rm $(exe) *.o
	rm *.o *.gch
build: $(exe)
