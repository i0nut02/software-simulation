#IDIR=../hdr
IDIR=.
ODIR=../obj
BDIR=../bin

CC=g++
# use this when connecting to Postgresql (library lpq)
#CFLAGS=-std=c++11 -I. -I$(IDIR) -I/usr/include/postgresql -lpq -lm 
CFLAGS=-std=c++20 -I. -I$(IDIR) -I/usr/include/postgresql -lpq -lm -lhiredis

# use this when no connection to Postgresql (library lpq) is needed
#CFLAGS=-std=c++11 -I. -I$(IDIR) -lm 

DEPS = $(wildcard $(IDIR)/*.h)
#OBJ = $(wildcard $(ODIR)/*.o)
SRC = $(wildcard *.cpp)
OBJ = $(patsubst %.cpp, $(ODIR)/%.o, $(SRC))

con2redis_OBJ=$(wildcard ../../con2redis/obj/*.o)
con2pgsql_OBJ=$(wildcard ../../con2db/*.o)

#all:	$(ODIR)/%.o

all:	create_dirs con2redis con2pgsql $(ODIR)/$(OBJ)

create_dirs:
	mkdir -p $(ODIR)

con2redis:
	$(MAKE) -C ../../con2redis/src/

con2pgsql:
	$(MAKE) -C ../../con2db/

$(ODIR)/%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)


.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ $(IDIR)/*~ core 
