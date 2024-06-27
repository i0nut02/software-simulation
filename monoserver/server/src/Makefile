#IDIR=../hdr
IDIR=.
ODIR=../obj
BDIR=../bin

CC=g++
CFLAGS=-std=c++20 -I. -I$(IDIR) -lhiredis -I/usr/include/postgresql -lpq -lm

DEPS = $(wildcard $(IDIR)/*.h)
SRC = $(wildcard *.cpp)
OBJ = $(patsubst %.cpp, $(ODIR)/%.o, $(SRC))

chronolib_OBJ=$(wildcard ../../../chronos_lib/obj/*.o)
con2redis_OBJ=$(wildcard ../../../con2redis/obj/*.o)
con2db_OBJ=$(wildcard ../../../con2db/*.o)

#all:	$(ODIR)/%.o

all:	create_dirs chronolib con2redis con2db $(ODIR)/$(OBJ) $(BDIR)/main

create_dirs:
	mkdir -p $(ODIR)
	mkdir -p $(BDIR)

chronolib:
	$(MAKE) -C ../../../chronos_lib/src/

con2redis:
	$(MAKE) -C ../../../con2redis/src/

con2db:
	$(MAKE) -C ../../../con2db

$(ODIR)/%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(BDIR)/main: $(OBJ) $(DEPS)
	$(CC) -o $@ $(OBJ) $(chronolib_OBJ) $(con2redis_OBJ) $(con2db_OBJ) $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ $(IDIR)/*~ core 