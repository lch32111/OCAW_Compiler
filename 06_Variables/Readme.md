https://github.com/DoctorWkt/acwj/tree/master/06_Variables

# Part 6 : Variables

나는 컴파일러에 global variables를 추가하는 것을 끝내고, 내가 의심했듯이, 그것은 많은 작업이었다. 또한, 컴파일러에서 꽤 많은 모든 파일들이 프로세스에서 수정되었다. 그래서 journey의 이 파트는 길 것이다.



## What Do We Want from Variables?

우리는 다음의 것들을 할 수 있길 원한다:

* Declare variables
* 저장된 values를 얻기위해 variables를 사용
* variables에 할당

여기에 우리의 test program이 될 `input02`가 있다:

```c
int fred;
int jim;
fred= 5;
jim= 12;
print fred + jim;
```

가장 명백한 볂화는, 그 문법이 이제 변수 선언, 할당 표현, 그리고 expressions에서 변수 이름을 가진다는 것이다. 그러나 우리가 그것에 들어가기 전에, 우리가 변수들을 어떠게 구현하는지 보자.



## The Symbol Table

모든 컴파일러는 [symbol table](https://en.wikipedia.org/wiki/Symbol_table)을 필요로 한다. 나중에, 우리는 한 개 이상의 global variables를 보유할 것이다. 그러나 지금은, 그 table에 한 entry의 구조가 있따 (`defs.h`)에서:

```c
// Symbol table structure
struct symtable {
  char *name;                   // Name of a symbol
};
```

우리는 `data.h`에 symbols의 한 array를 가진다:

```c
#define NSYMBOLS        1024            // Number of symbol table entries
extern_ struct symtable Gsym[NSYMBOLS]; // Global symbol table
static int Globs = 0;                   // Position of next free global symbol slot
```

`Globs`는 실제로 `sym.c`에 있는데, 이것은 symbol table를 관리하는 파일이다. 여기에서, 우리는 이러한 관리 함수들을 가진다:

* `int findglob(char* s)` : 그 symbol `s`가 global symbol table에 있는지를 결정한다. 그것의 slot position을 반환하거나, 발견되지 않는다면 -1를 반환한다.
* `static int newglob(void)` : 새로운 global symbol slot의 position을 얻거나, 만약 positions이 부족하다면 die한다.
* `int addglob(char* name)` : symbol table에 global symbol를 추가한다. symbol table에 있는 slot number를 반환한다.



## Scanning and New Tokens

만약 너가 example input file를 본다면, 우리는 몇 가지 새로운 토큰들이 필요하다:

* T_INT로 알려진 'int'
* T_EQUALS로 알려진 '='
* T_IDENT로 알려진 identifier names.

`=`의 scanning은 `scan()`에 추가하기 쉽다:

```c
  case '=':
    t->token = T_EQUALS; break;
```

우리는 `keyword()`에 'int' keyword를 추가할 수 있다:

```c
  case 'i':
    if (!strcmp(s, "int"))
      return (T_INT);
    break;
```

identifers에 대해, 우리는 `Text` 변수에 words를 저장하기 위해 이미 `scanident()`를 사용하고 있다. 한 word가 keyword가 아니라면 dying하는 것 대신에, 우리는 T_IDENT token를 반환할 수 있다:

```c
   if (isalpha(c) || '_' == c) {
      // Read in a keyword or identifier
      scanident(c, Text, TEXTLEN);

      // If it's a recognised keyword, return that token
      if (tokentype = keyword(Text)) {
        t->token = tokentype;
        break;
      }
      // Not a recognised keyword, so it must be an identifier
      t->token = T_IDENT;
      break;
    }
```



## The New Grammar

우리는 우리의 input 언어의 문법에 대한 변화를 볼 준비를 한다. 이전 처럼, 나는 그것을 BNF notation으로 정의할 것이다:

```BNF
 statements: statement
      |      statement statements
      ;

 statement: 'print' expression ';'
      |     'int'   identifier ';'
      |     identifier '=' expression ';'
      ;

 identifier: T_IDENT
      ;
```

한 identifier는 T_IDENT token으로서 반환되고, 그리고 우리는 이미 print statements를 파싱하는 코드를 가지고 있다. 그러나, 우리가 이제 세 개의 statements의 types를 가지고 있기 때문에, 각각의 것을 다루는 함수를 작성하는 것이 말이 된다. `stmt.c`에 있는 우리의 top-level `statements()` 함수는 이제 이렇게 보인다:

```c
// Parse one or more statements
void statements(void) {

  while (1) {
    switch (Token.token) {
    case T_PRINT:
      print_statement();
      break;
    case T_INT:
      var_declaration();
      break;
    case T_IDENT:
      assignment_statement();
      break;
    case T_EOF:
      return;
    default:
      fatald("Syntax error, token", Token.token);
    }
  }
}
```

나는 이전 print statement code를 `print_statement()`로 옮겼고, 너는 스스로 그것을 찾을 수 있다.



## Variable Declarations

variable declarations을 봐보자. 이것은 새로운 파일 `decl.c`에 있는데, 우리가 나중에 많은 다른 종류의 declarations을 가질 것이기 때문이다.

```c
// Parse the declaration of a variable
void var_declaration(void) {

  // Ensure we have an 'int' token followed by an identifier
  // and a semicolon. Text now has the identifier's name.
  // Add it as a known identifier
  match(T_INT, "int");
  ident();
  addglob(Text);
  genglobsym(Text);
  semi();
}
```

그 `ident()`와 `semi()` 함수들은 `match()`에 대한 wrappers이다:

```c
void semi(void)  { match(T_SEMI, ";"); }
void ident(void) { match(T_IDENT, "identifier"); }
```

`var_declaration()`으로 돌아와서, 우리가 `Text` buffer에서 identifier를 스캔했다면, 우리는 이것을 lobal symbol table에 `addglob(Text)`로 추가할 수 있다. 거기에 있는 코드는 한 변수가 여러번 (지금은) 선언되는 것을 허용한다.



## Assignment Statements

여기에 `stmt.c`에 있는 `assignment_statement()`가 있다:

```c
void assignment_statement(void) {
  struct ASTnode *left, *right, *tree;
  int id;

  // Ensure we have an identifier
  ident();

  // Check it's been defined then make a leaf node for it
  if ((id = findglob(Text)) == -1) {
    fatals("Undeclared variable", Text);
  }
  right = mkastleaf(A_LVIDENT, id);

  // Ensure we have an equals sign
  match(T_EQUALS, "=");

  // Parse the following expression
  left = binexpr(0);

  // Make an assignment AST tree
  tree = mkastnode(A_ASSIGN, left, right, 0);

  // Generate the assembly code for the assignment
  genAST(tree, -1);
  genfreeregs();

  // Match the following semicolon
  semi();
}
```

우리는 두 개의 새로운 AST node types를 가진다. A_ASSIGN은 left-hand child에서 expression을 가지고, 그것을 right-hand child에게 할당한다. 그 right-hand child는 A_LVIDENT node가 될 것이다.

왜 나는 이 노드를 *A_LVIDENT*라고 불렀는가? 왜냐하면 그것은 *lvalue* identifier를 나타내기 때문이다. 그래서  [lvalue](https://en.wikipedia.org/wiki/Value_(computer_science)#lrvalue)가 무엇인가?

한 lvalue는 특정한 location에 묶여있는 값이다. 여기에서 한 변수의 값을 보유한 메모리에 있는 주소이다. 우리가 다음을 할 때:

```c
	area = width * height;
```

우리는 우변의 결과 (즉, *rvalue*)를 좌변에 있는 변수 (즉, *lvalue*)에 *assign*한다. 그 *rvalue*는 특정한 위치에 묶여있지 않다. 여기에서, 그 expression result는 아마도 어떤 임의의 register이다.

또한, assignment statement가 다음의 syntax를 가지고 있는 것을 주목해라

```BNF
  identifier '=' expression ';'
```

우리는 그 expression을 A_ASSIGN node의 left sub-tree로 만들 것이고, right sub-tree에 A_LVIDENT details를 저장한다. 왜? 왜냐하면 우리가 그것을 변수에 저장하기 *전에* 그 expression을 evaluate할 필요가 있기 때문이다.



## Changes to the AST Structure

우리는 이제 A_INTLIT AST nodes에 integerliteral value 또는 A_IDENT AST nodes를 윟한 symbol의 details 둘 중 하나를 저장할 필요가 있다. 나는 이것을 위해 AST structure에 *union*를 추가했다 (`defs.h`):

```c
// Abstract Syntax Tree structure
struct ASTnode {
  int op;                       // "Operation" to be performed on this tree
  struct ASTnode *left;         // Left and right child trees
  struct ASTnode *right;
  union {
    int intvalue;               // For A_INTLIT, the integer value
    int id;                     // For A_IDENT, the symbol slot number
  } v;
};
```



## Generating the Assignment Code

`gen.c`에 있는 `genAST()`에서의 변화를 봐보자

```c
int genAST(struct ASTnode *n, int reg) {
  int leftreg, rightreg;

  // Get the left and right sub-tree values
  if (n->left)
    leftreg = genAST(n->left, -1);
  if (n->right)
    rightreg = genAST(n->right, leftreg);

  switch (n->op) {
  ...
    case A_INTLIT:
    return (cgloadint(n->v.intvalue));
  case A_IDENT:
    return (cgloadglob(Gsym[n->v.id].name));
  case A_LVIDENT:
    return (cgstorglob(reg, Gsym[n->v.id].name));
  case A_ASSIGN:
    // The work has already been done, return the result
    return (rightreg);
  default:
    fatald("Unknown AST operator", n->op);
  }
```

우리가 처음에 left-hand AST child를 evaluate하고, 그리고 left-hand sub-tree의 값을 보유하는 register number를 얻는다는 것을 주목해라. 우리는 이제 이 register number를 right-hand sub-tree에 넘긴다. 우리는 A_LVIDENT nodes를 위해 이것을 할 필요가 있다. 이것은 `cg.c`에 있는`cgstorglob()` 함수가 그 assignment expression의 rvalue result를 어떤 레지스터에 보유해야 하는지를 알기 위해서이다.

그래서, 이 AST tree를 고려해라:

```
           A_ASSIGN
          /        \
     A_INTLIT   A_LVIDENT
        (3)        (5)
```

우리는 A_INTLIT 연산을 evaluate하기 위해 `leftreg = genAST(n->left, -1);`를 호출한다. 이것은 `return (cgloadint(n->v.intvalue));`를 할 것이다. 즉, 3의 값으로 register를 load하고, register id를 반환한다.

그러고나서, 우리는 A_LVIDENT opreation을 evaluate하기 위해, `rightreg = genAST(n->right, leftreg);`를 호출한다. 이것은 `return (cgstorglob(reg, Gsym[n->v.id].name));`를 호출할 것이다. 즉, 그 register를 이름이 `Gsym[5]`에 있는 변수에 저장한다.

그러고나서, 우리는 A_ASSIGN case로 바꾼다. 모든 우리의 작업이 이미 처리 되었다. 그 rvalue는 여전히 register에 있다. 그래서, 그것을 거기에 두고, 반환하자. 우리는 다음과 같은 expressions을 할 수 있을 것이다:

```
a = b = c = 0;
```

여기에서 assignment는 statement일 뿐만 아니라, expression이다.



## Generating x86-64 Code

너는 내가 이전 `cgload()` 함수의 이름을 `cgloadint()`로 바꾸었다는 것을 눈치채었을 것이다. 이것은 좀 더 구체적이다. 우리는 이제, global variable에서 value를 load하는 함수를 가진다 (`cg.c`에서):

```c
int cgloadglob(char *identifier) {
  // Get a new register
  int r = alloc_register();

  // Print out the code to initialise it
  fprintf(Outfile, "\tmovq\t%s(\%%rip), %s\n", identifier, reglist[r]);
  return (r);
}
```

유사하게, 우리는 한 register를 variable에 저장하는 함수가 필요하다:

```c
// Store a register's value into a variable
int cgstorglob(int r, char *identifier) {
  fprintf(Outfile, "\tmovq\t%s, %s(\%%rip)\n", reglist[r], identifier);
  return (r);
}
```

우리는 또한 새로운 global integer variable를 생성하는 함수가 필요하다:

```c
// Generate a global symbol
void cgglobsym(char *sym) {
  fprintf(Outfile, "\t.comm\t%s,8,8\n", sym);
}
```

물론, 우리는 그 parser가 이 코드에 직접적으로 접근하도록 할 수 없다. 대신에, interface로서 역할을 하는 `gen.c`에 generic code generator에 있는 함수가 있다.

```c
void genglobsym(char* s) { cgglobsym(s); }
```



## Variable in Expressions

그래서 이제, 우리는 변수에 할당할 수 있다. 그러나, 우리가 한 변수의 값을 expression으로 어떻게 얻는가? 음, 우리는 이미 integer literal를 얻는 `primary()` 함수를 가진다. 그것을 수정해서, 한 변수의 값을 얻어오자:

```c
// Parse a primary factor and return an
// AST node representing it.
static struct ASTnode *primary(void) {
  struct ASTnode *n;
  int id;

  switch (Token.token) {
  case T_INTLIT:
    // For an INTLIT token, make a leaf AST node for it.
    n = mkastleaf(A_INTLIT, Token.intvalue);
    break;

  case T_IDENT:
    // Check that this identifier exists
    id = findglob(Text);
    if (id == -1)
      fatals("Unknown variable", Text);

    // Make a leaf AST node for it
    n = mkastleaf(A_IDENT, id);
    break;

  default:
    fatald("Syntax error, token", Token.token);
  }

  // Scan in the next token and return the leaf node
  scan(&Token);
  return (n);
}
```

변수를 보장하기 위해 T_IDENT case에서 체크하는 syntax는 우리가 그것을 사용하려고 하기전에 선언되었다는 것을 주목해라.

또한, 한 변수의 값을 *가져오는* 그 AST leaft node가 A_IDENT node라는 것을 주목해라. 한 변수에 저장하는 leaf는 A_LVIDENT node이다. 이것이 *rvalue*와 *lvalue*사이의 차이이다.



## Trying It Out

나는 변수 선언에 대해서 생각해보았는데, 그래서 이것을 `input02` file에서 시도해보자:

```c
int fred;
int jim;
fred= 5;
jim= 12;
print fred + jim;
```

우리는 이것을 하기 위해 `make test`를 할 수있다:

```shell
$ make test
cc -o comp1 -g cg.c decl.c expr.c gen.c main.c misc.c scan.c
               stmt.c sym.c tree.c
...
./comp1 input02
cc -o out out.s
./out
17
```

너가 볼 수 있듯이, 우리는 `fred + jim`을 계산했다. 그리고 이것은 5 + 12인 즉 17이다. 여기에 `out.s`에 있는 새로운 assembly lines이 있다:

```assembly
        .comm   fred,8,8                # Declare fred
        .comm   jim,8,8                 # Declare jim
        ...
        movq    $5, %r8
        movq    %r8, fred(%rip)         # fred = 5
        movq    $12, %r8
        movq    %r8, jim(%rip)          # jim = 12
        movq    fred(%rip), %r8
        movq    jim(%rip), %r9
        addq    %r8, %r9                # fred + jim
```



## Other Changes

나는 몇 가지 다른 변화를 만들었다. 내가 기억할 수 있는 유일한 주된 것은 fatal errors를 보고하는 것을 더 쉽게 만들기 위해 `misc.c`에 몇 가지 helper functions을 생성하는 것이다:

```c
// Print out fatal messages
void fatal(char *s) {
  fprintf(stderr, "%s on line %d\n", s, Line); exit(1);
}

void fatals(char *s1, char *s2) {
  fprintf(stderr, "%s:%s on line %d\n", s1, s2, Line); exit(1);
}

void fatald(char *s, int d) {
  fprintf(stderr, "%s:%d on line %d\n", s, d, Line); exit(1);
}

void fatalc(char *s, int c) {
  fprintf(stderr, "%s:%c on line %d\n", s, c, Line); exit(1);
}
```



## Conclusion and What's Next

많은 작업이였다. 우리는 symobl table management의 시작을 작성해야만 했었다. 우리는 두 개의 새로운 statement types를 다뤄야만 했었다. 우리는 새로운 토큰들과 새로운 AST node types를 추가해야만 했었다. 마지막으로, 우리는 올바른 x86-64 assembly output을 생성하기 위해 몇 가지 코드를 추가해야만 했었다.

몇 가지 예제 input files들을 작성해보고, 그 컴파일러가 그렇게 작동해야 하는 것처럼 하는지 보아라. 특히, 그것이 syntax errors와 semantic errors들을 탐지할 수 있는지 보아라 (declaration을 사용하는 것 없이).

우리의 compiler writing journey의 다음 part에서, 우리는 우리의 언어에 6가지 comparision operators를 추가할 것이다. 이것은 우리가 그 이후의 part에서 control structures에서 시작하게 허용할 것이다.





























 