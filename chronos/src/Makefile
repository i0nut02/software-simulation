#IDIR=../hdr
IDIR=.
ODIR=../obj
BDIR=../bin

CC=g++
CFLAGS=-std=c++20 -I. -I$(IDIR) -lhiredis

DEPS = $(wildcard $(IDIR)/*.h)
SRC = $(wildcard *.cpp)
OBJ = $(patsubst %.cpp, $(ODIR)/%.o, $(SRC))

con2redis_OBJ=$(wildcard ../../con2redis/obj/*.o)
logger_OBJ=$(wildcard ../../logger/obj/*.o)

#all:	$(ODIR)/%.o

all:	create_dirs con2redis logger $(ODIR)/$(OBJ) $(BDIR)/main

create_dirs:
	mkdir -p $(ODIR)
	mkdir -p $(BDIR)

con2redis:
	$(MAKE) -C ../../con2redis/src/

logger:
	$(MAKE) -C ../../logger/src

$(ODIR)/%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(BDIR)/main: $(OBJ) $(DEPS)
	$(CC) -o $@ $(OBJ) $(con2redis_OBJ) $(logger_OBJ) $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ $(IDIR)/*~ core 