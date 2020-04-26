https://github.com/DoctorWkt/acwj/tree/master/03_Precedence



# Part 3: Operator Precedence (연산자 우선순위)

우리는 우리의 compiler writing journey의 이전 파트에서, 한 parser가 반드시 우리의 언어의 semantis를 강요하지 않는 것을 보았다. 그것은 오직 grammar의 syntax와 structural rules를 강요한다.

우리는 `2 * 3 + 4 * 5`같은 식의 잘못된 값을 계산하는 코드로 끝났었다. 왜냐하면 그 코드는 다음 처럼 보이는 AST를 생성했기 때문이다:

```
     *
    / \
   2   +
      / \
     3   *
        / \
       4   5
```

다음 대신에:

```
          +
         / \
        /   \
       /     \
      *       *
     / \     / \
    2   3   4   5
```

이것을 해결하기 위해, 우리는 operator precedence를 수행하기 위해 우리의 parser에 코드를 추가해야만 한다. 이것을 하는 (적어도) 두 가지 방법들이 있다:

* operator precedence가 언어의 문법에 explicit하게 만들기
* 존재하는 parser를 operator precedence table로 영향을 미치기



## Making the Operator Precedence Explicit

여기에 그 journey의 마지막 부분의 우리의 문법이 있다:

```
expression: number
          | expression '*' expression
          | expression '/' expression
          | expression '+' expression
          | expression '-' expression
          ;

number:  T_INTLIT
         ;
```

4개의 수학 연산자 사이에 어떠한 구분이 없다는 것을 주목해라. 차이가 있도록 그 문법을 조정해보자:

```
expression: additive_expression
    ;

additive_expression:
      multiplicative_expression
    | additive_expression '+' multiplicative_expression
    | additive_expression '-' multiplicative_expression
    ;

multiplicative_expression:
      number
    | number '*' multiplicative_expression
    | number '/' multiplicative_expression
    ;

number:  T_INTLIT
         ;
```

우리는 이제 두 가지 유형의 expressions을 갖는다 : *additive expressions*과 *multiplicative expressions*. 그 문법은 숫자들이 이제 multiplicative expressions의 일부가 되도록 강요한다. 이것은 '*'와 '/' 연산자들이 다른 쪽에서 숫자들에 더 강하게 바인드되도록 강요하고, 더 높은 우선순위를 갖게 한다.

어떠한 additive expression은 실제로 그 자체로 multiplicative expression이거나 또는 additive (즉 multiplicative) expression인데, '+' 또는 '-' 연산자가 나오고, 그러고나서 또 다른 multiplicative expression이 나온다. 그 additive expression은 이제 multiplicative expression 보다 더 낮은 우선순위에 있다.



## Doing The Above in the Recursive Descent Parser

우리는 어떻게 우리의 문법의 위 버전을 가져와서, 그것을 우리의 recursive descent parser에 구현하는가? 나는 파일 `expr2.c`에서 이것을 했고, 아래에서 그 코드를 다룰 것이다.

그 답은 '*'과 '/' 연산자들을 다룰 `multiplicative_expr()`를 가질 것이고, 더 낮은 우선순위인 '+', '-' 연산자들을 다루는 `additive_expr()` 함수를 가진다.

두 함수들은 어떤 것과 한 연산자들을 읽어들일 것이다. 그러고나서,  같은 precedence에 다음의 연산자들이 있는 동안, 각 함수는 그 입력을 좀 더 파싱할 것이고, 그 첫 번째 연산자로 왼쪽과 오른쪽 절반을 합친다.

그러나, `additive_expr()`은 더 높은 우선순위를 갖는 `multiplicative_expr()` 함수 뒤로 미뤄둬야 할 것이다. 이것이 처리되는 방법이다.



## `additive_expr()`

