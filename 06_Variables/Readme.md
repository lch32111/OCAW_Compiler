https://github.com/DoctorWkt/acwj/tree/master/06_Variables

# Part 6 : Variables

나는 컴파일러에 global variables를 추가하는 것을 끝내고, 내가 의심했듯이, 그것은 많은 작업이었다. 또한, 컴파일러에서 꽤 많은 모든 파일들이 프로세스에서 수정되었다. 그래서 journey의 이 파트는 길 것이다.



## What Do We Want from Variables?

우리는 다음의 것들을 할 수 있길 원한다:

* Declare variables
* 저장된 values를 얻기위해 variables를 사용
* variables에 할당

여기에 우리의 test program이 될 `input02`가 있다:

```c
int fred;
int jim;
fred= 5;
jim= 12;
print fred + jim;
```

가장 명백한 볂화는, 그 문법이 이제 변수 선언, 할당 표현, 그리고 expressions에서 변수 이름을 가진다는 것이다. 그러나 우리가 그것에 들어가기 전에, 우리가 변수들을 어떠게 구현하는지 보자.



## The Symbol Table

모든 컴파일러는 [symbol table](https://en.wikipedia.org/wiki/Symbol_table)을 필요로 한다. 나중에, 우리는 한 개 이상의 global variables를 보유할 것이다. 그러나 지금은, 그 table에 한 entry의 구조가 있따 (`defs.h`)에서:

```c
// Symbol table structure
struct symtable {
  char *name;                   // Name of a symbol
};
```

우리는 `data.h`에 symbols의 한 array를 가진다:

```c
#define NSYMBOLS        1024            // Number of symbol table entries
extern_ struct symtable Gsym[NSYMBOLS]; // Global symbol table
static int Globs = 0;                   // Position of next free global symbol slot
```

`Globs`는 실제로 `sym.c`에 있는데, 이것은 symbol table를 관리하는 파일이다. 여기에서, 우리는 이러한 관리 함수들을 가진다:

* `int findglob(char* s)` : 그 symbol `s`가 global symbol table에 있는지를 결정한다. 그것의 slot position을 반환하거나, 발견되지 않는다면 -1를 반환한다.
* `static int newglob(void)` : 새로운 global symbol slot의 position을 얻거나, 만약 positions이 부족하다면 die한다.
* `int addglob(char* name)` : symbol table에 global symbol를 추가한다. symbol table에 있는 slot number를 반환한다.



## Scanning and New Tokens

만약 너가 example input file를 본다면, 우리는 몇 가지 새로운 토큰들이 필요하다:

* T_INT로 알려진 'int'
* T_EQUALS로 알려진 '='
* T_IDENT로 알려진 identifier names.

`=`의 scanning은 `scan()`에 추가하기 쉽다:

```c
  case '=':
    t->token = T_EQUALS; break;
```

우리는 `keyword()`에 'int' keyword를 추가할 수 있다:

```c
  case 'i':
    if (!strcmp(s, "int"))
      return (T_INT);
    break;
```



























 