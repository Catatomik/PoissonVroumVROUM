VG ?= valgrind
CFMT ?= clang-format

SRCDIR = src
TESTDIR = tests
BUILDDIR = build
SRCBUILDDIR = $(BUILDDIR)/src
TESTBUILDDIR = $(BUILDDIR)/tests

CFLAGS = -Wall -Werror -I$(SRCDIR) -pthread
LDFLAGS = -lm
CFMTFLAGS ?= -style='file:.clang-format'
VFLAGS ?= --leak-check=full --errors-for-leak-kinds=all --error-exitcode=1

SRC = $(wildcard $(SRCDIR)/*.c)
SRC_OBJ = $(patsubst $(SRCDIR)/%.c, $(SRCBUILDDIR)/%.o, $(SRC))
SRC_NO_MAIN_OBJ = $(filter-out $(SRCBUILDDIR)/main.o, $(SRC_OBJ))

TEST_BINS = $(patsubst $(TESTDIR)/%.c, $(TESTBUILDDIR)/%, $(wildcard $(TESTDIR)/*.c))
TEST_BINS_EXEC = $(patsubst $(TESTBUILDDIR)/%, %.test, $(TEST_BINS))
TEST_BINS_EXEC_VG = $(patsubst $(TESTBUILDDIR)/%, %.vg, $(TEST_BINS))

all: main $(TEST_BINS)

$(SRCBUILDDIR):
	mkdir -p $(SRCBUILDDIR)

$(TESTBUILDDIR):
	mkdir -p $(TESTBUILDDIR)

$(SRCBUILDDIR)/%.o: $(SRCDIR)/%.c | $(SRCBUILDDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

main: $(SRC_OBJ)
	$(CC) $(CFLAGS) -o $@ $(SRC_OBJ) $(LDFLAGS)

$(TESTBUILDDIR)/%.o: $(TESTDIR)/%.c | $(TESTBUILDDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

# In order to keep intermediate files
.PRECIOUS: $(TESTBUILDDIR)/%.o

$(TESTBUILDDIR)/%: $(TESTBUILDDIR)/%.o $(SRC_NO_MAIN_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Run a single test
# Uses static pattern (https://www.gnu.org/software/make/manual/html_node/Static-Pattern.html)
# In order to suggest a test bin to run in shell with tab
$(TEST_BINS_EXEC): %.test: $(TESTBUILDDIR)/%
	printf '\t- %s: ' $@
	timeout 5 ./$< >/dev/null && echo 'succès' || echo 'échec'

# Run all tests
test: $(TEST_BINS_EXEC)

ci-test:
	@exit $$(make -s test | sed -e '/succès$$/d' | tee /dev/stderr |  wc -l)

# Run a single test with valgrind
$(TEST_BINS_EXEC_VG): %.vg: $(TESTBUILDDIR)/%
	printf '\t- %s: ' $@
	timeout 5 valgrind $(VFLAGS) ./$< >/dev/null 2>&1 && echo 'succès' || echo 'échec'

valgrind: $(TEST_BINS_EXEC_VG)

ci-valgrind:
	@exit $$(make -s valgrind | sed -e '/succès$$/d' | tee /dev/stderr |  wc -l)

# Formats all files in source dirs (src, test)
format:
	$(CFMT) $(CFMTFLAGS) -i $(wildcard $(SRCDIR)/*.c) $(wildcard $(SRCDIR)/*.h) $(wildcard $(TESTDIR)/*.c)

# Check formatting according to previous rule
check-format:
	$(CFMT) $(CFMTFLAGS) --dry-run --Werror $(wildcard $(SRCDIR)/*.c) $(wildcard $(SRCDIR)/*.h) $(wildcard $(TESTDIR)/*.c)

clean:
	rm -rf $(BUILDDIR) main

# Rules that don't produce files
.PHONY: all $(TEST_BINS_EXEC) test ci-test $(TEST_BINS_EXEC_VG) valgrind ci-valgrind format check-format clean
# Do not print executed commands while running tests, for cleaner output
.SILENT: $(TEST_BINS_EXEC) $(TEST_BINS_EXEC_VG)