```c
// Return an AST tree whose root is a '+' or '-' binary operator
struct ASTnode *additive_expr(void) 
{
  struct ASTnode *left, *right;
  int tokentype;

  // Get the left sub-tree at a higher precedence than us
  left = multiplicative_expr();

  // If no tokens left, return just the left node
  tokentype = Token.token;
  if (tokentype == T_EOF)
    return (left);

  // Loop working on token at our level of precedence
  while (1) 
  {
    // Fetch in the next integer literal
    scan(&Token);

    // Get the right sub-tree at a higher precedence than us
    right = multiplicative_expr();

    // Join the two sub-trees with our low-precedence operator
    left = mkastnode(arithop(tokentype), left, right, 0);

    // And get the next token at our precedence
    tokentype = Token.token;
    if (tokentype == T_EOF)
      break;
  }

  // Return whatever tree we have created
  return (left);
}
```

시작하자 마자, 우리는 즉시 첫 번째 operator가 high-precedence인 '*' 또는 '/'인 경우인 `multiplicative_expr()`를 호출한다.

따라서, 우리가 `while` loop에 도달할 때, 우리는 '+' 또는 '-' operator를 가졌다는 것을 안다. 우리는 input에 남아있는 tokens이 없을 때 까지 loop한다. 즉, 우리가 T_EOF token에 도달할 때.

loop 안에서, 우리는 어떠한 future operators가 우리보다 더 높은 precedence인 경우에 또 다시 `multiplicative_expr()`를 호출한다. 다시, 이것은 그것들이 그렇지 않을 때 return할 것이다.

우리가 left and right sub-tree를 가진다면, 우리는 그것들을 우리가 loop에서 지난 번에 얻은 연산자와 결합한다. 이것은 반복하는데, 만약 우리가 `2 + 4  + 6`의 식을 가졌다면, 우리는 다음의 AST tree를 가지게 될 것이다:

```
       +
      / \
     +   6
    / \
   2   4
```

그러나 만약 `multiplicative_expr()`이 그것 자신의 higher precedence operators를 가진다면,우리는 sub-trees와 그것들 안의 여러가지 노드들을 결합할 것이다.



## multiplicative_expr()

```c
// Return an AST tree whose root is a '*' or '/' binary operator
struct ASTnode *multiplicative_expr(void) {
  struct ASTnode *left, *right;
  int tokentype;

  // Get the integer literal on the left.
  // Fetch the next token at the same time.
  left = primary();

  // If no tokens left, return just the left node
  tokentype = Token.token;
  if (tokentype == T_EOF)
    return (left);

  // While the token is a '*' or '/'
  while ((tokentype == T_STAR) || (tokentype == T_SLASH)) {
    // Fetch in the next integer literal
    scan(&Token);
    right = primary();

    // Join that with the left integer literal
    left = mkastnode(arithop(tokentype), left, right, 0);

    // Update the details of the current token.
    // If no tokens left, return just the left node
    tokentype = Token.token;
    if (tokentype == T_EOF)
      break;
  }

  // Return whatever tree we have created
  return (left);
}
```

그 코드는 real integer literals를 얻기위해 `primary()` 호출하게 된다는 것을 제외하고 `additive_expr()`과 유사하다. 우리는 또한 우리의 높은 precedence level에 있는 operators 즉, '*', '/' operators를 가지게 될 때만 loop한다. 우리가 low precedence operator에 도달하자 마자, 우리는 이 시점에서 우리가 구성한 sub-tree를 반환한다. 이것은 low precedence operator를 다루기 위해 `additive_expr()`로 돌아간다.



## Drawbacks of the Above (위의 단점들)

explicit operator precedence로 recursive descent parser를 구성하는 위의 방법은 precedence의 올바른 level에도달하는데 필요한 모든 함수 호출 때문에 비효율적일 수 있다.  또한 각각의 수준의 operator precedence를 다뤄야 할 함수들이 있어야 만하고, 우리는 결국에 많은 코드 라인을 가지게 된다.



## The Alternative: Pratt Parsing

