comp1: cg.c expr.c gen.c interp.c main.c scan.c tree.c
	gcc -o comp1 -g cg.c expr.c gen.c interp.c main.c scan.c tree.c

clean:
	rm -f comp1 *.0 *.s out

test: comp1
	./comp1 input01
	gcc -o out out.s
	./out
	./comp1 input02
	gcc -o out out.s
	./out
