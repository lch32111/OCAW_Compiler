https://github.com/DoctorWkt/acwj/tree/master/04_Assembly



# Part 4 : An Actual Compiler

실제로 컴파일러를 작성하는 나의 약속을 충족시킬 시간이다. 그래서 이 여정의 이 파트에서, 우리는 우리의 프로그램에 있는 interpreter를 x86-64 assembly code를 생성하는 코드로 대체한다.



## Revising the Interpreter

우리가 하기전에, `interp.c`에 있는 interpreter code를 재방문하는 것이 가치있을 것이다:

```c
int interpretAST(struct ASTnode *n) {
  int leftval, rightval;

  if (n->left) leftval = interpretAST(n->left);
  if (n->right) rightval = interpretAST(n->right);

  switch (n->op) {
    case A_ADD:      return (leftval + rightval);
    case A_SUBTRACT: return (leftval - rightval);
    case A_MULTIPLY: return (leftval * rightval);
    case A_DIVIDE:   return (leftval / rightval);
    case A_INTLIT:   return (n->intvalue);

    default:
      fprintf(stderr, "Unknown AST operator %d\n", n->op);
      exit(1);
  }
}
```

그 `interpretAST()` 함수는 주어진 AST tree를 depth-first로 방문한다. 그것은 어떤 left sub-tree를 평하하고, 그러고나서 right sub-tree를 평가한다. 마지막으로, 그것은 이러한 자식들에 작동하는 현재 트리의 base에 있는 `op` value를 사용한다.

만약 그 `op` value가 4개의 수학 연산자들 중의 하나이면, 그러면 이 수학 연산이 수행된다. 만약 그 `op` 값이 그 노드가 간단히 integer literal이라는 것을 가리킨다면, 그 literal value가 반환된다.

그 함수는 이 트리에 대한 최종값을 반환한다. 그리고, 그것의 재귀적이기 떄문에, 한 번에 한 sub-tree에 대한 전체트리의 최종 값을 계산할 것이다.



## Changing to Assembly Code Generation

우리는 generic한 assembly code generator를 사용할 것이다. 이것은, 차례대로 CPU에 특정한 코드 generatio functions의 한 set에 대해 호출할 것이다.

여기에 `gen.c`에서 generic assembly code generator가 있다:

```c
// Given an AST, generate
// assembly code recursively
static int genAST(struct ASTnode *n) {
  int leftreg, rightreg;

  // Get the left and right sub-tree values
  if (n->left) leftreg = genAST(n->left);
  if (n->right) rightreg = genAST(n->right);

  switch (n->op) {
    case A_ADD:      return (cgadd(leftreg,rightreg));
    case A_SUBTRACT: return (cgsub(leftreg,rightreg));
    case A_MULTIPLY: return (cgmul(leftreg,rightreg));
    case A_DIVIDE:   return (cgdiv(leftreg,rightreg));
    case A_INTLIT:   return (cgload(n->intvalue));

    default:
      fprintf(stderr, "Unknown AST operator %d\n", n->op);
      exit(1);
  }
}
```

친숙해 보인다. 우리는 같은 depth-first tree traversal를 하고있다. 이번에:

* A_INTLIT : literal value로 register를 load한다.
* 다른 연산자들 : left-child와 right-child의 value를 보유하는 두 개의 registers에 대해 수학 함수를 수행한다

값을 넘기는 대신에, `genAST()`에 있는 코드는 register identifiers를 보낸다. 예를들어 `cgload()`는 한 value를 register에 load하고, loaded value의 register의 identity를 반환한다.

`genAST()`는 그 자체로 이 시점에서 tree의 최종 값을 보유한 register의 identity를 보유한다. 이것이 그 top에서 코드가 register identities를 얻는 이유이다:

```c
  if (n->left) leftreg = genAST(n->left);
  if (n->right) rightreg = genAST(n->right);
```



