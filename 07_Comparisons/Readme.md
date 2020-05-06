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