코드의 양을 줄이는 한 가지 방법은, grammar에서 explicit precedence를 복사하는 함수들을 갖는 대신에, 각 token과 연관된 precedence values의 값을 갖는 [Pratt parser](https://en.wikipedia.org/wiki/Pratt_parser)를 사용하는 것이다.

이 시점에, 나는 Bob Nystrom이 작성한 [Pratt Parsers: Expression Parsing Made Easy](https://journal.stuffwithstuff.com/2011/03/19/pratt-parsers-expression-parsing-made-easy/)를 읽기를 매우 추천한다. Pratt parsers는 여전히 나의 머리를 아프게 해서, 너가 할 수 있는 만큼 그리고 기본 개념에 편해지는 만큼만 읽어라.



## expr.c : Pratt Parsing

나는 `expr2.c`에 대해 drop-in replacement인 `expr.c`에 Pratt parsing을구현했다.  투어를 시작해보자.

첫 째로, 우리는 각 토큰에 대해 precedence levels를 결정한 어떤 코드가 필요하다:

```c
// Operator precedence for each token
static int OpPrec[] = { 0, 10, 10, 20, 20,    0 };
//                     EOF  +   -   *   /  INTLIT

// Check that we have a binary operator and
// return its precedence.
static int op_precedence(int tokentype) {
  int prec = OpPrec[tokentype];
  if (prec == 0) {
    fprintf(stderr, "syntax error on line %d, token %d\n", Line, tokentype);
    exit(1);
  }
  return (prec);
}
```

더 높은 숫자들 (예를들어, 20)은 낮은 숫자들 보다 더 높은 우선순위를 의미한다 (예를들어 10).

이제, 너는 다음을 질문할지도 모른다 왜 너가 `OpPrec[]`으로 불려지는 look-up table를 가지고 있는데 함수를 가지고 있는지에 대해.  그 답은: syntaxx errors를 찾아내기 위해서이다.

`234 101 + 12`같은 입력을 고려해라. 우리는 처음에서 두 번째 tokens을 스캔할 수 있다. 그러나 만약 우리가 간단히 두 번째 `101` precedence를 사용했다면, 우리는 그것이 operator인지 알아차리지 못할 것이다. 따라서, `op_precedence()` 함수는 올바른 grammar syntax를 강화한다.

이제, 각 precedence level에 대한 함수를 갖는 대신에, 우리는 operator precedences의 테이블를 사용하는 단일의 expression function을 가지게 된다:

```c
// Return an AST tree whose root is a binary operator.
// Parameter ptp is the previous token's precedence.
struct ASTnode *binexpr(int ptp) {
  struct ASTnode *left, *right;
  int tokentype;

  // Get the integer literal on the left.
  // Fetch the next token at the same time.
  left = primary();

  // If no tokens left, return just the left node
  tokentype = Token.token;
  if (tokentype == T_EOF)
    return (left);

  // While the precedence of this token is
  // more than that of the previous token precedence
  while (op_precedence(tokentype) > ptp) {
    // Fetch in the next integer literal
    scan(&Token);

    // Recursively call binexpr() with the
    // precedence of our token to build a sub-tree
    right = binexpr(OpPrec[tokentype]);

    // Join that sub-tree with ours. Convert the token
    // into an AST operation at the same time.
    left = mkastnode(arithop(tokentype), left, right, 0);

    // Update the details of the current token.
    // If no tokens left, return just the left node
    tokentype = Token.token;
    if (tokentype == T_EOF)
      return (left);
  }

  // Return the tree we have when the precedence
  // is the same or lower
  return (left);
}
```

첫 째로, 이것이 이전의 parser functions과 같이 여전히 재귀라는 것에 주목해라. 이 번에, 우리는 우리가 호출되기 전에 발견되었던 token의 precedence level를 얻게된다. `main()` 은 가장 낮은 precedence인 0으로 우리를 호출할 것이지만, 우리는 더 높은 values로 호출할 것이다.

너는 또한 코드가 `multiplicative_expr()`함수와 꽤 유사하다는 것을 알아야 한다 : integer literal를 읽고, 그 operator의 token type을 얻고, 그러고나서 트리를 구성하는 것을 반복한다.

그 차이는 loop condition과 body이다:

```c
multiplicative_expr():
  while ((tokentype == T_STAR) || (tokentype == T_SLASH)) {
    scan(&Token); right = primary();

    left = mkastnode(arithop(tokentype), left, right, 0);

    tokentype = Token.token;
    if (tokentype == T_EOF) return (left);
  }

binexpr():
  while (op_precedence(tokentype) > ptp) {
    scan(&Token); right = binexpr(OpPrec[tokentype]);

    left = mkastnode(arithop(tokentype), left, right, 0);

    tokentype = Token.token;
    if (tokentype == T_EOF) return (left);
  }
```

Pratt parser와 함께, 그 다음 operator는 우리의 현재 token보다 더 높은 우선순위를 가질 때, `primary()`로 다음 integer literal를 얻는 대신에, 우리는 operator precedence를 만드는 `binexpr(OpPrec[tokentype])`으로 호출한다.

우리가 우리의 precedenc level 또는 더 낮은 token에 도달한다면, 우리는 단순히

```c
return (left);
```



이것은 많은 노드들과 더 높은 precedence에 있는 operators들을 가진 sub-tree이거나 또는 우리와 같은 precedence를 가진 operator에 대한 single integer literal일지도 모른다.

이제 우리는 expression parsing을 하는 단일의 함수를 가지고 있다. 그것은 operator precedence를 enforce하는 작은 helper function을 사용하고, 따라서 우리의 언어를 semantics를 구현한다.



## Putting Both Parsers Into Action

너는 두 개의 프로그램들을 만들 수 있다, 각 파서를 가진 하나씩:

```bash
$ make parser                                        # Pratt Parser
cc -o parser -g expr.c interp.c main.c scan.c tree.c

$ make parser2                                       # Precedence Climbing
cc -o parser2 -g expr2.c interp.c main.c scan.c tree.c
```

너는 또한 우리의 journey의 이전 파트의 같은 입력파일로 두 개의 parsers들을 테스트할 수 있다:

```c
$ make test
(./parser input01; \
 ./parser input02; \
 ./parser input03; \
 ./parser input04; \
 ./parser input05)
15                                       # input01 result
29                                       # input02 result
syntax error on line 1, token 5          # input03 result
Unrecognised character . on line 3       # input04 result
Unrecognised character a on line 1       # input05 result

$ make test2
(./parser2 input01; \
 ./parser2 input02; \
 ./parser2 input03; \
 ./parser2 input04; \
 ./parser2 input05)
15                                       # input01 result
29                                       # input02 result
syntax error on line 1, token 5          # input03 result
Unrecognised character . on line 3       # input04 result
Unrecognised character a on line 1       # input05 result
```



## Conclusion and What's next

아마 조금 뒤돌아보고 우리가 어디에 왔는지를 볼 시간이다. 우리는 이제 다음을 가진다:

* 우리의 언어에서 tokens을 인지하고 반환하는 scanner
* 우리의 문법을 인지하고, syntax errors를 보고하고, Abstract Syntax Tree를 구성하는 parser
* 우리의 언어의 semantics를 구현하는 parser를 위한 precedence table
* Abstract Syntax Tree를 depth-first로 탐색하고 그 input에서 expression의 결과를 계산하는 interpreter.

우리가 아직 가지고 있지 않은 것은 compiler이다. 그러나 우리는 우리의 첫 번째 컴파일러를 만드는 것에 거의 가까워 져 있다.

우리의 compiler writing journey의 다음 파트에서, 우리는 그 interpreter를 대체할 것이다. 그것의 장소에서, 우리는 maths operator를 가진 각 AST node에 대해 x86-64 assembly code를 생성하는 translator를 작성할 것이다. 우리는 또한 그 generator가 만들어내는 assembly code를 지원할 어떤 assembly preamble를과 postamble를 생성할 것이다.