## Calling `genAST()`

`genAST()`는 그것에 주어진 expression의 값을 계산만 할 것이다. 우리는 이 최종 계산을 출력할 필요가 있다. 우리는 또한 우리가 생성한 assembly code를 어떤 leading code (그 *preamble*)와 어떤 trailing code (그 *postable*)로 감쌀 필요가 있을 것이다. 이것은 `gen.c`에 있는 다른 함수에서 처리된다:

```c
void generatecode(struct ASTnode *n) {
  int reg;

  cgpreamble();
  reg= genAST(n);
  cgprintint(reg);      // Print the register with the result as an int
  cgpostamble();
}
```



## The x86-64 Code Generator

이것은 길에서 벗어난 generic code generator이다. 우리는 어떤 실제 assembly code의 생성을 볼필요가 있다. 지금은, 나는 x86-64 CPU를 타겟으로 하고 있는데, 이것이 여전히 가장 흔한 Linux platforms들 중의 하나이기 때문이다. 그래서 `cg.c`를 열어서 탐색해보자.

### Allocating REgisters

어떤 CPU는 제한된 개수의 registers를 가지고 있다. 우리는 integer literal values를 보유할 레지스터와 추가적으로 그것들에 수행해야할 어떤 계산을 위한 레지스터를 할당해야 할 것이다. 그러나, 우리가 한 값을 사용했다면, 우리는 그 값을 버릴 수 있고, 그러므로 그것을 보유한 레지스터를 해제할 수 있다. 그러고나서, 우리는 또 다른 값을 위해 그 register를 재사용할 수 있다.

레지스터 할당을 다루는 세 개의 함수들이 있다:

* `freeall_registers()` : 모든 registers가 이용가능하도록 설정
* `alloc_register()` : 한 free register를 할당
* `free_register()` : 할당된 레지스터를 해제

나는 코드를 설명하지 않을 것인데, 그것이 간단하고 에러 체크하는 게 있기 때문이다. 지금 당장, 만약 내가 registers가 부족하다면, 그 프로그램을 crash날 것이다. 나중에, 나는 우리가 free registers가 부족할 때의 상황을 다룰 것이다.

그 코드는 generic registers에 대해 작동한다 : r0, r1, r2 그리고 r3. 실제 register names을 가진 strings의 table이 있다:

```c
static char* reglist[4] = { "%r8", "%r9", "%r10", "%r11" };
```

이것은 이러한 함수들을 CPU architecture와 꽤 독립적으로 만든다.



### Loading a Register

이것은 `cgload()`에서 처리된다 : 한 register가 할당되고, 그러고나서 `movq` instruction은 literal value를 할당된 register에 load한다.

```c
// Load an integer literal value into a register.
// Return the number of the register
int cgload(int value) {

  // Get a new register
  int r= alloc_register();

  // Print out the code to initialise it
  fprintf(Outfile, "\tmovq\t$%d, %s\n", value, reglist[r]);
  return(r);
}
```



### Adding Two Registers

`cgadd()`는 두 개의 register numbers를 취하고, 그것들을 함께 더할 코드를 생성한다. 그 결과는 두 개의 registers들 중 하나에 저아되고, 나머지 하나는 나중을 위해 해제된다:

```c
// Add two registers together and return
// the number of the register with the result
int cgadd(int r1, int r2) {
  fprintf(Outfile, "\taddq\t%s, %s\n", reglist[r1], reglist[r2]);
  free_register(r1);
  return(r2);
}
```

그 덧셈이 *commutative*하다는 것에 주의해라. 그래서 나는 `r2`에 `r1` 를 더하는 대신에 `r1`에 `r2`를 더할 수 있다. 최종 값을 가진 register의 identity가 반환된다.



### Multiplying Two Registers

이것은 덧셈과 비슷하지만, 다시, 그 연산은 *commutative*이고, 그래서 어떤 register이든지 반환될 수 있다:

