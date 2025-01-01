
# MAKEFLAGS += --jobs=$(shell nproc)

# CC = cc
# CC = gcc
# CC = musl-gcc
# CC = clang
# CC = tcc

RCPDIR = ./rcp
SRCDIR = ./src
TMPDIR = ./tmp/$(CC)
BINDIR = ./bin/$(CC)

SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(patsubst $(SRCDIR)/%.c,$(TMPDIR)/%.o,$(SRCS))
EXE = $(BINDIR)/$(shell basename $$(pwd))
# TODO: This should append $(EXE) in case of Windows_NT instead of having two branches
ifeq ($(OS),Windows_NT)
	EXE = $(BINDIR)/$(shell basename $$(pwd)).exe
else
	EXE = $(BINDIR)/$(shell basename $$(pwd))
endif

# CFLAGS = @$(RCPDIR)/flags_gcc.txt
# CFLAGS = @$(RCPDIR)/flags_clang.txt
# CFLAGS = @$(RCPDIR)/flags_tcc.txt

# CFLAGS += -pipe -Og -ggdb3 -D_FORTIFY_SOURCE=3
# CFLAGS += -Ofast -static -s -flto -DDBG_DISABLED -DNDEBUG

# SANDBOX = $(shell cat $(RCPDIR)/sandbox_gdb.txt)
# SANDBOX = $(shell cat $(RCPDIR)/sandbox_rr.txt)
# SANDBOX = $(shell cat $(RCPDIR)/sandbox_perf.txt)
# SANDBOX = $(shell cat $(RCPDIR)/sandbox_valgrind.txt)

.PHONY:
# default: run compile_commands.json
# default: build compile_commands.json
default: build

.PHONY:
run: build
	@echo "Running $(EXE) ..."
	@$(SANDBOX) $(EXE)

.PHONY:
build: $(EXE)

.PHONY:
clean:
	@echo "Cleaning ..."
	@rm -frv $(shell cat .gitignore)


.PHONY:
always: ;

$(EXE): $(TMPDIR)/Makefile.mk always $(BINDIR) $(TMPDIR)
	@echo "Building $@ ..."
	@$(MAKE) CC='$(CC)' CFLAGS='$(CFLAGS)' EXE='$(EXE)' --file='$<' --no-print-directory

.DELETE_ON_ERROR:
$(TMPDIR)/%.mk: $(SRCDIR)/%.c | $(TMPDIR)
	@echo "Creating $@ ..."
	@printf "$(TMPDIR)/" > $@
	@$(CC) $(CFLAGS) -MM $< >> $@
	@echo "	@echo \"	Compiling \$$@ ...\"" >> $@
	@echo "	@\$$(CC) \$$(CFLAGS) -c \$$< -o \$$@" >> $@

.DELETE_ON_ERROR:
$(TMPDIR)/Makefile.mk: $(patsubst $(SRCDIR)/%.c,$(TMPDIR)/%.mk,$(SRCS)) | $(TMPDIR)
	@echo "Creating $@ ..."
	@echo "\$$(EXE): $(OBJS)" > $@
	@echo "	@echo \"	Linking \$$@ ...\"" >> $@
	@echo "	@\$$(CC) \$$(CFLAGS) \$$^ -o \$$@" >> $@
	@echo "	@ln -sfv \$$(EXE) ./\$$(shell basename \$$(EXE))" >> $@
	@for file in $^; do \
		echo "" >> $@; \
		cat $$file >> $@; \
	done \
	;

$(TMPDIR) $(BINDIR):
	@mkdir -pv $@

# https://clang.llvm.org/docs/JSONCompilationDatabase.html
compile_commands.json: $(SRCS)
	@echo "Creating $@ ..."
	@echo "[" > $@
	@for file in $^; do \
		echo "	{" >> $@; \
		echo "		\"file\": \"$$file\"," >> $@; \
		echo "		\"command\": \"clang $$file @$(RCPDIR)/flags_clang.txt -c\"," >> $@; \
		echo "		\"directory\": \"$(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))\"," >> $@; \
		echo "	}," >> $@; \
	done \
	;
	@echo "]" >> $@

