BIN        = msh

TEST_FILES = $(wildcard tests/*.c)
TEST_OBJS  = $(patsubst %.c,%.o,$(TEST_FILES))
TEST_DEPS  = $(patsubst %.c,%.d,$(TEST_FILES))
TEST_BIN   = $(sort $(patsubst %.c,%.test,$(TEST_FILES)))
LIBDIR     = mshparse
INCDIRS    = . tests util ln $(LIBDIR)

CC       = gcc
# generate files that encode make rules for the .h dependencies
DEPFLAGS = -MP -MD
# automatically add the -I onto each include directory
CFLAGS   = -Wall -Wextra -Werror -Wno-unused-function -g $(foreach D,$(INCDIRS),-I$(D)) -O0 $(DEPFLAGS)

# for-style iteration (foreach) and regular expression completions (wildcard)
CFILE    = $(wildcard *.c)
# regular expression replacement
OBJECT   = $(patsubst %.c,%.o,$(CFILE))
DEPFILE  = $(patsubst %.c,%.d,$(CFILE))

LIBFILES = $(wildcard $(LIBDIR)/*.c)
LIBOBJS  = $(patsubst %.c,%.o,$(LIBFILES))
LIBDEPS  = $(patsubst %.c,%.d,$(LIBFILES))
LIBS     = libln.a libmshparse.a

SHTESTS  = $(sort $(wildcard tests/m*.txt))

LD       = gcc
LDFLAGS  = -L. -lmshparse -lln

DOC_OUT  = README.pdf

UTIL     = util
UTIL_URL = https://github.com/gwu-cs-sysprog/utils/raw/main/util.tgz
LN       = ln
LN_URL   = https://github.com/gwu-cs-sysprog/linenoise.git

all: $(UTIL) $(LN) prebin $(TEST_BIN)

$(UTIL):
	wget $(UTIL_URL)
	tar zxvf util.tgz
	rm util.tgz

$(LN):
	git clone $(LN_URL) $(LN)

%.test: %.o
	$(LD) -o $@ $< $(LDFLAGS)

%.o:%.c
	$(CC) $(CFLAGS) -c -o $@ $<

### Products ###

libln.a: $(LN)/linenoise.o
	$(AR) -crs $@ $^

libmshparse.a: $(LIBOBJS)
	$(AR) -crs $@ $^

prebin: libmshparse.a libln.a $(BIN)

$(BIN): $(OBJECT)
	$(LD) -o $@ $^ $(LDFLAGS)

test: all $(UTIL)
	@echo "Running tests..."
	$(foreach T, $(TEST_BIN), ./$(T);)
	$(foreach T, $(SHTESTS), sh tests/shell_check.sh $(T);)
	@echo "\nRunning valgrind tests..."
	$(foreach T, $(TEST_BIN), sh util/valgrind_test.sh ./$(T);)
	$(foreach T, $(SHTESTS), sh tests/shell_check_valgrind.sh $(T);)
## 	@echo "\nRunning symbol visibility test..."
## 	sh tests/assess_visibility.sh "ptrie_add\|ptrie_allocate\|ptrie_autocomplete\|ptrie_free\|ptrie_print\|ptrie_test_eval" $(LIB)

%.pdf: %.md
	pandoc -V geometry:margin=1in $^ -o $@

doc: $(DOC_OUT)

clean:
	rm -rf $(TEST_BIN) $(TEST_DEPS) $(TEST_OBJS) $(OBJECT) $(DEPFILE) $(DOC_OUT) $(LIBS) $(BIN) $(LIBOBJS) $(LIBDEPS)

clean_all: clean
	rm -rf $(LN) $(UTIL)

.PHONY: all test clean doc prebin

# include the dependencies
-include $(DEPFILE) $(TEST_DEPS) $(LIBDEPS)
