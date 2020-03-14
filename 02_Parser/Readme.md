# Part 2: Introduction to Parsing

우리의 compiler writing journey의 이 파트에서, 나는 한 parser의 기본에 대해 소개할 것이다. 내가 첫 번째 파트에서 언급했었 듯이, 그 parser의 일은 input의 syntax와  structural elements를 인지하는 것이고, 그것들이 그 언어의 *문법*에 맞는지 보장하는 것이다.

우리는 이미 우리가 스캔할 수 있는 몇 가지 언어적인 elements들, 즉 우리의 tokens들을 가지고 있다:

* 4가지 기본 수학 연산자들 : `*`, `/`, `+` 그리고 `-`
* 1개 이상의 숫자 `0`.. `9`를 갖는 10진수의 전체 숫자들

이제 우리의 parser가 인지할 언어를 위한 문법을 정의하자

## BNF : Backus-Naur Form

만약 너가 컴퓨터 언어들을 다루려고 한다면, 어떤 시점에 너는 [BNF ](https://en.wikipedia.org/wiki/Backus–Naur_form)의 사용을 우연히 만나게 될 것이다. 나는 우리가 인지하고 싶은 문법을 표현하기 위해 여기에서 BNF syntrax를 충분히 소개할 것이다.

우리는 문법이 수학 식들을 전체 숫자들로 표현하기를 원한다. 여기에서 그 grammar의 BNF 설명이 있다:

```
expression: number
		  | expression '*' expression
		  | expression '/' expression
		  | expression '+' expression
		  | expression '-' expression
		  ;
		  
number:	T_INTLIT
		;
```

그 vertical bars는 문법에서의 옵션들을 분리한다. 그래서 위의 것은 말한다:

* 한 expression(식)은  숫자일 수 있거나,
* 한 식은 '*' token으로 나누어진 두 개의 식이거나
* 한 식은 '/' token으로 나누어진 두 개의 식이거나
* 한 식은 '+' token으로 나누어진 두 개의 식이거나
* 한 식은 '-' token으로 나누어진 두 개의 식이다.
* 한 number는 항상 T_INTLIT token이다.

문법의 BNF 정의가 *재귀적*인 것은 꽤 명백할 것이다 : 한 식은 다른 식을 참조하여 정의된다. 그러나 그 재귀를 "bottom-out(끝까지)"하는 방법이 있다 : 한 expression이 number인게 밝혀질 때, 이것은 항상 T_INTLIT token이고 따라서 재귀하지 않는다.

BNF에서, 우리는 "expression"과 "number"를 non-terminal symbols라고 말하는데, 그것들이 grammar에서 규칙에 의해 만들어졌기 때문이다.  그러나, T_INTLIT은 어떠한 규칙에 의해서도 정의되지 않았기 때문에 *terminal* symbol이다. 대신에, 그것은 그 언어에서 이미 인지되는 token이다. 유사하게, 그 4개의 maths operator tokens은 terminal symbols이다.

## Recursive Descent Parsing

우리의 언어를 위한 문법이 재귀적이라면, 우리가 그것을 재귀적으로 파싱하려고 하는 것이 말이 된다. 우리가 할 필요가 있는 것은 token을 읽어들이고, 그러고나서 그 다음 token을 *앞서 보는 것이다*. 다음 토큰이 무엇인지를 기반으로, 우리는 그 input을 파싱하는데 무슨 path가 필요한지를 결정할 수 있다. 이것은 우리가 이미 호출되었던 함수를 재귀적으로 호출하는 것을 요구할지도 모른다.

우리의 경우에, 어떤 expression에 있는 첫 번째 token은 숫자일 것이고, 그 뒤에 수학 연산자가 따라 올지도 모른다. 그 이후에, 오직 하나의 숫자가 있을지도 모르거나, 새로 전체 expression의 시작이 있을지도 모른다. 우리는 이것을 어떻게 재귀적으로 파싱할 수 있는가?

우리는 이것처럼 보이는 pseudo-code를 작성할 수 잇다:

```
function expression()
{
	Scan and check the first token is a number. Error if it's not
	Get the next token
	If we have reached the end of the input, return, i.e. base case
	
	Otherwise, call expression()
}
```

이 함수를 입력 `2 + 3 - 5 T_EOF`에 대해 작동시켜보자. `T_EOF`는 input의 end를 반영하는 토큰이다. 나는 `expression()`에 대한 각 호출에 번호를 붙일 것이다.

```
expression0:
	Scan in the 2, it's a number
	Get next token, +, which isn't T_EOF
	Call expression()
	
		expression1:
			Scan in the 3, it's a number
			Get next token, -, which isn't T_EOF
			Call expression()
			
				expression2:
					Scan in the 5, it's a number
					Get next token, T_EOF, so return from expression2
			return from expression1
	return from expression0
```

그렇다. 그 함수는 재귀적으로 그 입력 `2 + 3 - 5 T_EOF`를 파싱할 수 있다.

물론, 우리는 그 입력으로 어떠한 것도 하지 않았지만, 그것은 parser의 일이 아니다. 그 parser의 일은 그 input를 *인지*하는 것이고, 어떠한 syntax errors를 경고하는 것이다. 어떤 누군가는 그 입력에 대해 *semantic analysis*를 하려고 할 것이다, 즉 그 입력의 의미를 이해하고 수행하느 ㄴ것이다.

> 나중에, 너는 이것이 사실, 옳지 않다는 것을 알게 될 것이다.  syntax analysis와 semantic analysis가 뒤얽히는 것은 종종 말이 된다.

## Abstract Syntax Trees

semantic analysis를 하기 위해서, 우리는 그 인지된 input을 이해하거나, 그것을 또 다른 포맷으로 변환하는 (예를들어 assembly code) 코드가 필요하다. 그 journey의 이 파트에서, 우리는 그 입력을 위한 interpreter를 구성할 것이다. 그러나 거기에 도달하기 위해, 우리는 그 입력을 처음에 [abstract syntax tree](https://en.wikipedia.org/wiki/Abstract_syntax_tree)로, 또한 AST로 알려진 것으로 변환할 것이다.

나는 너가 ASTs에 대한 이 짧은 설명을 읽기를 매우 추천한다:

* [Leveling Up One’s Parsing Game With ASTs](https://medium.com/basecs/leveling-up-ones-parsing-game-with-asts-d7a6fc2400ff) by Vaidehi Joshi

> 위의 것을 번역했다. [여기에서](Leveling Up One's Parsing Game With ASTs.md)

이것은 잘 쓰여져있고, ASTs의 목적과 구조를 설명하느 ㄴ것을 정말 도와준다. 나는 너가 돌아올 때 여기에 있을 것이다.

우리가 구성할 AST에서 각 노드의 구조는 `defs.h`에 설명되어 있다:

```c
// AST node types
enum {
  A_ADD, A_SUBTRACT, A_MULTIPLY, A_DIVIDE, A_INTLIT
};

// Abstract Syntax Tree structure
struct ASTnode {
  int op;                               // "Operation" to be performed on this tree
  struct ASTnode *left;                 // Left and right child trees
  struct ASTnode *right;
  int intvalue;                         // For A_INTLIT, the integer value
};
```

`op` 값이 `A_ADD`, 그리고 `A_SUBTRACT`같은 어떤 AST nodes들은 `left`와 `right`에 의해 가리켜지는 두 개의 자식 ASTs들을 가진다. 나중에 ,우리는 그 sub-trees의 값들을 더하거나 뺄 것이다.

대안적으로, `op` value가 A_INTLIT인 AST node는 정수 값을 나타낸다. 그것은 어떠한 sub-tree children를 갖지 않는다. `intvalue` field에서 값만을 가진다.



## Building AST Nodes and Trees

`tree.c`에 있는 코드는 ASTs를 구성하는 함수들을 가진다. 가장 일반적인 함수인 `mkastnode()`는 AST node에서 모든 4개의 필드들에 대한 값을 취한다. 그것은 노드를 할당하고, 그 field values를 채우고, 노드에 대한 포인터를 반환한다.

```c
// Build and return a generic AST node
struct ASTnode *mkastnode(int op, struct ASTnode *left,
                          struct ASTnode *right, int intvalue) {
  struct ASTnode *n;

  // Malloc a new ASTnode
  n = (struct ASTnode *) malloc(sizeof(struct ASTnode));
  if (n == NULL) {
    fprintf(stderr, "Unable to malloc in mkastnode()\n");
    exit(1);
  }
  // Copy in the field values and return it
  n->op = op;
  n->left = left;
  n->right = right;
  n->intvalue = intvalue;
  return (n);
}
```

이것을 보면, 우리는 leaf AST node (즉, 자식이 없는 것)을 만드는 좀 더 특정한 함수를 만들 수 있고, 단일 child를 가진 AST node를 만들 수 있다.

```c
// Make an AST leaf node
struct ASTnode *mkastleaf(int op, int intvalue) {
  return (mkastnode(op, NULL, NULL, intvalue));
}

// Make a unary AST node: only one child
struct ASTnode *mkastunary(int op, struct ASTnode *left, int intvalue) {
  return (mkastnode(op, left, NULL, intvalue));
```



## Purpose of the AST

우리는 나중에 expression의 최종값을 재귀적으로 계산하기 위해서 탐색할 수 있게 하기 위해서, 우리가 인지하는 각 expression을 저장하기 위해 AST를 사용할 것이다. 우리는 수학 연산자들의 precedence를 다루고 싶어한다. 여기에 예제가 있다.

`2 * 3 + 4 * 5`의 식을 고려해라. 이제, 곱은 덧셈보다 더 높은 우선순위를 갖는다. 그러므로, 우리는 곱셈 operands를 함께 *bind*하고 싶고, 우리가 더 하기 전에 이러한 연산들을 수행하고 싶다.

만약 우리가 AST tree를 생성한다면 이렇게 보인다

```
          +
         / \
        /   \
       /     \
      *       *
     / \     / \
    2   3   4   5
```

그러고나서, 트리를 탐색할 때, 우리는 처음에 `2 * 3`를 수행하고, 그러고나서 `4 * 5`를 할 것이다. 우리가 이러한 결과를 갖는다면, 우리는 그러고나서 덧셈을 수행하기 위해 그 tree의 root까지 그것들을 보낼 수 있다.



## A Nasive Expression Parser

이제, 우리는 우리의 scanner로부터 token values를 AST node operation values로서 재사용할 수 있찌만, 나는 tokens과 AST nodes의 개념을 별개로 유지하고 싶다. 그래서 우선, 나는 token values를 AST node operation values로 매핑하는 함수를 가질 것이다. 그 parser의 나머지를 따라, 이것은 `expr.c`에 있다:

```c
// Convert a token into an AST operation.
int arithop(int tok) {
  switch (tok) {
    case T_PLUS:
      return (A_ADD);
    case T_MINUS:
      return (A_SUBTRACT);
    case T_STAR:
      return (A_MULTIPLY);
    case T_SLASH:
      return (A_DIVIDE);
    default:
      fprintf(stderr, "unknown token in arithop() on line %d\n", Line);
      exit(1);
  }
}
```

switch문의 default 문은 우리가 그 주어진 token을 AST node type으로 바꿀 수 없을 때 fire된다. 그것은 우리의 parser에서 syntax checking의 부분을 형성할 것이다.

우리는 다음 토큰이 integer literal인지를 체크하고 그 literal value를 가지는 AST node를 구성하는 함수가 필요하다. 여기에 그것이 있다:

```c
// Parse a primary factor and return an
// AST node representing it.
static struct ASTnode *primary(void) {
  struct ASTnode *n;

  // For an INTLIT token, make a leaf AST node for it
  // and scan in the next token. Otherwise, a syntax error
  // for any other token type.
  switch (Token.token) {
    case T_INTLIT:
      n = mkastleaf(A_INTLIT, Token.intvalue);
      scan(&Token);
      return (n);
    default:
      fprintf(stderr, "syntax error on line %d\n", Line);
      exit(1);
  }
}
```

이것은 global 변수 `Token`이 있다는 것을 가정하고, 그것이 이미 input에서 scanned 되어있는 가장 최큰 token을 가지고 있다고 가정한다. `data.h`에서:

```c
extern_ struct token    Token;
```

그리고 `main()`에서:

```c
  scan(&Token);                 // Get the first token from the input
  n = binexpr();                // Parse the expression in the file
```

이제 우리는 parser를 위한 코드를 작성할 수 있다:

```c
// Return an AST tree whose root is a binary operator
struct ASTnode *binexpr(void) {
  struct ASTnode *n, *left, *right;
  int nodetype;

  // Get the integer literal on the left.
  // Fetch the next token at the same time.
  left = primary();

  // If no tokens left, return just the left node
  if (Token.token == T_EOF)
    return (left);

  // Convert the token into a node type
  nodetype = arithop(Token.token);

  // Get the next token in
  scan(&Token);

  // Recursively get the right-hand tree
  right = binexpr();

  // Now build a tree with both sub-trees
  n = mkastnode(nodetype, left, right, 0);
  return (n);
}
```

이 naive parser code에서 다른 연산자의 우선순위를 다루는 것이 없다는 것을 유의해라. 그것이 보이듯이, 그 코드는 모든 연산자들을 동일한 우선순위를 갖은채 다룬다. 만약 너가 `2 * 3 + 4 * 5`식을 파싱할 때 코드를 따라가다 보면, 너는 그것이 이 AST를 구성할 것이라는 것을 알게 된다:

```
     *
    / \
   2   +
      / \
     3   *
        / \
       4   5
```

이것은 명백히 옳지 않다. 그것이 `4 * 5` 곱하여 20을 얻고, 그러고나서 `3 + 20`를 하여 23을 얻는다. `2 * 3`을 하여 6을 얻는 대신에.

그래서 나는 이것을 왜 했는가? 나는 너에게 간단한 parser를 작성하는 것은 쉽지만, 그것을 semantic analysis를 하게 만드는 것은 더 어렵다는 것을 보여주길 원했었다.



## Interpreting the Tree

이제 우리는 (부정확한) AST tree를 가졌으니, 그것을 해석할 어떤 코드를 작성해보자. 다시, 우리는 그 트리를 탐색하는 재귀 코드를 작성할 것이고 여기에 그 pseudo-code가 있다:

```
interpretTree:
	처음에, 왼쪽 sub-tree를 해석하고, 그것의 값을 얻어라.
	그러고나서, 오른쪽 sub-tree를 해석하고 그것의 값을 얻어라.
	우리의 tree에 있는 root에 있는 노드에서 연산을 수행해라
	두 개의 sub-tree values로, 그리고 이 값을 반환해라
```

그 올바른 AST tree로 돌아가서:

```
          +
         / \
        /   \
       /     \
      *       *
     / \     / \
    2   3   4   5
```

그 호출 구조는 이것처럼 보일 것이다:

```
interpretTree0(tree with +):
  Call interpretTree1(left tree with *):
     Call interpretTree2(tree with 2):
       No maths operation, just return 2
     Call interpretTree3(tree with 3):
       No maths operation, just return 3
     Perform 2 * 3, return 6

  Call interpretTree1(right tree with *):
     Call interpretTree2(tree with 4):
       No maths operation, just return 4
     Call interpretTree3(tree with 5):
       No maths operation, just return 5
     Perform 4 * 5, return 20

  Perform 6 + 20, return 26
```



## Code to Interpret the Tree

이것은 `interp.c`에 있고, 위의 pseudo-code를 따른다:

```c
// Given an AST, interpret the
// operators in it and return
// a final value.
int interpretAST(struct ASTnode *n) {
  int leftval, rightval;

  // Get the left and right sub-tree values
  if (n->left)
    leftval = interpretAST(n->left);
  if (n->right)
    rightval = interpretAST(n->right);

  switch (n->op) {
    case A_ADD:
      return (leftval + rightval);
    case A_SUBTRACT:
      return (leftval - rightval);
    case A_MULTIPLY:
      return (leftval * rightval);
    case A_DIVIDE:
      return (leftval / rightval);
    case A_INTLIT:
      return (n->intvalue);
    default:
      fprintf(stderr, "Unknown AST operator %d\n", n->op);
      exit(1);
  }
}
```

또 다시, switch 문에 있는 default 문은 우리가 AST node type를 해석할 수 없을 때 fires한다. 이것은 우리의 parser에서 semantic checking의 부분을 형성할 것이다.



## Building the Parser

여기에 `main()`에서 interpreter에 대한 호출같은 코드가 있다:

```c
  scan(&Token);                 // Get the first token from the input
  n = binexpr();                // Parse the expression in the file
  printf("%d\n", interpretAST(n));      // Calculate the final result
  exit(0);
```

너는 이제 그 parser를 다음을 하여 빌드할 수 있다:

```c
$ make
cc -o parser -g expr.c interp.c main.c scan.c tree.c
```

나는 너가 parser를 테스트할 몇 가지 input files들을제공하지만, 물론 너는 너 자신만의 것을 만들 수 있다. 기억해라, 그 계산된 결과들은 부정확하지만,그 parser는 연속되는 숫자들, 연속하는 operators 그리고 input의 끝에 없는 숫자 같은 input errors들을탐지할 수 있어야 한다. 나는 또한 interpreter에 디버깅 코드를 추가했다. 그래서 너는 어떤 AST tree nodes가 어떤 순서로 구해지는지 볼 수 있다.

```shell
$ cat input01
2 + 3 * 5 - 8 / 3

$ ./parser input01
int 2
int 3
int 5
int 8
int 3
8 / 3
5 - 2
3 * 3
2 + 9
11

$ cat input02
13 -6+  4*
5
       +
08 / 3

$ ./parser input02
int 13
int 6
int 4
int 5
int 8
int 3
8 / 3
5 + 2
4 * 7
6 + 28
13 - 34
-21

$ cat input03
12 34 + -56 * / - - 8 + * 2

$ ./parser input03
unknown token in arithop() on line 1

$ cat input04
23 +
18 -
45.6 * 2
/ 18

$ ./parser input04
Unrecognised character . on line 3

$ cat input05
23 * 456abcdefg

$ ./parser input05
Unrecognised character a on line 1
```



## Conclusion and What's next

parser은 언어의 문법을 인지하고, 컴파일러에 대한input이 이 문법에 순응하는지를 체크한다. 만약 그렇지 않는다면, 그 parser은 에러 메세지를 출력해야 한다. 우리의 expression grammar는 재귀적이기 때문에, 우리는 우리의 expressions를 인지하기 위해 recursive descent parser를 작성하는 것을 선택했다.

지금 당장 parser는 작동한다. 위의 output에 의해 보여지듯이,그러나 그것은 input의 semantics을 옳바르게 얻는 것을 실패한다. 다시 말해서, 그것은 그 expressions의 올바른 값을 계산하지 않는다.

우리의 compiler writing journey의 다음 부분에서, 우리는 그 parser를 수정할 것인데, 그것이 올바른 수학 결과들을 얻기 위해 expressions의 semantic analysis를 하도록 하기 위해서이다.