```c
// Multiply two registers together and return
// the number of the register with the result
int cgmul(int r1, int r2) {
  fprintf(Outfile, "\timulq\t%s, %s\n", reglist[r1], reglist[r2]);
  free_register(r1);
  return(r2);
}
```



### Subtracting Two Registers

뺄셈은 commutative하지 않다: 우리는 순서를 올바르게 해야한다. 그 두 번째 register는 첫 번째 것으로부터 뺄셈 되고, 우리는 그 첫 번째 것을 반환하고 두 번째 것을 해제한다:

```c
// Subtract the second register from the first and
// return the number of the register with the result
int cgsub(int r1, int r2) {
  fprintf(Outfile, "\tsubq\t%s, %s\n", reglist[r2], reglist[r1]);
  free_register(r2);
  return(r1);
}
```



### Dividing Two Registers

나눗셈 또한 commutative하지 않다. 그래서 이전의 notes가 적용된다. x86-64에서, 이것은 심지어 휠씬 더 복잡하다. 우리는 `r1`으로부터 *dividend*를 가진 `%rax`를 불러와야 할 필요가 있다. 이것은 `cqo`로 8  bytes로 확장되어질 필요가 있다. 그러고나서, `idivq`는 `r2`에 있는 divisor로 `%rax` 를 나눌 것이다. 그리고 그 몫인 *quotient*는 `%rax`에 남겨진다. 그래서 우리는 그것을 `r1` 또는 `r2`에 복사할 필요가 있다. 그러고나서 우리는 나머지 레지스터를 해제할 수 있다.

```c
// Divide the first register by the second and
// return the number of the register with the result
int cgdiv(int r1, int r2) {
  fprintf(Outfile, "\tmovq\t%s,%%rax\n", reglist[r1]);
  fprintf(Outfile, "\tcqo\n");
  fprintf(Outfile, "\tidivq\t%s\n", reglist[r2]);
  fprintf(Outfile, "\tmovq\t%%rax,%s\n", reglist[r1]);
  free_register(r2);
  return(r1);
}
```





### Printing A Register

한 register를 decimal number로 출력하는 x86-64 instruction이 있지 않다. 이 문제를 해결하기 위해서, 그 assembly preamble은 `printint()`라고 불려지는 함수를 포함한다. 이것은 register argument를 취하고, 이것을 decimal로 출력할 `printf()`를 호출한다.

나는 그 코드를 `cgpreamble()`에 넣었을 뿐만 아니라, 그것은 `main()`을 위한 beginning code를 포함한다. 이것은 우리가 완료된 프로그램을 얻기 위해 우리의 output file을 assemble할 수 있게 위해서이다. `cgpostamble()`를 위한 코드는 여기에서 주어지지 않지만, 그 프로그램을 종료시키기 위해 간단히 `exit(0)`를 호출시킨다.

그러나, 여기에 `cgprintint()`가 있다:

```c
void cgprintint(int r) {
  fprintf(Outfile, "\tmovq\t%s, %%rdi\n", reglist[r]);
  fprintf(Outfile, "\tcall\tprintint\n");
  free_register(r);
}
```

Linux x86-64는 한 함수에 대한 첫 번째 인자가 `%rdi` register에 있기를 기대하고, 그래서 우리는 `call printint`를 하기 전에 우리의 register를 `%rdi`로 옮긴다.



## Doing Our First Compile

이것이 x86-64 code generator를 위한 것이다.  우리의 output file으로서 `out.s`를 open시키는 `main()`에서의 추가 코드가 있따. 나는 또한 그 프로그램의 interpreter를 남겨두었는데, 우리는 우리의 assembly가 input expression에 대해 interpreter와 같은 답을 꼐산하는지를 확인할 수 있게 하기 위해서이다.

compiler를 만들고 그것을 `input01`에 대해 동작시키자:

```shell
$ make
cc -o comp1 -g cg.c expr.c gen.c interp.c main.c scan.c tree.c

$ make test
./comp1 input01
15
cc -o out out.s
./out
15
```

Yes! 첫 번째 15가 interpreter의 output이고, 그 두 번째가 assembly의 output이다.



## Examining the Assembly Output

그래서, 그 assembly output이 정확히 무엇인가? 여기에 그 input file이 있다:

```
2 + 3 * 5 - 8 / 3
```

그리고 여기에 주석이 있는 이 input에 대한 `out.s`이다:

```assembly
        .text                           # Preamble code
.LC0:
        .string "%d\n"                  # "%d\n" for printf()
printint:
        pushq   %rbp
        movq    %rsp, %rbp              # Set the frame pointer
        subq    $16, %rsp
        movl    %edi, -4(%rbp)
        movl    -4(%rbp), %eax          # Get the printint() argument
        movl    %eax, %esi
        leaq    .LC0(%rip), %rdi        # Get the pointer to "%d\n"
        movl    $0, %eax
        call    printf@PLT              # Call printf()
        nop
        leave                           # and return
        ret

        .globl  main
        .type   main, @function
main:
        pushq   %rbp
        movq    %rsp, %rbp              # Set the frame pointer
                                        # End of preamble code

        movq    $2, %r8                 # %r8 = 2
        movq    $3, %r9                 # %r9 = 3
        movq    $5, %r10                # %r10 = 5
        imulq   %r9, %r10               # %r10 = 3 * 5 = 15
        addq    %r8, %r10               # %r10 = 2 + 15 = 17
                                        # %r8 and %r9 are now free again
        movq    $8, %r8                 # %r8 = 8
        movq    $3, %r9                 # %r9 = 3
        movq    %r8,%rax
        cqo                             # Load dividend %rax with 8
        idivq   %r9                     # Divide by 3
        movq    %rax,%r8                # Store quotient in %r8, i.e. 2
        subq    %r8, %r10               # %r10 = 17 - 2 = 15
        movq    %r10, %rdi              # Copy 15 into %rdi in preparation
        call    printint                # to call printint()

        movl    $0, %eax                # Postamble: call exit(0)
        popq    %rbp
        ret
```

Excellent! 우리는 이제 적당한 compiler를 가지게 되었다: 한 언어로 된 input를 취하고 그 input을 다른 언어로의 변환를 생성하는 프로그램.

우리는 여전히 그 output을 machine code로 assemble해야하고, 그것을 support libraries와 함께 링크해야 하지만, 이것은 우리가 이제 수동으로 할 수 있는 것이다. 나중에, 우리는 이것을 자동으로 하는 코드를 작성할 것이다.



## Conclusion and What's Next

interpreter에서 generic code generator로 바꾸는 것은 사소하지만, 우리는 그러고나서 실제 assembly output을 생성하기 위해 코드를 작성해야만 한다. 이것을 하기 위해, 우리는 registers를 어떻게 할당하는 지에 대해 생각해야만 한다 : 지금, 우리는 간단한 솔루션을 가지고 있다. 우리는 또한 `idivq` instruction같은 x86-64 의 특이함을 다루어야만 했다.

내가 아직 다륒 않은 것은 : 왜 한 expression에 대해 AST를 생성하는데 시달리는가? 확실히, 우리가 우리의 Pratt parser에서 '+' token을 만날 때, `cgadd()`를 호출할 수 있었고, 다른 연산자들에 대해서도 마찬가지 이다. 나는 이것에 대해 너가 생각하도록 남겨둘 것이지만, 나는 한 두 단계 뒤에 그것에 대해 돌아와서 말할 것이다.

우리의 compiler writing journey의 다음 파트에서, 우리는 우리의 언어에 대한 몇 가지 statements를 추가할 것이다. 이것은 적절한 computer language를 닮게 만들기 시작하기 위해서이다.