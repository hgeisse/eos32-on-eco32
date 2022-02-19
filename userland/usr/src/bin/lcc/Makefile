#
# Makefile for 'lcc' command
#

LCCSRCS = etc/lcc.c etc/eco32-eos32.c
CPPSRCS = cpp/cpp.c cpp/eval.c cpp/getopt.c cpp/hideset.c cpp/include.c \
          cpp/lex.c cpp/macro.c cpp/nlist.c cpp/tokens.c cpp/unix.c
RCCSRCS = src/main.c src/alloc.c src/bind.c src/dag.c src/decl.c \
          src/enode.c src/error.c src/expr.c src/event.c src/init.c \
          src/inits.c src/input.c src/lex.c src/list.c src/output.c \
          src/prof.c src/profio.c src/simp.c src/stab.c src/stmt.c \
          src/string.c src/sym.c src/trace.c src/tree.c src/types.c \
          src/null.c src/symbolic.c src/gen.c src/bytecode.c
BACKENDS = build/eco32.c build/dagcheck.c
LBURGSRCS = lburg/lburg.c lburg/gram.c

all:		mkbuild build/lcc build/cpp build/rcc build/lburg

install:	mkbuild build/lcc build/cpp build/rcc build/lburg
		cp build/lcc /bin
		cp build/cpp /bin
		cp build/rcc /bin
		cp build/lburg /bin

mkbuild:
		-mkdir ./build 
#		^ ignore errors

build/lcc:	$(LCCSRCS)
		lcc -A -DLCCDIR=\"/bin/\" \
		  -o build/lcc $(LCCSRCS)

build/cpp:	$(CPPSRCS)
		lcc -A -o build/cpp $(CPPSRCS)

build/rcc:	$(RCCSRCS) $(BACKENDS)
		lcc -A -I./src -o build/rcc \
		  $(RCCSRCS) $(BACKENDS)

build/alpha.c:	src/alpha.md
		lburg src/alpha.md build/alpha.c

build/dagcheck.c:	src/dagcheck.md
		lburg src/dagcheck.md build/dagcheck.c

build/eco32.c:	src/eco32.md
		lburg src/eco32.md build/eco32.c

build/mips.c:	src/mips.md
		lburg src/mips.md build/mips.c

build/sparc.c:	src/sparc.md
		lburg src/sparc.md build/sparc.c

build/x86linux.c:	src/x86linux.md
		lburg src/x86linux.md build/x86linux.c

build/x86.c:	src/x86.md
		lburg src/x86.md build/x86.c

lburg/gram.c:	lburg/gram.y
		yacc lburg/gram.y
		cp y.tab.c lburg/gram.c
		rm y.tab.c

build/lburg:	$(LBURGSRCS)
		lcc -A -o build/lburg $(LBURGSRCS)

clean:
		rm -f *~
		rm -f ./lburg/gram.c
		rm -rf ./build
