

# Ein simples Makefile mit zwei Zielen (targets)
BIN      := dialog-test

CC       := gcc
EXT      := .c

SRCDIR   := ../../

SRC      := xml.c dialog.c
SRC      += tests/dialog/$(BIN).c

OBJ      := $(subst $(EXT),.o,$(SRC))
OBJ      := $(addprefix $(SRCDIR), $(OBJ))
SRC      := $(addprefix $(SRCDIR), $(SRC))
TXT      := $(addprefix $(SRCDIR), $(TXT))


CFLAGS   := -Wall `pkg-config --cflags glib-2.0 libxml-2.0` -I../..
LDFLAGS  := 
LIBS     := `pkg-config --libs glib-2.0 libxml-2.0`

# Debug Setup
DBOBJ    := $(subst .o,.do,$(OBJ))
DBFLAGS  := -g3 -ggdb3 -D_DEBUG=1 -Wextra 
ifeq ($(CC),gcc)
	DBFLAGS += -p
endif	

.PHONY: debug clean

all: $(BIN)

$(BIN): $(OBJ)
	@echo "Linking $@ ..."
	@$(CC) -o $@ $(OBJ) $(LDFLAGS) $(LIBS)

debug: $(DBOBJ)
	@echo "Linking $(BIN)-db ..."
	@$(CC) -o $(BIN)-db $(DBOBJ) $(LDFLAGS) $(LIBS) $(DBFLAGS)

%.o : %$(EXT)
	@echo "Compiling $< ..."
	@$(CC) -o $@ -c $< $(CFLAGS)

%.do : %$(EXT)
	@echo "Compiling $< ..."
	@$(CC) -o $@ -c $< $(CFLAGS) $(DBFLAGS)

clean:
	@echo "Clean all"
	-@rm -f $(BIN){,-db} $(OBJ) $(DBOBJ) gmon.out 

