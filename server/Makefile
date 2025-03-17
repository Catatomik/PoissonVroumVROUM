SRCDIR = src
TESTDIR = tests
BUILDDIR = build
SRCBUILDDIR = $(BUILDDIR)/src
TESTBUILDDIR = $(BUILDDIR)/tests

CFLAGS = -Wall -Werror -I$(SRCDIR)

SRC = $(wildcard $(SRCDIR)/*.c)
SRC_OBJ = $(patsubst $(SRCDIR)/%.c, $(SRCBUILDDIR)/%.o, $(SRC))
SRC_NO_MAIN_OBJ = $(filter-out $(SRCBUILDDIR)/main.o, $(SRC_OBJ))

TEST_SRC = $(wildcard $(TESTDIR)/*.c)
TEST_OBJ = $(patsubst $(TESTDIR)/%.c, $(TESTBUILDDIR)/%.o, $(TEST_SRC))
TEST_BINS = $(patsubst $(TESTBUILDDIR)/%.o, $(TESTBUILDDIR)/%, $(TEST_OBJ))

all: main test

$(SRCBUILDDIR):
	mkdir -p $(SRCBUILDDIR)

$(TESTBUILDDIR):
	mkdir -p $(TESTBUILDDIR)

main: $(SRC_OBJ)
	$(CC) $(CFLAGS) -o main $(SRC_OBJ)


$(SRCBUILDDIR)/%.o: $(SRCDIR)/%.c | $(SRCBUILDDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

test: $(TEST_BINS)
	for test_bin in $(TEST_BINS); do \
	    echo "Running $$test_bin"; \
		./$$test_bin; \
	done

$(TESTBUILDDIR)/%.o: $(TESTDIR)/%.c | $(TESTBUILDDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(TESTBUILDDIR)/%: $(TESTBUILDDIR)/%.o $(SRC_NO_MAIN_OBJ)
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -rf $(BUILDDIR) main test
