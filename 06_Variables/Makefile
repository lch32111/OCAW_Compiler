SRCS= cg.c decl.c expr.c gen.c main.c misc.c scan.c stmt.c sym.c tree.c
comp1: $(SRCS)
	gcc -o comp1 -g $(SRCS)

clean:
	rm -f comp1 *.0 *.s out

test : comp1 input01 input02
	./comp1 input01
	gcc -o out out.s
	./out
	./comp1 input02
	gcc -o out out.s
	./out
