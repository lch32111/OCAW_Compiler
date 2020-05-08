https://github.com/DoctorWkt/acwj/tree/master/07_Comparisons

# Part 7 : Comparison Operators

나는 다음에 IF statements를 추가할 예정이였었지만, 몇 가지 comparison operators를 처음에 추가하는 게 더 낫다고 깨달았다. 이것은 꽤 쉽다고 알려져 있는데, 왜냐하면 그것들은 이미 있는 것들과 똑같이 binary operators이기 때문이다.

그래서, 6개의 comparison operators를 추가하기위해 필요한 변홛르이 무엇인지 봐보자 : `==`, `!=`, `<`, `>`, `<=` and `>=`.



## Adding New Tokens

우리는 6개의 새로운 토큰들을 가질 것이다. 그래서 `defs.h`에 추가할 것이다:

```c
// Token types
enum {
  T_EOF,
  T_PLUS, T_MINUS,
  T_STAR, T_SLASH,
  T_EQ, T_NE,
  T_LT, T_GT, T_LE, T_GE,
  T_INTLIT, T_SEMI, T_ASSIGN, T_IDENT,
  // Keywords
  T_PRINT, T_INT
};
```

나는 tokens들을 재배열 했는데, 우선순위가 있는 것들은 low-to-high 우선순위로 오게되도록 했다. 우선순위를 갖지 않는 토큰들 전에.



## Scanning The Tokens

이제 우리는 그것들을 scan해야한다. 우리가 `=` and `==`, `<` and `<=`, `>` and `>=`를 구분해야 한다는 것을 주의해라. 그래서 우리는 input으로부터 추가 character를 읽을 필요가 있을 것이고, 만약 우리가 그것이 필요하지 않다면 다시 넣어야 할 것이다. 여기에 `scan.c`에 있는 새로운 코드가 있다:

```c
  case '=':
    if ((c = next()) == '=') {
      t->token = T_EQ;
    } else {
      putback(c);
      t->token = T_ASSIGN;
    }
    break;
  case '!':
    if ((c = next()) == '=') {
      t->token = T_NE;
    } else {
      fatalc("Unrecognised character", c);
    }
    break;
  case '<':
    if ((c = next()) == '=') {
      t->token = T_LE;
    } else {
      putback(c);
      t->token = T_LT;
    }
    break;
  case '>':
    if ((c = next()) == '=') {
      t->token = T_GE;
    } else {
      putback(c);
      t->token = T_GT;
    }
    break;
```

나는 또한 T_ASSIGN에 `=` token의 이름을 바꾸었는데, 그것과 새로운 T_EQ token사이에 혼동하지 않도록 하기 위해서이다.

## New Expression Code

우리는 이제 6개의 새로운 토큰들을 scan할 수 있다. 그래서 이제, 그것들이 expressions에 나타날 떄, 그것들을 파싱해야 하고, 그것들의 operator precedence를 강제해야 한다.

이제, 너는 다음의 것을 작업할 것이다:

* 나는 self-compiling compiler가 될 것을 구성하고 있다
* C 언어로
* SubC compiler를 reference로 해서.

