https://github.com/DoctorWkt/acwj/tree/master/09_While_Loops



# Part 9 : While Loops

여행의 이번 파트에서, 우리는 우리의 언어에 WHILE loops를 추가할 것이다. 어떤 의미에서, WHILE loop는 else clause가 없는 IF 문과 많이 비슷하다. 우리가 그 loop의 top으로 다시 돌아간다는 점을 제외하고.

그래서, 이것은 :

```
	while (condition is true) {
		statements;
	}
```

다음으로 바뀌어야 한다:

```
Lstart:	evalute condition
		jump to Lend if condition false
		statements
		jump to Lstart
Lend:
```

이것은 IF문과 사용했었던 우리가 scanning, parsing 그리고 코드 생성 구조를 빌릴 수 있다는 것과 WHILE문을 다루기 위해 작은 변화만을 만들 수 있다는 것을 의미한다.

우리가 이것이 어떻게 되게 하는지 봐보자.



## New Tokens

우리는 T_WHILE이라는 새로운 토큰이 필요한데, 새로운 'while' keyword를 위해서이다. `defs.h`과 `scan.c`에 대한 변화들은 명백해서 나는 여기에서 그것들을 생략할 것이다.



## Parsing the While Syntax

WHILE loop를 위한 BNF grammar는 다음이다:

```
 // while_statement: 'while' '(' true_false_expression ')' compound_statement ;
```

그리고 우리는 이것을 파싱하기 위해 `stmt.c`에서 함수가 필요하다. 여기에 그것이 있다; IF 문의 파싱과 비교하여 이것의 간단함에 주목해라:

```c
// Parse a WHILE 
```



