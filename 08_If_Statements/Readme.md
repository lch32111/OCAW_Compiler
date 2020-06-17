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



