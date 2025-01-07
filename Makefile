.SUFFIXES: .c .cpp .o .x .h
DIR = $(notdir $(CURDIR))
SYSNAME = $(shell uname -s)
NAME1 = ans
NAME2 = alg
NAME3 = simpler

EXEC1  = $(NAME1).x
EXEC2  = $(NAME2).x
EXEC3  = $(NAME3).x

OBJS1  = $(NAME1).o
OBJS2  = $(NAME2).o
OBJS3  = $(NAME3).o

ifeq ($(SYSNAME), Linux)
  LIB_SHAR = lib$(LIB_NAME).so
endif

ifeq ($(SYSNAME), Darwin)
  LIB_SHAR = lib$(LIB_NAME).dylib
endif

LDLIBS = -lm

INPUT= 

CO = g++
LD = $(CO)

CFLAGS = -std=c++20

AR = ar

ARFLAGS = rsv

%.o: %.cpp
	$(CO) $(CFLAGS) -c $<

.PHONY: all
all: $(EXEC1) $(EXEC2) $(EXEC3)

$(EXEC1): $(OBJS1) 
	$(LD) -o $@ $(LFLAGS) $^
$(EXEC2): $(OBJS2)
	$(LD) -o $@ $(LFLAGS) $^
$(EXEC3): $(OBJS3)
	$(LD) -o $@ $(LFLAGS) $^
###############################################################################
.PHONY: run
###############################################################################
run: $(EXEC1)
	@echo '****** RUN ******';
	./$(EXEC1)
###############################################################################
test: $(EXEC2)
	@echo '****** TEST ******';
	./$(EXEC2) $(INPUT)
###############################################################################
simple: $(EXEC3)
	clear;
	@echo '\t\t\t\t****** TEST ******';
	./$(EXEC3) $(INPUT)
###############################################################################
.PHONY: clean tar
clean:                                                     
	rm -f *.o  *~ *.a *.so *.x core core* a.out; rm -rf ${LIB_DIR}

tar: clean
	(cd ../; tar -cvzf $(DIR).tar.gz  $(DIR) )