암시하는 바는, 나는 C의 subset으로서 충분한 컴파일러를 작성하고 있다는 것이고, 그것은 그 자기 자신을 컴파일 할 것이라는 것이다. 그러므로, 나는 일반적인 [C operator precedence order](https://en.cppreference.com/w/c/language/operator_precedence)를 사용할 것이다. 이것은 comparison operators가 multiply and divide보다 더 높은 precedence를 가진다는 것을 의미한다.

나는 또한 tokens을 AST node types에 매핑할 switch statement가 점점 더 커지기만 할 거라는 것을 깨달았다. 그래서 나는 모든 binary operators에 대해 그들 사이에서 1:1 매핑이 있도록 하기위해 AST node types를 재배열하기로 했다:

```c
// AST node types. The first few line up
// with the related tokens
enum {
  A_ADD=1, A_SUBTRACT, A_MULTIPLY, A_DIVIDE,
  A_EQ, A_NE, A_LT, A_GT, A_LE, A_GE,
  A_INTLIT,
  A_IDENT, A_LVIDENT, A_ASSIGN
};
```

이제 `expr.c`에서, 나는 token을 AST node conversion으로 간단하게 할 수 있고, 그 새로운 tokens들의 precedence에 추가할 수 있따:

```c
// Convert a binary operator token into an AST operation.
// We rely on a 1:1 mapping from token to AST operation
static int arithop(int tokentype) {
  if (tokentype > T_EOF && tokentype < T_INTLIT)
    return(tokentype);
  fatald("Syntax error, token", tokentype);
}

// Operator precedence for each token. Must
// match up with the order of tokens in defs.h
static int OpPrec[] = {
  0, 10, 10,                    // T_EOF, T_PLUS, T_MINUS
  20, 20,                       // T_STAR, T_SLASH
  30, 30,                       // T_EQ, T_NE
  40, 40, 40, 40                // T_LT, T_GT, T_LE, T_GE
};
```

parsing과 operator precedence에 대해 이게 다이다!

## Code Generation

그 6개의 새로운 operators가 binary operators이기 때문에, 그것들을 다루기 위해 `gen.c`에 있는 generic code generator를 수정하는 것은 쉽다:

```c
  case A_EQ:
    return (cgequal(leftreg, rightreg));
  case A_NE:
    return (cgnotequal(leftreg, rightreg));
  case A_LT:
    return (cglessthan(leftreg, rightreg));
  case A_GT:
    return (cggreaterthan(leftreg, rightreg));
  case A_LE:
    return (cglessequal(leftreg, rightreg));
  case A_GE:
    return (cggreaterequal(leftreg, rightreg));
```

## x86-64 Code Generation

이제 조금 까다로워 진다. C에서, comparison operators를 한 결과를 반환한다. 만약 그것들이 true를 evaluate한다면, 그것들의 결과는 1이다. 만약 그것들이 false를 evaluate한다면, 그것들의 결과는 0이다. 우리는 이것을 반영하는 x86-64 assembly code를 작성할 필요가 있다.

운 좋게도, 이것을 하는 몇 가지 x86-64 instructions이 있다. 그 방법을 따라 처리하는데 있어서 몇 가지 문제들이 있다. 이 x86-64 instruction을 고려해라:

```assembly
    cmpq %r8,%r9
```

위의 `cmpq` instruction은 %r9 - %r8를 수행하고, negative and zero flags를 포함하는 몇 가지 상태 flags들을 설정한다. 따라서, 우리는 비교의 결과를 보기위해 flag combinations을 볼 수 있다:

| Comparison | Operation | Flags If True          |
| ---------- | --------- | ---------------------- |
| %r8 == %r9 | %r9 - %r8 | Zero                   |
| %r8 != %r9 | %r9 - %r8 | Not Zero               |
| %r8 > %r9  | %r9 - %r8 | Not Zero, Negative     |
| %r8 < %r9  | %r9 - %r8 | Not Zero, Not Negative |
| %r8 >= %r9 | %r9 - %r8 | Zero or Negative       |
| %r8 <= %r9 | %r9 - %r8 | Zero or Not Negative   |

두 개의 flag values를 기반으로 한 register를 1 또는 0으로 설정하는 6개의 x86-64 instructions이 있다: 위의 테이블 행의 순서로, `sete`, `setne`, `setg`, `setl`, `setge` and `setle`. 

문제는 이러한 instructions들이 한 register의 lowest byte만 설정한다는 것이다. 만약 그 register가 이미 lowest byte 밖에서 bits를 설정했다면, 그것들은 설정된채로 있을 것이다. 그래서 우리는 한 변수를 1로 설정할지도 모르지만, 만약 그것이 이미 값 1000 (decimal)를 가지고 있따면, 그러면 그것은 이제 우리가 원하지 않은 1001이 될 것이다.

해결책은 원치않는 bits를 제거하기위해 `setX` instruction 이후에 그 register에 `andq` 를 하는 것이다. `cg.c`에서, 이것을 하는 일반적인 비교 함수가 있다:

```c

```

























