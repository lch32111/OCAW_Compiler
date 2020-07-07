https://github.com/DoctorWkt/acwj/tree/master/10_For_Loops

# Part 10: FOR Loops

우리의 compiler writing journey의 이 파트에서, 나는 FOR loops를 추가할 것이다. 구현에 측면에서 해결해야 할 점이 있는데, 나는 그것이 어떻게 해결되는지에 대해 이야기 하기 전에 설명하고 싶다.



## The FOR Loop Syntax

나는 너가 FOR loops의 syntax와 친숙하다고 가정한다. 한 예시는

```c
   for (i=0; i < MAX; i++)
    printf("%d\n", i);
```

나는 우리의 언어를 위해 이 BNF syntax를 사용할 것이다:

```BNF
for_statement: 'for' '(' preop_statement ';'
						 true_false_expression ';'
						 postop_statement ')' compound_statement ;

preop_statement:	statement  ;		(for now)
postop_statement:	statement  ;		(for now)
```

그 `preop_statement`는 그 loop가 시작하기 전에 작동된다. 나중에, 우리는 무슨 종류의 행동들이 여기에서 수행될 수 있는지를 정확히 제한해야 할 것이다 (예를들어, IF statements이 안된다는지). 그러고나서 `true_false_expression`이 평가된다. 만약 true라면, 그 loop는 `compound_statement`를 실행한다. 이것이 된다면, 그 `postop_statement`가 수행된다. 그리고 그 코드는 `true_false_expression`을 다시하여 반복한다.



## The Wrinkle

해결해야 할 것은 `postop_statement`가 `compound_statement`전에 파싱되지만, 우리가 `compound_statement`를 위한 코드 이후에 `postop_statement`를 위한 코드를 생성해야만 한다는 것이다.

이 문제를 해결하는 몇 가지 방법들이 있다. 내가 이전의 컴파일러를 작성했을 때, 나는 `compound_statement` assembly code를 임시 버퍼에 넣고, 내가 `postop_statement`를 위한 코드를 생성했다면 그 buffer를 다시 쓰는 것을 선택했었다. SubC compiler에서, Nils은 똑똑한 labels를 이용하고, 정확한 순서를 보장하기 위해 코드의 실행을 "꿰어줄"(thread) labels로 점프한다.

그러나 우리는 여기에서 AST tree를 구성한다. 올바른 순서로 생성된 assembly code를 얻기 위해 그것을 사용해보자.



## What Sort of AST Tree?

너는 FOR loop가 4개의 structural components를 가진다는 것을 눈치챘을지도 모른다:

1. `preop-statement`
2. `true_false_expression`
3. `postop_statement`
4. `compound_statement`

나는 AST node structure가 4개의 자식을 가지도록 바꾸길 원하지 않는다.  그러나, 우리는 FOR loop를 augmented WHILE loop로서 시각화 할 수 있다:

```c
   preop_statement;
   while ( true_false_expression ) {
     compound_statement;
     postop_statement;
   }
```

우리는 이 structure를 반영하기 위해 우리의 존재하는 node types들로 AST tree를 구성할 수 있는가? 그렇다:

```
         A_GLUE
        /     \
   preop     A_WHILE
             /    \
        decision  A_GLUE
                  /    \
            compound  postop
```

직접 이 tree를 위에서 아래로, 왼쪽에서 아래로 탐색해라. 그리고 우리가 옳바른 순서로 어셈블리 코들르 생성할 것이라고 너 자신을 설득해라. 우리는 `compound_statement`와 `post_statement`를 함께 glue했어야만 했다. WHILE loop가 종료될 때, 그것이 `compound_statement`와 `postop_statement` 둘 다에 대해 생략하게 하기 위해서 이다.

이것은 또한, 우리가 새로운 `T_FOR` token이 필요하지만, 우리는 새로운 AST node type을 가지지 않을 것이라는 것을 의미한다. 그래서 유일한 컴파일러 변화는 scanning과 parsing이다.



## Tokens and Scanning

'for'를 위한 새로운 키워드가 있다. 관련된 token은 T_FOR이다. 여기에서 큰 변화는 없다.



## Parsing Statements

우리는 parser에게 구조적 변화를 만들 필요가 있다. FOR 문법에 대해, 나는 오직 `preop_statement`과 `postop_statement`로서 단일 statement를 원한다. 지금 당장, 우리는 `compound_statement()` 함수를 가지고 있는데, 이것은 간단히 그것이 right curly bracket '}'에 도달할 때 까지 반복한다. 우리는 이것은 분리할 필요가 있다. 그래서 `compound_statement`는 한 statement를 얻기위해 `single_statement()`를 호출한다.

그러나, 또 다른 wrinkle이 있다. `assignment_statement()`에서 assignment statements의 존재하는 파싱을 보아라. 그 parser는 statement의 끝에 semicolon을 찾아야 한다.

그것은 compound statements에 좋지만, 이것은 FOR loops에 대해 작용하지 않을 것이다. 나는 다음과 같은 것을 써야만 할 것이다:

```c
  for (i=1 ; i < 10 ; i= i + 1; )
```

각 assignment statement는 semicolon으로 끝나야 하기 때문이다.

우리가 필요한 것은 single statement parser가 semicolon을 scan하지 않는 것이지만, compound statement parser에 따라 그것을 남기게 하는 것이다. 그래서 우리는 어떤 statements들 (assignment statements)에 대해서 semicolons을 스캔하고, 다른 statements (연속적인 IF statements 사이에서 하지 않게)에 대해서 하지 않게 한다.

모두 설명되었으니, 이제 새로운 single and compound statement parsing code를 봐보자:

```c
// Parse a single statement
// and return its AST
static struct ASTnode *single_statement(void) {
  switch (Token.token) {
    case T_PRINT:
      return (print_statement());
    case T_INT:
      var_declaration();
      return (NULL);		// No AST generated here
    case T_IDENT:
      return (assignment_statement());
    case T_IF:
      return (if_statement());
    case T_WHILE:
      return (while_statement());
    case T_FOR:
      return (for_statement());
    default:
      fatald("Syntax error, token", Token.token);
  }
}

// Parse a compound statement
// and return its AST
struct ASTnode *compound_statement(void) {
  struct ASTnode *left = NULL;
  struct ASTnode *tree;

  // Require a left curly bracket
  lbrace();

  while (1) {
    // Parse a single statement
    tree = single_statement();

    // Some statements must be followed by a semicolon
    if (tree != NULL &&
	(tree->op == A_PRINT || tree->op == A_ASSIGN))
      semi();

    // For each new tree, either save it in left
    // if left is empty, or glue the left and the
    // new tree together
    if (tree != NULL) {
      if (left == NULL)
	left = tree;
      else
	left = mkastnode(A_GLUE, left, NULL, tree, 0);
    }
    // When we hit a right curly bracket,
    // skip past it and return the AST
    if (Token.token == T_RBRACE) {
      rbrace();
      return (left);
    }
  }
}
```

나는 또한 `print_statement()`와 `assignment_statement()`에서 `semi()`에 대한 호출을 제거했다.



## Parsing FOR Loops





































