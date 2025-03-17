SRCDIR = src
TESTDIR = tests
BUILDDIR = build
SRCBUILDDIR = $(BUILDDIR)/src
TESTBUILDDIR = $(BUILDDIR)/tests

CFLAGS = -Wall -Werror -I$(SRCDIR)

SRC = $(wildcard $(SRCDIR)/*.c)
SRC_OBJ = $(patsubst $(SRCDIR)/%.c, $(SRCBUILDDIR)/%.o, $(SRC))
SRC_NO_MAIN_OBJ = $(filter-out $(SRCBUILDDIR)/main.o, $(SRC_OBJ))

TEST_BINS = $(patsubst $(TESTDIR)/%.c, $(TESTBUILDDIR)/%.test, $(wildcard $(TESTDIR)/*.c))
TEST_BINS_EXEC = $(patsubst $(TESTBUILDDIR)/%.test, %, $(TEST_BINS))

all: main test

$(SRCBUILDDIR):
	mkdir -p $(SRCBUILDDIR)

$(TESTBUILDDIR):
	mkdir -p $(TESTBUILDDIR)

$(SRCBUILDDIR)/%.o: $(SRCDIR)/%.c | $(SRCBUILDDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

main: $(SRC_OBJ)
	$(CC) $(CFLAGS) -o $@ $(SRC_OBJ)

$(TESTBUILDDIR)/%.o: $(TESTDIR)/%.c | $(TESTBUILDDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

# In order to keep intermediate files
.PRECIOUS: $(TESTBUILDDIR)/%.o

$(TESTBUILDDIR)/%.test: $(TESTBUILDDIR)/%.o $(SRC_NO_MAIN_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Run a single test
$(TEST_BINS_EXEC): %: $(TESTBUILDDIR)/%.test
	printf '\t- %s: ' $@
	./$< >/dev/null && echo 'succès' || echo 'échec'

# Run all tests
test: $(TEST_BINS_EXEC)

clean:
	rm -rf $(BUILDDIR) main

# Rules that don't produce files
.PHONY: all test clean
# 
.SILENT: $(TEST_BINS_EXEC)
