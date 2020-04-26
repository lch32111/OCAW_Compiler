https://github.com/DoctorWkt/acwj/tree/master/05_Statements



# Part 5 : Statements

우리 언어의 문법에 "적절한" statements(표현)을 추가할 시간이다.  나는 이것과 같은 code의 lines들을 작성할 수 있길 원한다:

```
   print 2 + 3 * 5;
   print 18 - 6/3 + 4*2;
```

물론, 우리는 whitespace를 무시하고 있기 때문에, 한 표현에 대한 모든 tokens들이 같은 line에 있어야 할 필요성은 없다. 각 statements는 `print`라는 keyword와 함께 시작하고, semicolon으로 종료된다. 그래서 이것들은 우리의 언어에서 새로운 tokens이 될 것이다.



## BNF Description of the Grammar

우리는 이미 expressions를 위한 BNF notation을 보았었다. 이제 statements의 위의 types에 대한 BNF syntax를 정의하자:

```makefile
statements: statement
	| statement statements
	;

statement: 'print' expression ';'
	;
```

한 input file은 여러가지 statements로 구성된다. 한 statement이거나 또는 한 statement 이후에 더 많은 statements가 있는 것 일 수 있다. 각 statement는 `print` keyword로 시작하고, 그러고나서 한 expression이 나오고, 그러고나서 semicolon이 나온다.



## Chnages to the Lexical Scanner

위의 syntax(문법)을 파싱하는 코드에 갈 수 있기 전에, 우리는 존재하는 코드의 좀 더 많은 조각들을 추가할 필요가 있다. lexical scanner로 시작해보자.

semicolons을 위해 한 token을 추가하는 것은 쉬울 것이다. 이제, 그 `print` keyword이다. 나중에, 우리는 그 언어에서 많은 keywords를 가질 것이고, 더해서 우리의 변수에 대한 identifiers를 가질 것이다. 그래서 우리는 우리가 그것들을 다루는데 도와줄 어떤 코드를 추가할 필요가 있다.

`scan.c`에서, 나는 SubC compiler에서 내가 빌린 이 코드를 추가했다. 그것은 non-alphanumeric character에 도달할 때 까지 alphanumeric characters를 buffer에 read한다.

```c
// Scan an identifier from the input file and
// store it in buf[]. Return the identifier's length
static int scanident(int c, char *buf, int lim) {
  int i = 0;

  // Allow digits, alpha and underscores
  while (isalpha(c) || isdigit(c) || '_' == c) {
    // Error if we hit the identifier length limit,
    // else append to buf[] and get next character
    if (lim - 1 == i) {
      printf("identifier too long on line %d\n", Line);
      exit(1);
    } else if (i < lim - 1) {
      buf[i++] = c;
    }
    c = next();
  }
  // We hit a non-valid character, put it back.
  // NUL-terminate the buf[] and return the length
  putback(c);
  buf[i] = '\0';
  return (i);
}
```

우리는 또한 그 언어에서 keywords를 인지할 함수가 필요하다. 한 방법은 keywords들의 한 목록을 가지는 것일 것이고, 그 list를 방문하고, `scanident()`의 buffer에 대해 각각 하나씩 `strcmp()`하는 것이다. SubC의 코드는 최적화를 가지고 있다: `strcmp()`를 하기 전에 첫 번째 문자에 대해 매칭시킨다. 이것은 수십개의 keywords들에 대한 비교를 가속화 시킨다. 지금 당장 우리는 이 최적화가 필요 없지만, 나는 그것을 나중에 넣는다:

```c
// Given a word from the input, return the matching
// keyword token number or 0 if it's not a keyword.
// Switch on the first letter so that we don't have
// to waste time strcmp()ing against all the keywords.
static int keyword(char *s) {
  switch (*s) {
    case 'p':
      if (!strcmp(s, "print"))
        return (T_PRINT);
      break;
  }
  return (0);
}
```

이제, `scan()`에서 switch statement 끝에서, 우리는 semicolons과 keywords를 인지하는 코드를 추가한다:

```c
    case ';':
      t->token = T_SEMI;
      break;
    default:

      // If it's a digit, scan the
      // literal integer value in
      if (isdigit(c)) {
        t->intvalue = scanint(c);
        t->token = T_INTLIT;
        break;
      } else if (isalpha(c) || '_' == c) {
        // Read in a keyword or identifier
        scanident(c, Text, TEXTLEN);

        // If it's a recognised keyword, return that token
        if (tokentype = keyword(Text)) {
          t->token = tokentype;
          break;
        }
        // Not a recognised keyword, so an error for now
        printf("Unrecognised symbol %s on line %d\n", Text, Line);
        exit(1);
      }
      // The character isn't part of any recognised token, error
      printf("Unrecognised character %c on line %d\n", c, Line);
      exit(1);
```

나는 또한 keywords와 identifiers를 저장하는 global `Text` buffer를 추가했다:

```c
#define TEXTLEN         512             // Length of symbols in input
extern_ char Text[TEXTLEN + 1];         // Last identifier scanned
```



## Changes to the Expression Parser

이제까지, 우리의 input files은 단일의 expression만을 포함했다; 그러므로, `binexpr()`에 있는 (`expr.c`에서) 우리의 Pratt parser code에서, 우리는 그 parser를 종료하기 위해 이 코드를 가졌었다:

```c
  // If no tokens left, return just the left node
  tokentype = Token.token;
  if (tokentype == T_EOF)
    return (left);
```

우리의 새로운 문법으로, 각 expression은 semicolon으로 종료된다. 따라서, 우리는 `T_SEMI` tokens를 찾아내고 expression parsing을 종료하기 위해 expression parser에 있는 코드를 바꿀 필요가 있다:

