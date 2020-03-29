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

4개의 수학 연산자 사이에 어떠한 구부닝 없다는 것을 주목해라. 차이가 있도록 그 문법을 조정해보자:

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

그러나, `additive_expr()`은 더 높은 precedence `multiplicative_expr()` 함수를 뒤로 미뤄둬야 할 것이다. 이것이 처리되는 방법이다.





























