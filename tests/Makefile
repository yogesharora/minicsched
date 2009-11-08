MCC = ../copt
SIM = ./s3sim

.SUFFIXES: %.o %.c

CFLAGS =

ifdef VERBOSE
CFLAGS += -v
endif

tests :=  \
	graded_001.opt \
	graded_002.opt \
	graded_003.opt \
	test_000.opt \
	test_001.opt \
	test_002.opt \
	test_003.opt \
	test_004.opt \
	test_005.opt \
	test_007.opt \
	test_008.opt \
	test_009.opt \
	test_010.opt \
	test_011.opt \
	test_014.opt \


%.opt: %.asm
ifdef DEBUG
	gdb --args $(MCC) $(CFLAGS) -o $@ $<
else
	$(MCC) $(CFLAGS) -o $@ $<
ifdef SIMULATE
ifdef COMPARE
	$(SIM) $<
endif
ifdef VERBOSE
	$(SIM) -v $@
else
	$(SIM) $@
endif
endif	
endif

all: clean $(tests)

clean:
	rm -Rf *.opt	
