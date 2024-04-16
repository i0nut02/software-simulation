#IDIR=../hdr
IDIR=.
ODIR=../obj
BDIR=../bin

CC=g++
CFLAGS=-std=c++20 -I. -I$(IDIR)

DEPS = $(wildcard $(IDIR)/*.h)
SRC = $(wildcard *.cpp)
OBJ = $(patsubst %.cpp, $(ODIR)/%.o, $(SRC))

#all:	$(ODIR)/%.o

all:	create_dirs $(ODIR)/$(OBJ)

create_dirs:
	mkdir -p $(ODIR)

$(ODIR)/%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(BDIR)/main: $(OBJ) $(DEPS)
	$(CC) -o $@ $(OBJ) $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ $(IDIR)/*~ core 