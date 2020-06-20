https://github.com/DoctorWkt/acwj/tree/master/08_If_Statements

# Part 8 : If Statements

우리가 값들을 비교할 수 있으니까, 우리의 언어에 IF 문을 추가할 시간이다. 그래서, 첫 째로, IF문의 일반 syntax를 보고, 그것들이 어떻게 assembly 언어로 바뀌는지 보자.



## The IF Syntax

그 IF statement syntax는 :

```
  if (condition is true) 
    perform this first block of code
  else
    perform this other block of code
```

이제, 이것은 어떻게 보통 어셈블리어로 변경되는가? 우리는 opposite comparision을하고, 그 opposite comparison이 true라면 jump/branch한다:

```
     perform the opposite comparison
       jump to L1 if true
       perform the first block of code
       jump to L2
L1:
       perform the other block of code
L2:
   
```

여기에서 `L1`과 `L2`는 assembly language labels이다.



## Generating The Assembly in Our Compiler

이제 바로, 우리는 비교를 기반으로 하여 레지스터를 설정하기 위해 코드를 만들어낸다, 예르들어

```
	int x; x = 7 < 9;	From input 04
```

는 다음이 된다

```
	movq 	$7, %r8
	movq 	$9, %r9
	cmpq	%r9, %r8
	setl	%r9b		Set if less than
	andq	$255,%r9
```

그러나 IF statement에 대해, 우리는 opposite comparison에서 jump할 필요가 있다:

```
	if (7 < 9)
```

이것은 다음이 되어야 한다:

```
	movq	$7, %r8
	movq	$9, %r9
	cmpq	%r9, %r8
	jge		L1			Jump if greater than or equal to
	....
L1:
```

그래서 나는 우리의 journey의 이 파트에서 IF 문을 구현했다. 이것은 작업하고 있는 프로젝트이기 때문에, 나는 몇 가지 것들을 undo해야만 했고, journey의 일부로서 그것들을 리팩토링 해야했었다. 나는 그 방법을 따라 변화 된 것 뿐만 아니라 추가된 것들 까지 다룰 것이다.



## New Tokens and the Dangling Else

