
.PHONY: all tests
.SECONDARY:

OBJS :=  s3.lex.o s3.y.o minicsched.o \
		DDG.o DDGNode.o BasicBlockFinder.o BasicBlock.o \
		ModuloSchedulor.o

all: sched

sched: $(OBJS) cmdline.o
	$(LD) -o $@ $(LDFLAGS) $(LIBS) $^ 

cmdline.c:
	$(LEX) -Pcmd -o$@ cmdline.lex 

tests:
	$(MAKE) -k -C tests all

clean:
	-rm -Rf $(OBJS) cmdline.o cmdline.c
	-rm -f *.y.h *.y.output
	-rm -f *.y.c *.lex.c
	-rm -f sched
	-rm -f sched.exe
	-rm -f *~
	-rm -f ./tests/minic.verbose.txt

wc:
	-wc -l *.c *.y *.lex


minicopt.o:  cmdline.lex s3inst.h
cmdline.o: cmdline.c
cmdline.c: cmdline.lex
s3.y.o : s3inst.h
s3.lex.o: s3.y.c

include Make.defs
include Make.rules
