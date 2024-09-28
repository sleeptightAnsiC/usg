
# CC = c99
CC = gcc
# CC = clang
# CC = tcc

RCPDIR = ./rcp
SRCDIR = ./src
TMPDIR = ./tmp/$(CC)
BINDIR = ./bin/$(CC)

SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(patsubst $(SRCDIR)/%.c,$(TMPDIR)/%.o,$(SRCS))
EXE = $(BINDIR)/usg

# CFLAGS = @$(RCPDIR)/flags_cc.txt
CFLAGS = @$(RCPDIR)/flags_gcc.txt
# CFLAGS = @$(RCPDIR)/flags_clang.txt
# CFLAGS = @$(RCPDIR)/flags_tcc.txt

CFLAGS += -pipe
CFLAGS += -g3

CFLAGS += -O0
# CFLAGS += -O3
# CFLAGS += -Ofast

SANDBOX = $(shell cat $(RCPDIR)/sandbox_gdb.txt)
# SANDBOX = $(shell cat $(RCPDIR)/sandbox_rr.txt)
# SANDBOX = $(shell cat $(RCPDIR)/sandbox_perf.txt)
# SANDBOX = $(shell cat $(RCPDIR)/sandbox_valgrind.txt)


.PHONY: run
run: build
	$(SANDBOX) $(EXE)

# WARN: hardcoded requirement for creating directories
.PHONY: build
build: $(TMPDIR) $(BINDIR) compile_commands.json $(EXE)

.PHONY: clean
clean:
	rm -frv $(shell cat .gitignore)


.PHONY: always
always: ;

$(EXE): $(TMPDIR)/Makefile.mk always
	$(MAKE) CC='$(CC)' CFLAGS='$(CFLAGS)' EXE='$(EXE)' --file='$<'

# TODO: '|| (rm $@ && exit 1)' is a bit ugly but we have to use it
#       because tcc does not support gcc's '-MT'
#       Maybe it would be nice to contribute '-MT' to tcc someday!
# FIXME: I think I missunderstood '-MT' with '-MF'
.PRECIOUS: $(TMPDIR)/%.mk
$(TMPDIR)/%.mk: $(SRCDIR)/%.c
	printf "$(TMPDIR)/" > $@
	$(CC) $(CFLAGS) -MM $^ >> $@ || (rm $@ && exit 1)
	echo "	\$$(CC) \$$(CFLAGS) -c \$$< -o \$$@" >> $@

$(TMPDIR)/Makefile.mk: $(patsubst $(SRCDIR)/%.c,$(TMPDIR)/%.mk,$(SRCS))
	echo "\$$(EXE): $(OBJS)" > $@
	echo "	\$$(CC) \$$(CFLAGS) \$$^ -o \$$@" >> $@
	echo "	ln -sfv \$$(EXE) ./executable" >> $@
	for file in $^; do \
		echo "" >> $@; \
		cat $$file >> $@; \
	done \
	;

$(TMPDIR) $(BINDIR): $@
	mkdir -p $@

compile_commands.json: $(SRCS)
	echo "[" > $@
	for file in $^; do \
		echo "	{" >> $@; \
		echo "		\"file\": \"$$file\"," >> $@; \
		echo "		\"command\": \"clang $$file @$(RCPDIR)/flags_clang.txt -c\"," >> $@; \
		echo "		\"directory\": \"$(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))\"," >> $@; \
		echo "	}," >> $@; \
	done \
	;
	echo "]" >> $@