```c
// Return an AST tree whose root is a binary operator.
// Parameter ptp is the previous token's precedence.
struct ASTnode *binexpr(int ptp) {
  struct ASTnode *left, *right;
  int tokentype;

  // Get the integer literal on the left.
  // Fetch the next token at the same time.
  left = primary();

  // If we hit a semicolon, return just the left node
  tokentype = Token.token;
  if (tokentype == T_SEMI)
    return (left);

    while (op_precedence(tokentype) > ptp) {
      ...

          // Update the details of the current token.
    // If we hit a semicolon, return just the left node
    tokentype = Token.token;
    if (tokentype == T_SEMI)
      return (left);
    }
}
```



## Changes to the Code Generator

나는 `gen.c`에 있는 generic code generator를 `cg.c`에 있는 CPU-specific code와 별개로 유지하길 원한다. 그것은 또한 그 컴파일러의 나머지가 오직 `gen.c`에 있는 함수들만 호출해야만 할 것이고, `gen.c`만이 `cg.c`에 있는 코드를 호출할 것이라는 것을 의미한다.

결국, 나는 몇 가지 새로운 "front-end" 함수들을 `gen.c`에 정의했다:

```c
void genpreamble()        { cgpreamble(); }
void genpostamble()       { cgpostamble(); }
void genfreeregs()        { freeall_registers(); }
void genprintint(int reg) { cgprintint(reg); }
```



## Adding the Parser for Statements

우리는 새로운 파일 `stmt.c`를 가진다. 이것은 우리의 언어에서 모든 주된 statements를 위한 파싱 코드를 가질 것이다. 지금 당장, 우리는 내가 위에서 주었떤 statements에 대한 BNF grammar를 파싱할 필요가 있다. 이것은 이 단일의 함수로 처리된다. 나는  그 재귀 정의를 한 loop로 변환한다:

```c
// Parse one or more statements
void statements(void) {
  struct ASTnode *tree;
  int reg;

  while (1) {
    // Match a 'print' as the first token
    match(T_PRINT, "print");

    // Parse the following expression and
    // generate the assembly code
    tree = binexpr(0);
    reg = genAST(tree);
    genprintint(reg);
    genfreeregs();

    // Match the following semicolon
    // and stop if we are at EOF
    semi();
    if (Token.token == T_EOF)
      return;
  }
}
```

각 loop에서, 그 코드는 `T_PRINT` token을 찾는다. 그러고나서, 그것은 expression을 파싱하기 위해 `binexpr()`를 호출한다. 마지막으로, 그것은 `T_SEMI` token을 찾는다. 만약 한 T_EOF token이 나온다면, 우리는 그 loop에서 나온다.

각 expression tree 후에, `gen.c`에 있는 코드는 그 트리를 assembly code로 변환하기 위해 호출되고, 최종 값을 출력하기 위해 그 assembly인 `printint()`를 호출하도록 변환된다.



## Some Helper Functions

위의 코드에서 두 개의 새로운 helper functions이 있다. 나는 그것들을 새로운 파일인 `misc.c`에 넣었다:

```c
// Ensure that the current token is t,
// and fetch the next token. Otherwise
// throw an error 
void match(int t, char *what) {
  if (Token.token == t) {
    scan(&Token);
  } else {
    printf("%s expected on line %d\n", what, Line);
    exit(1);
  }
}

// Match a semicon and fetch the next token
void semi(void) {
  match(T_SEMI, ";");
}
```

이러한 것들은 parser에서 syntax checking의 부분을 형성한다. 나중에, 나는 우리의 syntax checking를 더 쉽게 만들기 위해 `match()`를 호출하는 좀 더 짧은 함수들을 추가할 것이다.



## Changes to `main()`

`main()`는 old input files에서 single expression를 파싱하기위해 직접적으로 `binexpr()`를 호출했었다. 이제는 이렇게 한다:

```c
  scan(&Token);                 // Get the first token from the input
  genpreamble();                // Output the preamble
  statements();                 // Parse the statements in the input
  genpostamble();               // Output the postamble
  fclose(Outfile);              // Close the output file and exit
  exit(0);
```



## Trying It Out

새로운 코드와 변경된 코드에 대한 것들이였다. 새로운 코드를 돌려보자. 여기에 새로운 input file이 있다, `input01`:

```
print 12 * 3;
print 
   18 - 2
      * 4; print
1 + 2 +
  9 - 5/2 + 3*5;
```

그렇다, 나는 여러 라인에 걸쳐서 토큰들을 펼치도록 하는 것을 체크하기로 했다. 그 input file을 컴파일하고 작동시키기 위해, `make test`를 해라:

```shell
$ make test
cc -o comp1 -g cg.c expr.c gen.c main.c misc.c scan.c stmt.c tree.c
./comp1 input01
cc -o out out.s
./out
36
10
25
```

그리고 그것은 작동한다!



## Conclusion and What's Next

우리는 우리의 언어에 첫 번째 "실제" statement grammar를 추가했다. 나는 그것을 BNF notation으로 정의했지만, 재귀가 아닌 loop로 그것을 구현하는 것이 더 쉬어ㅜㅆ다. 걱정하지 말아라, 우리는 곧 recursive parsing으로 하는 것으로 돌아갈 것이다.

우리가 그 scanner를 수정해야했던 방법을 따라, keywords와 identifiers에 대한 support를 추가하고, 좀 더 깔끔하게 generic code generator와 CPU-specific generator를 분리해라.

우리의 compiler writing journey의 다음 파트에서, 우리는 그 언어에 대한 변수들을 추가할 것이다. 이것은 상당한 양의 작업을 요구할 것이다.