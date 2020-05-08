## 3 An Overview of x86-64

Intel과 AMD에 의해 공급되는 새로운 하드웨어와 이러한 머신들을 타겟으로 하는 GCC의 새로운 버전의 조합은 IA32 machines을 위해 생성된 것과  x86-64 코드를 상당히 다르게 만든다.

| C declaration | Intel data type    | GAS suffix | x86-64 Size (Bytes) |
| ------------- | ------------------ | ---------- | ------------------- |
| char          | Byte               | b          | 1                   |
| short         | Word               | w          | 2                   |
| int           | Double word        | 1          | 4                   |
| unsigned      | Double word        | 1          | 4                   |
| long int      | Quad word          | q          | 8                   |
| unsigned long | Quad word          | q          | 8                   |
| char *        | Quad word          | q          | 8                   |
| float         | Single precision   | s          | 4                   |
| double        | Double precision   | d          | 8                   |
| long double   | Extended precision | t          | 16                  |

Figure1 :  **x86-64의 standard data types의 Sizes**. long integers와 pointers 둘 다 8 bytes를 요구한다. IA32에서는 4 bytes로 요구되는 것과 비교된다.



주된 기능은 다음을 포함한다:

* Pointers와 long integers는 64bits long이다. Integer arithmetic operations은 8, 16, 32, 그리고 64-bit data types를 지원한다.
* general-purpose registers의 set은 8에서 16으로 확장된다.
* 많은 program state가 stack에서보다는 registers에 보유된다. Integer와 pointer procedure arguments (6개 까지) registers를 통해 전달된다. 어떤 procedures는 스택에 전혀 접근할 필요가 없다.
* Conditional operations은 가능할 때 conditional move instructions을 사용하여 구현되고, 전통적인 branching code보다 더 좋은 성능을 만들어낸다.
* Floating-point operations은 register-oriented instruction set을 사용하여 구현된다. IA32에서 지원되었던 stack기반의 접근법 대신에.



### 3.1 Data Types

그림 1은 x86-64를위한 다른 C data types의 sizes를 보여준다. 이러한것들을 IA32 sizes와 비교하여 (CS:APP Figure 3.1), 우리는 포인터들이 (여기에서 data type char*로 보여지는) 4가 아닌 8 bytes를 요구한다는 것을 알게된다. 원칙적으로, 이것은 memory의 16 *exabytes* (약 18.4 x 10^18 bytes)에 접근할 수 있는 능력을 준다.

이것은 엄청난 양의 메모리 처럼 보이지만, 4gigabytes가 첫번째 32-bit machines이 1970년대 후반에 나왔을 때 놀라운 것처럼 보였다는 것을 명심해라. 실제로, 대부분의 머신들은 full address range를 지원하지 않는다 - AMD와 Intel x86-64 machines의 현재 세대들은 256 terabytes (2^48) bytes of virtual memory를 지원하지만, pointers를 위한 이러한 많은 메모리를 할당하는 것은 장기간 compatibility를 위한 좋은 아이디어이다.

우리는 또한 prefix "long"이 integers를 64-bits로 바꾼다는 것을 본다. 이것은 상당히 더 큰 범위의 값을 허용한다. 반면에, 32-bit unsigned value는 4,294,967,295까지 범위를 가질 수 있고, word size를 64bits로 늘려, 18,446,744,073,709,551,615의 최대 값을 주게된다.

IA32와 관련하여, long prefix는 또한 IA32에 의해 지원되는 (CS:APP Section 2.4.6) floating point double를 80-bit format 를 사용하도록 바꾼다. 이러한 것은 x86-64에 대해 16 bytes의 할당으로 메모리에 저장된다. IA32에 대해서는 12 bytes로 된것과 비교된다. 이것은 메모리 read and write 연산의 성능을 향상시키는데, 이것은 일반적으로 한 번에 8 또는 16 bytes를 가져온다. 12 or 16bytes가 할당되든, 오직 low-order 10 bytes만이 실제 사용된다.



### 3.2 Assembly Code Example

CS:APP의 Section 3.2.3은 function simple의 GCC에 의해 생성된 IA32 assembly code를 보여준다. 아래는 simple과 유사한 simple_l에 대한 C code이다. 그것이 long integers를사용하는 것만 다르다:

```c
long int simple_l(long int *xp, long int y)
{
	long int t = *xp + y;
	*xp = t;
	return t;
}
```

GCC가 command line에서 x86-64 machine에서 작동할 때

```shell
unix> gcc -O2 -S -m32 code.c
```

그것은어떤 IA32 machine과 호환되는 코드를 생성한다:

```assembly
IA32 version of function simple_l.
Arguments in stack locations 8(%ebp) (xp) and 12(%ebp) (y)
1 simple_l:
2 pushl %ebp Save frame pointer
3 movl %esp, %ebp Create new frame pointer
4 movl 8(%ebp), %edx Get xp
5 movl (%edx), %eax Retrieve *xp
6 addl 12(%ebp), %eax Add y to get t (and return value)
7 movl %eax, (%edx) Store t at *xp
8 leave Restore stack and frame pointers
9 ret Return
```

이 코드는 거의 CS:APP에서 보여진 것과 거의 동일하다, 그것이 stack frame을 해제하기위해 `movl %evp, %esp`와 `popl %ebp` sequence 대신에 단일의 `leave` instruction을 사용한다는 점을 제외하고.

우리는 GCC에게 x86-64 code를 생성하라고 할 때

```shell
unix> gcc -O2 -S -m64 code.c
```

대부분의 머신들에서, flag -m64는 요구되지 않는다), 우리는 매우 다른 코드를 얻는다:

```assembly
x86-64 version of function simple_l.
Arguments in registers %rdi (xp) and %rsi (y)

1 simple_l:
2 addq (%rdi), %rsi Add *xp to y to get t
3 movq %rsi, %rax Set t as return value
4 movq %rsi, (%rdi) Store t at *xp
5 ret Return
```

주요한 차이는 다음을 포함한다

* `movl` 과 `addl` instructions 대신에, 우리는 `movq`와 `addq`를 본다. long integers로서 선언된 pointers와 variables은 32 bits (long words)가 아닌 이제 64 bits (quad words)이다.
* 

