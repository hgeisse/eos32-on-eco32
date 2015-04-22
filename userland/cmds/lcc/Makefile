#
# Makefile for 'lcc' command
#

BUILD = ../../../../build

LCCSRCS = etc/lcc.c etc/eco32-eos32.c
CPPSRCS = cpp/cpp.c cpp/eval.c cpp/getopt.c cpp/hideset.c cpp/include.c \
          cpp/lex.c cpp/macro.c cpp/nlist.c cpp/tokens.c cpp/unix.c
RCCSRCS = src/main.c src/alloc.c src/bind.c src/dag.c src/decl.c \
          src/enode.c src/error.c src/expr.c src/event.c src/init.c \
          src/inits.c src/input.c src/lex.c src/list.c src/output.c \
          src/prof.c src/profio.c src/simp.c src/stab.c src/stmt.c \
          src/string.c src/sym.c src/trace.c src/tree.c src/types.c \
          src/null.c src/symbolic.c src/gen.c src/bytecode.c
BACKENDS = build/alpha.c build/dagcheck.c build/eco32.c build/mips.c \
           build/sparc.c build/x86linux.c build/x86.c
LBURGSRCS = lburg/lburg.c lburg/gram.c

all:		mkbuild build/lcc build/cpp build/rcc build/lburg

install:	mkbuild build/lcc build/cpp build/rcc build/lburg
		mkdir -p $(BUILD)/cmds
		cp build/lcc $(BUILD)/cmds
		cp build/cpp $(BUILD)/cmds
		cp build/rcc $(BUILD)/cmds
		cp build/lburg $(BUILD)/cmds

mkbuild:
		mkdir -p ./build

build/lcc:	$(LCCSRCS)
		$(BUILD)/bin/lcc -A -DLCCDIR=\"/bin/\" \
		  -o build/lcc $(LCCSRCS)

build/cpp:	$(CPPSRCS)
		$(BUILD)/bin/lcc -A -o build/cpp $(CPPSRCS)

build/rcc:	$(RCCSRCS) $(BACKENDS)
		$(BUILD)/bin/lcc -A -I./src -o build/rcc \
		  $(RCCSRCS) $(BACKENDS)

build/alpha.c:	src/alpha.md
		$(BUILD)/bin/lburg src/alpha.md build/alpha.c

build/dagcheck.c:	src/dagcheck.md
		$(BUILD)/bin/lburg src/dagcheck.md build/dagcheck.c

build/eco32.c:	src/eco32.md
		$(BUILD)/bin/lburg src/eco32.md build/eco32.c

build/mips.c:	src/mips.md
		$(BUILD)/bin/lburg src/mips.md build/mips.c

build/sparc.c:	src/sparc.md
		$(BUILD)/bin/lburg src/sparc.md build/sparc.c

build/x86linux.c:	src/x86linux.md
		$(BUILD)/bin/lburg src/x86linux.md build/x86linux.c

build/x86.c:	src/x86.md
		$(BUILD)/bin/lburg src/x86.md build/x86.c

build/lburg:	$(LBURGSRCS)
		$(BUILD)/bin/lcc -A -o build/lburg $(LBURGSRCS)

clean:
		rm -f *~
		rm -rf ./build
