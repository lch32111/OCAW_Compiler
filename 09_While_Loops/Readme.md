https://github.com/DoctorWkt/acwj/tree/master/09_While_Loops



# Part 9 : While Loops

여행의 이번 파트에서, 우리는 우리의 언어에 WHILE loops를 추가할 것이다. 어떤 의미에서, WHILE loop는 else clause가 없는 IF 문과 많이 비슷하다. 우리가 그 loop의 top으로 다시 돌아간다는 점을 제외하고.

그래서, 이것은 :

```
	while (condition is true) {
		statements;
	}
```

다음으로 바뀌어야 한다:

```
Lstart:	evalute condition
		jump to Lend if condition false
		statements
		jump to Lstart
Lend:
```

이것은 IF문과 사용했었던 우리가 scanning, parsing 그리고 코드 생성 구조를 빌릴 수 있다는 것과 WHILE문을 다루기 위해 작은 변화만을 만들 수 있다는 것을 의미한다.

우리가 이것이 어떻게 되게 하는지 봐보자.



## New Tokens

우리는 T_WHILE이라는 새로운 토큰이 필요한데, 새로운 'while' keyword를 위해서이다. `defs.h`과 `scan.c`에 대한 변화들은 명백해서 나는 여기에서 그것들을 생략할 것이다.



## Parsing the While Syntax

WHILE loop를 위한 BNF grammar는 다음이다:

```
 // while_statement: 'while' '(' true_false_expression ')' compound_statement ;
```

그리고 우리는 이것을 파싱하기 위해 `stmt.c`에서 함수가 필요하다. 여기에 그것이 있다; IF 문의 파싱과 비교하여 이것의 간단함에 주목해라:

```c
// Parse a WHILE statement
// and return its AST
struct ASTnode *while_statement(void) {
  struct ASTnode *condAST, *bodyAST;

  // Ensure we have 'while' '('
  match(T_WHILE, "while");
  lparen();

  // Parse the following expression
  // and the ')' following. Ensure
  // the tree's operation is a comparison.
  condAST = binexpr(0);
  if (condAST->op < A_EQ || condAST->op > A_GE)
    fatal("Bad comparison operator");
  rparen();

  // Get the AST for the compound statement
  bodyAST = compound_statement();

  // Build and return the AST for this statement
  return (mkastnode(A_WHILE, condAST, NULL, bodyAST, 0));
}
```

우리는 새로운 AST node type이 필요하다.  A_WHILE. 이것은 `defs.h`에 추가되었다. 이 노드는 그 condition을 evaluate할 left child sub-tree를 가지고, WHILE loop의 body인 compound statement를 위한 right child sub-tree를 가진다.



## Generic Code Generation

우리는 start and end label를 만들고, 그 조건문을 평가하고, 그 loop에서 나가고 그 loop의 top으로 돌아가기 위해 적절한 jumps를 넣을 필요가 있다. 다시, 이것은 IF문을 생성하는 코드보다 훨씬 더 간단하다. `gen.c`에서 :

```c
// Generate the code for a WHILE statement
// and an optional ELSE clause
static int genWHILE(struct ASTnode *n) {
  int Lstart, Lend;

  // Generate the start and end labels
  // and output the start label
  Lstart = label();
  Lend = label();
  cglabel(Lstart);

  // Generate the condition code followed
  // by a jump to the end label.
  // We cheat by sending the Lfalse label as a register.
  genAST(n->left, Lend, n->op);
  genfreeregs();

  // Generate the compound statement for the body
  genAST(n->right, NOREG, n->op);
  genfreeregs();

  // Finally output the jump back to the condition,
  // and the end label
  cgjump(Lstart);
  cglabel(Lend);
  return (NOREG);
}
```

내가 해야만 했었던 한 가지 것은, 비교 연산자들의 부모 AST node가 A_WHILE일 수 있따는 것을 인지하는 것이다. 그래서 `genAST()`에서, 그 비교 연산자들을 위한 코드는 다음과 같이 보인다:

```c
    case A_EQ:
    case A_NE:
    case A_LT:
    case A_GT:
    case A_LE:
    case A_GE:
      // If the parent AST node is an A_IF or A_WHILE, generate 
      // a compare followed by a jump. Otherwise, compare registers 
      // and set one to 1 or 0 based on the comparison.
      if (parentASTop == A_IF || parentASTop == A_WHILE)
        return (cgcompare_and_jump(n->op, leftreg, rightreg, reg));
      else
        return (cgcompare_and_set(n->op, leftreg, rightreg));
```

그리고, 전적으로 우리는 이것이 우리가 WHILE loops를 구현하는데 필요한 모든 것이다!

## Testing the New Language Additions

나는 모든 input files을 `test/` 폴더로 옮겼다. 만약 너가 `make test`를 한다면, 그것은 이 폴더로 갈 것이고, 각 input를 컴파일하고 그 output을 옳다고 알려진 output과 비교할 것이다:

```makefile
cc -o comp1 -g cg.c decl.c expr.c gen.c main.c misc.c scan.c stmt.c
      sym.c tree.c
(cd tests; chmod +x runtests; ./runtests)
input01: OK
input02: OK
input03: OK
input04: OK
input05: OK
input06: OK
```

너는 또한 `make test6`을 할 수 있다. 이것은 `tests/input06` 파일을 컴파일한다

```c
{ int i;
  i=1;
  while (i <= 10) {
    print i;
    i= i + 1;
  }
}
```



이것은 그 숫자를 1부터 10까지 출력할 것이다:

```shell
cc -o comp1 -g cg.c decl.c expr.c gen.c main.c misc.c scan.c
      stmt.c sym.c tree.c
./comp1 tests/input06
cc -o out out.s
./out
1
2
3
4
5
6
7
8
9
10
```

그리고 컴파일된 어셈블리어 결과이다

```assembly
	.comm	i,8,8
	movq	$1, %r8
	movq	%r8, i(%rip)		# i= 1
L1:
	movq	i(%rip), %r8
	movq	$10, %r9
	cmpq	%r9, %r8		# Is i <= 10?
	jg	L2			# Greater than, jump to L2
	movq	i(%rip), %r8
	movq	%r8, %rdi		# Print out i
	call	printint
	movq	i(%rip), %r8
	movq	$1, %r9
	addq	%r8, %r9		# Add 1 to i
	movq	%r9, i(%rip)
	jmp	L1			# and loop back
L2:
```



## Conclusion and What's Next

그 WHILE loop는 추가하기에 쉬웠다. 우리가 IF문을 하기만 했다면. 그것들이 많은 유사성을 공유하기 때문이다.

나는 우리가 또한 이제 [Turing-complete](https://en.wikipedia.org/wiki/Turing_completeness) 언어를 가진다고 생각한다:

* storage의 무한한 양. 즉 무한한 개수의 변수들
* 저장된 값들을 기반으로 의사결정하는 능력. 즉 IF 문
* directions을 바꾸는 능력, 즉 WHILE loops

그래서 우리는 이제 멈출 수 있고, 우리의 일은 끝났다! 아니. 물론 아니다. 우리는 여전히 컴파일러가 그 자체를 컴파일하도록 작업할 것이다.

우리의 compiler writing journey의 다음 파트에서, 우리는 그 언어에 FOR loops를 추가할 것이다.