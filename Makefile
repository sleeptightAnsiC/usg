
MAKEFLAGS += --jobs=$(shell nproc)

# CC = cc
CC = gcc
# CC = clang
# CC = tcc

RCPDIR = ./rcp
SRCDIR = ./src
TMPDIR = ./tmp/$(CC)
BINDIR = ./bin/$(CC)

SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(patsubst $(SRCDIR)/%.c,$(TMPDIR)/%.o,$(SRCS))
EXE = $(BINDIR)/$(shell basename $$(pwd))

CFLAGS = @$(RCPDIR)/flags_gcc.txt
# CFLAGS = @$(RCPDIR)/flags_clang.txt
# CFLAGS = @$(RCPDIR)/flags_tcc.txt

CFLAGS += -pipe -O0 -ggdb3
# CFLAGS += -Ofast -static -s -flto -DDBG_DISABLED -DNDEBUG

SANDBOX = $(shell cat $(RCPDIR)/sandbox_gdb.txt)
# SANDBOX = $(shell cat $(RCPDIR)/sandbox_rr.txt)
# SANDBOX = $(shell cat $(RCPDIR)/sandbox_perf.txt)
# SANDBOX = $(shell cat $(RCPDIR)/sandbox_valgrind.txt)

.PHONY: default
default: run compile_commands.json

.PHONY: run
run: build
	@echo "Running $(EXE) ..."
	@$(SANDBOX) $(EXE)

.PHONY: build
build: $(EXE)

.PHONY: clean
clean:
	@echo "Cleaning ..."
	@rm -frv $(shell cat .gitignore)


.PHONY: always
always: ;

$(EXE): $(TMPDIR)/Makefile.mk always $(BINDIR) $(TMPDIR)
	@echo "Building $@ ..."
	@$(MAKE) CC='$(CC)' CFLAGS='$(CFLAGS)' EXE='$(EXE)' --file='$<' --no-print-directory

# TODO: '|| (rm $@ && exit 1)' is a bit ugly but we have to use it
#       because tcc does not support gcc's '-MT'
#       Maybe it would be nice to contribute '-MT' to tcc someday!
# FIXME: I think I missunderstood '-MT' with '-MF'
.PRECIOUS: $(TMPDIR)/%.mk
$(TMPDIR)/%.mk: $(SRCDIR)/%.c | $(TMPDIR)
	@echo "Creating $@ ..."
	@printf "$(TMPDIR)/" > $@
	@$(CC) $(CFLAGS) -MM $< >> $@ || (rm $@ && exit 1)
	@echo "	@echo \"	Compiling \$$@ ...\"" >> $@
	@echo "	@\$$(CC) \$$(CFLAGS) -c \$$< -o \$$@" >> $@

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

