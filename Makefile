.SUFFIXES: .c .cpp .o .x .h
DIR = $(notdir $(CURDIR))
SYSNAME = $(shell uname -s)
NAME1 = ans
NAME2 = alg
NAME3 = simpler
NAME4 = scaled

EXEC1  = $(NAME1).x
EXEC2  = $(NAME2).x
EXEC3  = $(NAME3).x
EXEC4  = $(NAME4).x

OBJS1  = $(NAME1).o
OBJS2  = $(NAME2).o
OBJS3  = $(NAME3).o
OBJS4  = $(NAME4).o

ifeq ($(SYSNAME), Linux)
  LIB_SHAR = lib$(LIB_NAME).so
endif

ifeq ($(SYSNAME), Darwin)
  LIB_SHAR = lib$(LIB_NAME).dylib
endif

LDLIBS = -lm

INPUT= 
MODE=
PS1=$'\\[\E[0;1;3;38;5;136m\\]blahblah$ \\[\E[m\\]'

CO = g++
LD = $(CO)

CFLAGS = -std=c++20 -O2

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
$(EXEC4): $(OBJS4)
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
	@tput setaf 2;
	@printf '%*s\n' $$(tput cols) '' | tr ' ' '-'
#@echo '--------------------------------------------------------------------------------';
	@echo '\t\t\t   ***** COMPRESSION ******';
	@tput setaf 7;
	@./$(EXEC3) $(MODE) $(INPUT)
	@tput setaf 2;
	@echo '\n\t\t           ****** FINISHED ******';
	@printf '%*s\n' $$(tput cols) '' | tr ' ' '-'
#@echo '--------------------------------------------------------------------------------';
###############################################################################
.PHONY: diff
diff:
	@tput setaf 2;
	@echo '\n';
	@diff -s $(INPUT).txt $(INPUT)_compressed_decompressed.txt
	@echo '\n';
###############################################################################
scaled: $(EXEC4)
#clear;
	@echo '\t\t\t   ****** SCALED ******';
	./$(EXEC4) $(INPUT)
###############################################################################
.PHONY: clean tar
clean:                                                     
	rm -f *.o  *~ *.a *.so *.x core core* a.out; rm -rf ${LIB_DIR}

tar: clean
	(cd ../; tar -cvzf $(DIR).tar.gz  $(DIR) )