우리는 우리의 언어에 많은 새로운 tokens들이 필요할 것이다. 나는 또한 (이제)  [dangling else problem](https://en.wikipedia.org/wiki/Dangling_else)문제를 피하길 원한다. 끝으로, 나는 문법을 바꾸었는데, 모든 문장들의 그룹들이 '{'...'}' curly brackets로 감싸지게 하기 위해서이다; 나는 그러한 grouping을 "compound statement"라고 부른다. 우리는 IF 식을 가질 또한 '('...')' 괄호들이 필요하고, 더해서, 'if'와 'else' keywords가 필요하다. 따라서, 새로운 토큰들은 `defs.h`에 있다:

```
  T_LBRACE, T_RBRACE, T_LPAREN, T_RPAREN,
  // Keywords
  ..., T_IF, T_ELSE
```



## Scanning the Tokens

단일 문자 토큰들은 명백해야 하고, 나는 그것들을 스캔하는 코드를 주지 않을 것이다. 그 키워드들은 또한 꽤 명백해야 하지만, 나는 `scan.c`에서 `keyword()`의 스캐닝 코드를 줄 것이다:

```c
  switch (*s) {
    case 'e':
      if (!strcmp(s, "else"))
        return (T_ELSE);
      break;
    case 'i':
      if (!strcmp(s, "if"))
        return (T_IF);
      if (!strcmp(s, "int"))
        return (T_INT);
      break;
    case 'p':
      if (!strcmp(s, "print"))
        return (T_PRINT);
      break;
  }
```



## The New BNF Grammar

우리의 문법이 커지기 시작하고 있다. 그래서 나는 그것을 어느정도 다시쓴다:

```BNF
compound_statement: '{' '}'		// empty, i.e. no statement
	|	'{' statement '}'
	|	'{' statement statements '}'
	;

statement: 	print_statement
	|		declaration
	|		assignment_statement
	|		if_statement
	;
	
print_statement: 'print' expression ';' ;

declaration: 'int' identifier ';' ;

assignment_statement: identifier '=' expression ';' 	;

if_statement: 	if_head
	|			if_head 'else' compound_statement
	;

if_head: 'if' '(' ture_false_expression ')' compound_statement;

identifier: T_IDENT ;
```

나는 `true_false_expression`의 정의를 제외 했지만, 우리가 몇 가지 연산자를 더 추가하는 시점에 나는 그것을 추가할 것이다.

IF 문을 위한 문법에 주목해라: 그것은 `if_head` ('else' 절이 없는) 이거나 또는 `if_head` 뒤에 'else'와 `compound_statement`가 나온다.

나는 그들 자신만의 non-terminal name을 갖게 하기 위해 모든 다른 statement types들을 분리했다. 또한, 그 이전의 `statements` 의non-terminal은 이제 `compound_statement` non-terminal이고, 이것은 statements 주변에  '{' ... '}' 를 요구한다.

> Chan 2020-06-20
>
> https://en.wikipedia.org/wiki/Terminal_and_nonterminal_symbols
>
> terminal과 non-terminal이 정확히 무엇인지 알 수 없어서 Google에서 compiler non-terminal이라고 검색하니 이런 위키가 있었다. 여기에서 example쪽을 보면 무엇이 terminal이고 nonterminal인지 나온다. 간단하게 말해서, 이런 BNF처럼 문법을 정의하는데 있어서, 재귀로 계속 타고 들어가게 만드는 것들은 non-terminal이 되는 것이고, 더 이상 들어갈 게 없으면 terminal이라 부른다.

이것은 head에 있는 `compound_statement`가 '{'...'}'로 둘러싸여지고, 'else' 키워드 후에 어떤 `compound_statement`가 된다는 것을 의미한다. 그래서 만약 윌가 nested IF 문을 가진다면, 그것들은 다음처럼 보여야 한다:

```
  if (condition1 is true) {
    if (condition2 is true) {
      statements;
    } else {
      statements;
    }
  } else {
    statements;
  }
```

그리고 각 'else'가 어떤 것에 속해야 하는지에 대한 애매함이 없다. 이것은 dangling else problem을 해결한다. 나중에, 나는 '{'...'}'를 optional하게 만들 것이다.



## Parsing Compound Statements

그 옛날 `void statements()` 함수는 이제 `compound_statement()`이고, 이것 처럼 보인다:

```c
// Parse a compound statement
// and return its AST
struct ASTnode *compound_statement(void) {
  struct ASTnode *left = NULL;
  struct ASTnode *tree;

  // Require a left curly bracket
  lbrace();

  while (1) {
    switch (Token.token) {
      case T_PRINT:
        tree = print_statement();
        break;
      case T_INT:
        var_declaration();
        tree = NULL;            // No AST generated here
        break;
      case T_IDENT:
        tree = assignment_statement();
        break;
      case T_IF:
        tree = if_statement();
        break;
    case T_RBRACE:
        // When we hit a right curly bracket,
        // skip past it and return the AST
        rbrace();
        return (left);
      default:
        fatald("Syntax error, token", Token.token);
    }

    // For each new tree, either save it in left
    // if left is empty, or glue the left and the
    // new tree together
    if (tree) {
      if (left == NULL)
        left = tree;
      else
        left = mkastnode(A_GLUE, left, NULL, tree, 0);
    }
  }
```

첫 째로, 그 코드가 parser에게 `lbrace()`로 compound statement의 시작에서 '{'와 부합하도록 강요한다는 것에 주목해라, 그래서 우리는 `rbrace()`로 끝나는 '}'와 매칭될 때만 종료할 수 있다.

둘 째로, `print_statement()`, `assignment_statement()` 그리고 `if_statement()` 모두 AST tree를 반환한다. `compound_statement()`가 그렇게 하듯이. 우리의 이전 코드에서, `print_statement()`는 그 자체로 expression을 evaluate하기 위해 `genAST()`를 호출했었고, 그 이후에 `genprintint()`에 대한 호출이 다음에 나왔다. 유사하게, `assignment_statement()` 또한 할당을 하기 위해 `genAST()`를 호출했었다.

음, 이것은 우리가 여기 저기에서 AST trees들을 가진다는 것을 의미한다. 이것은 단일 AST tree를 생성하고, 그것을 위해 assembly code를 생성하기 위해 `genAST()`를 한 번 호출하는 것은 말이 된다.

이것은 의무는 아니다. 예를들어, SubC는 expressions을 위해 ASTs만을 생성한다. 언어의 구조적 부분들을 위해, statements 같이, SubC는 내가 compiler의 이전 부분에서 하고 있던 것처럼 그 코드 생성에 대한 특정한 호출을 만든다.

이제 나는 parser로 전체 input에 대해 단일의 AST tree를 생성하기로 결정했다. 그 input이 파싱되기만 한다면, 그 assembly output은 한 AST tree로부터 생성되어질 수 있다.

나중에, 아마도 각 함수에 대한 AST tree를 생성할 것이다. 나중에.



## Parsing the IF Grammar

우리는 recursive descent parser이기 때문에, IF statement를 파싱하는 것은 너무 나쁜 것이 아니다:

```c
// Parse an IF statement including
// any optional ELSE clause
// and return its AST
struct ASTnode *if_statement(void) {
  struct ASTnode *condAST, *trueAST, *falseAST = NULL;

  // Ensure we have 'if' '('
  match(T_IF, "if");
  lparen();

  // Parse the following expression
  // and the ')' following. Ensure
  // the tree's operation is a comparison.
  condAST = binexpr(0);

  if (condAST->op < A_EQ || condAST->op > A_GE)
    fatal("Bad comparison operator");
  rparen();

  // Get the AST for the compound statement
  trueAST = compound_statement();

  // If we have an 'else', skip it
  // and get the AST for the compound statement
  if (Token.token == T_ELSE) {
    scan(&Token);
    falseAST = compound_statement();
  }
  // Build and return the AST for this statement
  return (mkastnode(A_IF, condAST, trueAST, falseAST, 0));
}
```

지금 당장, 나는 `if (x-2)`같은 input을 다루고 싶지 않다. 그래서 나는 `binexpr()`의 binary expression을 6개의 비교 연산자인 A_EQ, A_NE, A_LT, A_GT, A_LE 또는 A_GE중의 하나인 root를 가지도록 제한했다.



## The Third Child

나는 적절하게 설명하지 않고 과거에 어떤 것을 밀수 했었다. `if_statement()`의 마지막 줄에, 나는 다음으로 AST node를 구성한다 :

```c
	mkastnode(A_IF, condAST, trueAST, falseAST, 0);
```

이것은 세 개의 AST sub-trees이다! 여기에서 무엇이 되고 있는가? 너가 볼 수 있 듯이, 그 IF statement는 세 개의 자식을 가질 것이다:

* condition을 evaluates하는 sub-tree
* 즉시 다음에 나오는 compound statement
* 'else' keyword 이후의 optional compound statement

그래서 우리는 이제 세 개의 자식을 가지는 AST node structure가 필요하다 (`defs.h` 에서) :

```c
// AST node types.
enum {
  ...
  A_GLUE, A_IF
};

// Abstract Syntax Tree structure
struct ASTnode {
  int op;                       // "Operation" to be performed on this tree
  struct ASTnode *left;         // Left, middle and right child trees
  struct ASTnode *mid;
  struct ASTnode *right;
  union {
    int intvalue;               // For A_INTLIT, the integer value
    int id;                     // For A_IDENT, the symbol slot number
  } v;
};
```

따라서, 한 A_IF tree는 이렇게 보인다:

```
                      IF
                    / |  \
                   /  |   \
                  /   |    \
                 /    |     \
                /     |      \
               /      |       \
      condition   statements   statements
```



## Glue AST Nodes

또한 새로운 A_GLUE AST node type이 있다. 이것은 무엇을 위해 사용되는가? 우리는 이제 많은 statements를가진 단일의 AST tree를 구성한다. 그래서, 우리는 그것들을 함께 붙이는 방법이 필요하다.

`compound_statement()` loop 코드의 끝을 다시 보아라:

```c
      if (left != NULL)
        left = mkastnode(A_GLUE, left, NULL, tree, 0);
```

우리가 새로운 sub-tree를 얻을 때 마다, 우리는 그것을 존재하는 tree에 붙여준다. 그래서, 이 statements의 순서에 대해:

```
    stmt1;
    stmt2;
    stmt3;
    stmt4;
```

우리는 다음을 결국에 가지게 된다:

```
             A_GLUE
              /  \
          A_GLUE stmt4
            /  \
        A_GLUE stmt3
          /  \
      stmt1  stmt2
```

그리고, 우리가 그 tree를 depth-first로 left에서 right로 탐색함에 따라, 이것은 여전히 올바른 순서로 assembly code를생성한다.



## The Generic Code Generator





































