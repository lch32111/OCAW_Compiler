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









































