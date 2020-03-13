## Part 1 : Introduction to Lexical Scanning (어휘 스캔)

우리는 간단한 lexical scanner로 우리의 compiler writing journey를 시작한다. 내가 이전 파트에서 언급했듯이, scanner의 job은 lexical elements, 또는 *tokens*를 input language에서 확인하느 ㄴ것이다.

우리는 오직 5개의 lexical elements (어휘 요소)들만을 가진 언어로 시작할 것이다:

* 4개의 기본 수학 연산자 : `*`, `/`, `+`, 그리고 `-`
* 1개 이상의 숫자 `0`.. `9`를 가진 전체 숫자.

우리가 스캔하는 각 token은 이 구조에 담길 것이다 (`defs.h`)로 부터:

```c
// Token structure
struct token
{
    int token;
    int intvalue;
};   
```

거기에서, `token` field는 이러한 값들 중 하나가 될 수 있다 (`defs.h`로 부터):

```c
// Tokens
enum
{
  T_PLUS, T_MINUS, T_STAR, T_SLASH, T_INTLIT  
};
```

token이 `T_INTLIT`일 때 (즉, 정수 상수), 그 `intvalue` field는 우리가 스캔했던 정수의 값을 가지고 있을 것이다.



## Functions in `scan.c`

그 `scan.c` 파일은 우리의 lexical scanner의 함수들을 가지고 있다.  우리는 우리의 input file로부터 한 번에 하나의 문자를 읽어들일 것이다. 그러나, 우리가 input stream에서 미리 너무 많이 읽었다면, 한 문자를 "put back(다시 넣울)" 필요가 있을 시간이 있을 것이다. 우리는 또한 우리가 현재 무슨 line에 있는지를 추적하기를 원하는데, 이것은 우리가 우리의 debug messages에서 line number를 출력할 수 있기 위해서이다. 이러한 모든 것은 `next()` 함수에서 된다:

```c
// Get the next character from the input file
static int next(void)
{
    int c;
    
    if(Putback)			// Use the character put
    {
		c = Putback;	// back if there is one
        Putback = 0;
        return c;
    }
    
    c = fgetc(Infile);	// Read from input file
    if ('\n' == c)
        Line++;			// Increment line count
    return c;
}
```

그 `Putback`과 `Line` 변수들은 우리의 input file pointer에 따라 `data.h`에 정의된다:

```c
extern_ int Line;
extern_ int Putback;
extern_ FILE* Infile;
```

모든 C 파일들은 `extern_`이 `extern`으로 대체되는 것을 포함한다. 그러나 `main.c`는 그 `extern_`를 제거할 것이다; 그러므로, 이러한 변수들은 `main.c`에 "속하게"될 것이다.

마지막으로, 우리는 한 문자를 어떻게 그  input stream에 넣는가? 따라서:

```c
// Put back an unwanted character
static void putback(int c)
{
    Putback = c;
}
```



## 공백문자 무시하기 (Ignoring Whitespace)

우리는 whitespace character가 아닌 것을 얻을 때 까지 whitespace characters를 조용히 스킵하고 읽고 반환하는 함수가 필요하다. 따라서:

```c
// Skip past input that we don't need to deal with,
// i.e. whitespace, newlines. Return the first
// character we do need to deal with.
static int skip(void)
{
	int c;

	c = next();
	while (' ' == c || '\t' == c || '\n' == c || '\r' == c || '\f' == c)
	{
		c = next();
	}

	return (c);
}
```



## Scanning Tokens : `scan()`

그래서 이제, 우리는 whitespace를 생략하면서 문자들을 읽어 들일 수 있다; 우리는 또한 만약 한 문자가 너무 멀리 읽었다면 한 문자를 다시 넣을 수 있다. 우리는 이제 우리의 첫 번째 lexical scanner를 쓸 수 있다:

```c
// Scan and return the next token found in the input.
// Return 1 if token valid, 0 if no tokens left.
int scan(struct token *t) {
  int c;

  // Skip whitespace
  c = skip();

  // Determine the token based on
  // the input character
  switch (c) {
  case EOF:
    return (0);
  case '+':
    t->token = T_PLUS;
    break;
  case '-':
    t->token = T_MINUS;
    break;
  case '*':
    t->token = T_STAR;
    break;
  case '/':
    t->token = T_SLASH;
    break;
  default:
    // More here soon
  }

  // We found a token
  return (1);
}
```

간단한 하나의 문자 tokens을 위해 이것이 다이다 : 각 인지된 문자에 대해, 그것을 한 token으로 바꾸어라. 너는 다음과 같이 질문할지도 모른다 : 왜 그 인지된 문자를 `struct token`에 넣지 않는가? 그 답은, 나중에 우리가 `==` 같은 여러개의 문자 tokens과 `if`와 `while`과 같은 키워드들을 인지할 필요가 있을 것이기 때문이다.



## Integer Literal Values

사실, 우리는 이미 이 상황을 보아야 한다. 왜냐하면 우리는 또한 3827과 87731과 같은 정수 상수값들을 인지할 필요가 있기 때문이다. 여기에 `switch` 문의 놓친 `default` code가 있다:

```c
default:

// If it's a digit, scan the
// literal integer value in
if (isdigit(c)) {
    t->intvalue = scanint(c);
    t->token = T_INTLIT;
    break;
}

printf("Unrecognised character %c on line %d\n", c, Line);
exit(1);
```

일단, 우리가 decimal digit character에 도달한다면, 우리는 이 첫 번째 문자로 scanint() helper 함수를 호출한다. 그것은 scanned integer value를 반환할 것이다. 이것을 하기 위해서, 그것은 각 문자를 차례대로 읽어야 하고, 그것이 적절한 숫자인지를 체크하고, 최종 숫자를 구성해야 한다. 여기에 그 코드가 있다:

```c
// Scan and return an integer literal
// value from the input file. Store
// the value as a string in Text.
static int scanint(int c)
{
	int k, val = 0;

	// Convert each character into an int value
	while ((k = chrpos("0123456789", c)) >= 0)
	{
		val = val * 10 + k;
		c = next();
	}

	// We hit a non integer character, put it back
	putback(c);
	return val;
}
```

우리는 0의 `val` 값으로 시작한다. 우리가 `0`에서 `9`의 집합에서 한 문자를 얻을 때 마다, 우리는 `chrpos()`로 이것을 `int` 값으로 변환한다. 우리는 `val`를 10배 더 크게 만들고, 그러고나서 이 새로운 숫자를 그것에 더한다.

예를들어, 만약 우리가 문자 `3`, `2`, `8`를 가진다면, 우리는 다음을 한다:

* `val = 0 * 10 + 3`, 즉 3
* `val = 3 * 10 + 2`, 즉 32
* `val = 32 * 10 + 8`, 즉 328

끝으로, 너는 `putback(C)`에 대한 호출을 보았는가? 우리는 이 시점에 decimal digit이 아닌 문자를 발견했다. 우리는 간단히 그것을 버릴 수 없지만, 운 좋게도, 우리는 나중에 쓰기 위해 input stream에 다시 넣을 수 있다.

너는 또한 이 시점에 질문할지도 모른다: 왜 정수를 만들기 위해 간단히 '0'의 ASCII 값을 `c`에서 뺄셈 하지 않는가? 그 답은, 나중에, 우리가 또한 16진법 수를 변환하기 위해  `chrpos("0123456789abcdef")`를 할 수 있게 하기 위해서이다.

여기에 `chrpos()`에 대한 코드가 있다:



## Scanner 작동시키기

`main.c`에 있는 코드는 작동하는 위의 스캐너를 넣는다. 그 `main()`함수는 한 파일을 열고, 토큰을 위해 그것을 스캔한다:

```c
void main(int argc, char *argv[]) {
  ...
  init();
  ...
  Infile = fopen(argv[1], "r");
  ...
  scanfile();
  exit(0);
}
```

그리고 `scanfile()` loops는 새로운 토큰이 있으면 토큰의 세부사항을 출력한다:

```c
// List of printable tokens
char *tokstr[] = { "+", "-", "*", "/", "intlit" };

// Loop scanning in all the tokens in the input file.
// Print out details of each token found.
static void scanfile() {
  struct token T;

  while (scan(&T)) {
    printf("Token %s", tokstr[T.token]);
    if (T.token == T_INTLIT)
      printf(", value %d", T.intvalue);
    printf("\n");
  }
```



## 예시 입력 파일

나는 몇 가지 예제 입력 파일들을 제공한다. 그래서 너는 그 scanner가 각 파일에서 발견하는 것을 볼 수 있고, 그 scanner가 무슨 input files를 거절하는지를 볼 수 있다

```shell
$ make
cc -o scanner -g main.c scan.c

$ cat input01
2 + 3 * 5 - 8 / 3

$ ./scanner input01
Token intlit, value 2
Token +
Token intlit, value 3
Token *
Token intlit, value 5
Token -
Token intlit, value 8
Token /
Token intlit, value 3

$ cat input04
23 +
18 -
45.6 * 2
/ 18

$ ./scanner input04
Token intlit, value 23
Token +
Token intlit, value 18
Token -
Token intlit, value 45
Unrecognised character . on line 3
```



## 결론과 다음

우리는 작게 시작했고, 우리는 4개의 주된 수학 연산자와 정수 상수값을 인지하는 간단한 lexical scanner를 가지고 있다. 우리는 whitespace를 생략할 필요가 있고, 만약 input을 너무 많이 읽었다면 다시 문자를 되돌리는 것이 필요하다고 보았다.

단일의 문자 토큰들은 스캔하기에 쉽지만, multi-character tokens은 좀 더 어렵다. 끝으로, 그`scan()` 함수는 `struct token` 변수에서 input file로부터 다음 token을 반환한다

```c
struct token {
  int token;
  int intvalue;
};
```

우리의 compiler writing journey의 다음 파트에서, 우리는 우리의 input files의 문법을 해석하기 위해, recursive descent parser를 구성할 것이고, 각 파일에 대해 최종 값을 계산하고 출력할 것이다.
