# Part11: Functions, part 1

나는 우리의 언어에 함수를 구현하는 작업을 시작하고 싶지만, 나는 이것이 매우 많은단계들을 포함할 것이라는 것을 안다. 우리가 그 방법을 따라 다룰 몇 가지 것들은:

* data의 Types : `char`, `int`, `long`  등.
* 각 함수 retunn type.
* 각 함수에 대한 arguments의 개수.
* 한 함수에 대해 local인 변수와, global variables.

우리의 journey의 이 부분에서 처리되기에 너무 많다. 그래서 내가 여기에서 하려는 것은 우리가 다른 함수들을 *선언*할 수 있는 지점에 도달하는 것이다. 우리의 최종 executable에서 `main()` function만 작동할 것이지만, 우리는 여러 개의 함수를 위한 코드를 생성할 능력을 가질 것이다.

바라건데 곳,  우리의 컴파일러가 인지할 언어는 우리의 입력이 실제 C compiler에 의해 인지될 수 있을 C의 충분한 subset이될 것이다. 그러나, 아직은 아니다.



## The Simplistic Functions Syntax

이것은 명백히 한 placeholder가 될 것인데, 우리가 함수처럼 보이는것을 parse할 수있게 하기 위해서 이다. 이것이 된다면, 우리는 그러한 다른 중요한 것들을 추가할 수 있다: types, return types, arguments 등.

그래서, 지금, 나는 BNF로 이것처럼 보이는 함수 문법을 추가할 것이다:

```
function_declaration: 'void' identifier '(' ')' compound_statement ;
```

모든 함수들은 `void`로 선언될 것이고, 어떠한 인자들도 가지지 않을 것이다. 우리는 또한 한 함수를 호출할 능력을 도입하지 않을 것이다. 그래서 오직 `main()` function만 실행될 것이다.

우리는 새로운 키워드`void`와 새로운 token T_VOID가 필요하다. 둘 다 추가하기 쉽다.



## Parsing the Simplistic Function Syntax

그 새로운 함수 syntax는 매우간단해서, 우리는 그것을 파싱하기에 좋고 작은 함수를 작성할 수 있다 (`decl.c`에):

```c
// Parse the declaration of a simplistic function
struct ASTnode *function_declaration(void) {
  struct ASTnode *tree;
  int nameslot;

  // Find the 'void', the identifier, and the '(' ')'.
  // For now, do nothing with them
  match(T_VOID, "void");
  ident();
  nameslot= addglob(Text);
  lparen();
  rparen();

  // Get the AST tree for the compound statement
  tree= compound_statement();

  // Return an A_FUNCTION node which has the function's nameslot
  // and the compound statement sub-tree
  return(mkastunary(A_FUNCTION, tree, nameslot));
}
```

이것은 syntax checking과 AST building을할 것이지만, 여기에서 어떠한 semantic error를 체크하는 것이 없다. 한 함수가 다시 선언되면 어떻게 될까? 우리는 아직 그것을 신경쓰지 않을 것이다.



## Modifications to `main()`

 위의 함수로, 우리는 하나씩여러개의 함수들을 파싱하기 위해서 `main()`에서 어떤 코드들을 다시 작성할 수 있다:

```c
scan(&Token);                 // Get the first token from the input
genpreamble();                // Output the preamble
while (1) {                   // Parse a function and
    tree = function_declaration();
    genAST(tree, NOREG, 0);     // generate the assembly code for it
    if (Token.token == T_EOF)   // Stop when we have reached EOF
        break;
}
```

내가 `genpostamble()` 함수를 제거했다는 것을 유의해라. 그것은 왜냐하면 그것의 output이 기술적으로 `main()`에대한 생성된 assembly에 대한 postamble이기 때문이다. 우리는 이제 한 함수의 시작과 한 함수의 끝을 생성하기 위한 어떤 코드 생성 함수가 필요하다.



## Generic Code Generation for Functions

우리는 A_FUNCTION AST node를 가졌으니, 우리는 generic code generator에 그것을다루기 위해 어떤 코드를 추가하는 게 좋다. 위를 보면, 이것은 단일 child를 가진 unary AST node를 만든다:

```c
  // Return an A_FUNCTION node which has the function's nameslot
  // and the compound statement sub-tree
  return(mkastunary(A_FUNCTION, tree, nameslot));
```

그 child는 function의 body인 compound state를 가진 sub-tree를 가진다. 우리는 compound statement를 위한 코드를 생성하기 *전에* 함수의 시작을 생성할 필요가 있다. 그래서, 여기에 이것을 하기 위한 `genAST()`에서의 코드가 있다:

```c
    case A_FUNCTION:
      // Generate the function's preamble before the code
      cgfuncpreamble(Gsym[n->v.id].name);
      genAST(n->left, NOREG, n->op);
      cgfuncpostamble();
      return (NOREG);
```

## x86-64 Code Generation

이제 우리는 각함수에 대해 stack과 frame pointer를 설정할 코드를 생성해야하고, 또한 그 함수의 끝에서 이것을 되돌리고, 그 함수의 호출자에게 반환해야 하는 하는 지점에 있다.

우리는 이미 `cgpreamble()`과 `cgpostamble()`에 이 코드를 가지고 있지만, `cgpreamble()`은 또한 `printint()` 함수를 위한 assembly code이다. 그러므로, assembly code의 이러한 snippets을 `cg.c`에서 새로운 함수들로 분리하는 것이 중요하다:

```c
// Print out the assembly preamble
void cgpreamble() {
  freeall_registers();
  // Only prints out the code for printint()
}

// Print out a function preamble
void cgfuncpreamble(char *name) {
  fprintf(Outfile,
          "\t.text\n"
          "\t.globl\t%s\n"
          "\t.type\t%s, @function\n"
          "%s:\n" "\tpushq\t%%rbp\n"
          "\tmovq\t%%rsp, %%rbp\n", name, name, name);
}

// Print out a function postamble
void cgfuncpostamble() {
  fputs("\tmovl $0, %eax\n" "\tpopq     %rbp\n" "\tret\n", Outfile);
}
```



## Testing The Function Generation Functionality

우리는 새로운 테스트 프로그램을 가진다, `test/input08`. 이것은 C rogram처럼 보이기 시작한다 (`print` statement를 제외하고)

```c
void main()
{
  int i;
  for (i= 1; i <= 10; i= i + 1) {
    print i;
  }
}
```

이것을 테스트 하기 위해, `make test8`를 해라, 이것은 다음을 한다

```shell
cc -o comp1 -g cg.c decl.c expr.c gen.c main.c misc.c scan.c
    stmt.c sym.c tree.c
./comp1 tests/input08
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

나는 assembly otuput을 보지 않을 것이다, 왜냐하면 그것은 지난 파트에서 FOR loop test를 위해 생성된 코드와 동일하기 때문이다.

그러나, 나는 모든 이전 테스트 입력 파일들에 `void main()`를 추가했는데, 이것은 그 언어가 compound statement code전에 function declaration을 요구하기 때문이다.

그 테스트 프로그램 `tests/input09`는 그것 안에 두 개이 함수들이 선언되어 있다. 그 컴파일러는 행우리의 복하게 각 함수에 대해 작동하는 assembly code를 생성하지만, 현재, 우리는 그 두 번째 함수를 작동시킬 수 없다.



## Conclusion and What's Next

우리는 우리의 언어에 함수를 추가하는 좋은 시작을 했다. 이제, 그것은 매우간단한 함수 선언일 뿐이다.

우리의 compiler writing journey의 다음 부분에서, 우리는 우리의 컴파일러에 types들을 추가시키는 프로세스를 시작할 것이다.







