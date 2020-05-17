## 3 An Overview of x86-64

Intel과 AMD에 의해 공급되는 새로운 하드웨어와 이러한 머신들을 타겟으로 하는 GCC의 새로운 버전의 조합은 IA32 machines을 위해 생성된 것과  x86-64 코드를 상당히 다르게 만든다.

| C declaration | Intel data type    | GAS suffix | x86-64 Size (Bytes) |
| ------------- | ------------------ | ---------- | ------------------- |
| char          | Byte               | b          | 1                   |
| short         | Word               | w          | 2                   |
| int           | Double word        | l          | 4                   |
| unsigned      | Double word        | l          | 4                   |
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

이 코드는 거의 CS:APP에서 보여진 것과 거의 동일하다, 그것이 stack frame을 해제하기위해 `movl %ebp, %esp`와 `popl %ebp` sequence 대신에 단일의 `leave` instruction을 사용한다는 점을 제외하고.

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
* 우리는 예를들어 `%rsi`. `%rdi`같은 resgisters들의 64-bit versions을 본다. 그 procedure는 `%rax` register에 한 값을 저장하여 반환한다.
* 어떠한 stack frame이 x86-64 version에서 생성되지 않는다. 이것은 IA32 code에서 stack frame을 설정하고 (lines 2-3), 제거하는 (line 8) 명령어들을 제거한다.
* arguments인 `xp`와 `y`는 stack이 아닌 registers `%rdi`와 `%rsi`에서 넘겨진다. 이러한 registers들은 `%edi`와 `%esi` registers들의 64-bit versions이다. 이것은 memory에서 arguments를 가져올 필요성을 제거한다. 결과로, lines 2와 3에 있는 두 개의 instructions은 `*xp`를 가져올 수 있고,  그것을 y에더하고, 그것을 return value로 설정한다. 반면에 IA32 코드는 code의 세 라인을 요구했다 : 4 - 6

이러한 변화의 순수 효과는 IA32 코드가 7개의 메모리 참조를 하는데 8개의 instructions으로 구성되어 있는 반면에, x86-64 코드는 3개의 메모리 참조를 하는데 4개의 명령어로 구성된다는 점이다. Intel Pentium 4 Xeon에서 작동시켜, 우리의 실험은 IA32 코드가 호출당 17 clock cycles를 요구하는 반면, x86-64 코드는 호출당 12 cycles를 요구한다는 것을 보여주었다. AMD Opteron에서 작동시켜서, 우리는 호출 당 9와 7 cycles를 각각 얻는다. 같은 C 코드를 가진 같은 머신에서 1.3 ~ 1.4배의 성능 증가를 얻는 것은 엄청난 성취이다. 명백히 x86-64는 중요한 진보를 나타낸다.



### 4 Accessing Information

그림 2는 x86-64에서 general-purpose registers의 셋을 보여준다. IA32를 위한 registers들과 비교하여 (CS:APP Figure 3.2), 우리는 많은 차이를 보게 된다:

* registers의 개수는 두 배가 되어 16이 되었다. 새로운 레지스터들은 8-15로 숫자를 부여받는다.
* 모든 registers는 64bits 길이 이다. IA32 registers의 64-bit extensions은 `%rax, %rcx, %rdx, %rbx, %rsi, %rdi, %rsp, %rbp`로 이름 붙여지어진다. 새로운 registers들은 `%r8-%r15`로 이름 붙여진다.
* 각 register의 low-order 32 bits는직접적으로 접근되어질 수 있다. 이것은 우리에게 IA32의 친숙한 registers들을 준다 : `%eax, %ecx, %edx, %ebx, %esi, %edi, %esp, %ebp`, 뿐만 아니라 8개의 새로운 32-bit registers들도 : `%r8d-%r15d`.
* 각 register의 low-order 16 bits는 직접적으로 접근되어질 수 있고, IA32에도 해당된다. 새로운 레지스터들의 *word-size* versions은 `%r8w-%r15w`로 이름붙여진다.

| 63 ~ 32 | 31 ~ 16      | 15 ~ 8 | 7 ~ 0 | 구분             |
| ------- | ------------ | ------ | ----- | ---------------- |
| %rax    | %eax, %ax    | %ah    | %al   | Return value     |
| %rbx    | %ebx, %ax    | %bh    | %bl   | Callee saved     |
| %rcx    | %ecx, %cx    | %ch    | %cl   | 4th argument     |
| %rdx    | %edx, %dx    | %dh    | %dl   | 3rd argument     |
| %rsi    | %esi, %si    |        | %sil  | 2nd argument     |
| %rdi    | %edi, %di    |        | %dil  | 1st argument     |
| %rbp    | %ebp, %bp    |        | %bpl  | Callee saved     |
| %rsp    | %esp, %sp    |        | %spl  | Stack pointer    |
| %r8     | %r8d, %r8w   |        | %r8b  | 5th argument     |
| %r9     | %r9d, %r9w   |        | %r9b  | 6th argument     |
| %r10    | %r10d, %r10w |        | %r10b | Callee saved     |
| %r11    | %r11d, %r11w |        | %r11b | Used for linking |
| %r12    | %r12d, %r12w |        | %r12b | Unused for C     |
| %r13    | %r13d, %r13w |        | %r13b | Callee saved     |
| %r14    | %r14d, %r14w |        | %r14b | Callee saved     |
| %r15    | %r15d, %r15w |        | %r15b | Callee saved     |

그림 2 : **Integer registers.** 존재하는 8개의 registers들은 64-bit versions으로 확장된다. 그리고 8개의 새로운 registers들이 추가되었다. 각 레지스터는 8 bits (byte), 16 bits (word), 32 bits (double word), or 64 bits (quad word)로서 접근 되어질 수 있다.

* 각 레지스터의 low-order 8 bits는 직접적으로 접근딜 수 있다. 이것은 처음 4개의 registers들인 (`%al, %cl, %dl, %bl`)에 대해 IA32에도 해당된다.  다른 IA32 registers들의 byte-size versions들은 `%sil, dil, %spl, %bpl`로 이름지어진다. 새로운 registers들의 byte-size versions은 `%r8b-%r15b`라고 이름지어진다.
* backward compatibility를 위해, `%rax, %rcx, %rdx, %rbx` 레지스터들의 second byte는 single-byte operands를 가진 instructions으로 직접 접근될 수 있다.

IA32 처럼, 대부분의 레지스터들은 상호 교환하여 사용될 수 있찌만, 몇 가지 특별한 케이스들이 있다. `%rsp`는 특별한 상태를 가지는데, 그것이 top stack element에 대한 포인터를 보유한다는 점이다. 그러나, IA32에서와 다르게, 어떠한 frame pointer register가 없다. 즉 register `%rbp`가 general-purpose register로서 사용 가능하다. 특별한 conventions은 registers를 통해 procedure arguments를 넘기기 위해 사용되는 것이고, registers들이 procedure calls 동안 어떻게 저장되어지고, restored되어지는지를 위해 사용된다는 것이다. Section 6에서 이야기 되듯이. 게다가, 몇 가지 arithmetic instructions은 registers `%rax`와 `%rdx`들을 특별히 이용한다.

대부분, x86-64의 operand specifiers는 IA32에 있는 것들과 같다 (CS:APP Figure 3.3를 보아라). 한 가지 사소한 차이는, *PC 관련* operand addressing의 몇 가지 형태들이 지원된다는 것이다. IA32과 함께, 이 addressing의 형태는 jump와 다른 control transfer instructions을 위해서만 지원된다 (CS:APP Section 3.6.3)

. 이 모드는 offsets (CS:APP Figure 3.3 as *Imm*)이 오직 32 bits 길이인 사실에 대해 보상하기 위해 제공되어 진다. 이 field를 32-bit로 보아서,  two's complement number인 instructions은 program counter를 기준으로 약 +-2.15 x 10^9의 window내에서 데이터를 접근할 수 있다. x86-64와 함꼐, 그 프로그램 카운터는 `%rip`로 이름지어진다.

PC 관련 data addressing의 예쩨로서, 다음의 procedure를 고려해라. 이것은 이전에 검사된 `call_simple_l` 함수를 호출한다:

```c
long int gval1 = 567;
long int gval2 = 763;
long int call_simple_l()
{
	long int z = simple_l(&gval1, 12L);
	return z + gval2;
}
```

이 코드는 global variables `gval1`과 `gval2`를 참조한다. 이 함수가 compiled, assembled, 그리고 linked되어질 때, 우리는 다음의 executable code를 얻는다 (disassembler `objdump`에 의해 생성된):

```assembly
1 0000000000400500 <call_simple_l>:
2 400500: be 0c 00 00 00 mov $0xc,%esi Load 12 as 1st argument
3 400505: bf 08 12 50 00 mov $0x501208,%edi Load &gval1 as 2nd argument
4 40050a: e8 b1 ff ff ff callq 4004c0 <simple_l> Call simple_l
5 40050f: 48 03 05 ea 0c 10 00 add 1051882(%rip),%rax Add gval2 to result
6 400516: c3 retq
```

line 3에 있는 instruction은 global variable `gval1`의 주소를 `%rdi` 레지스터에 저장한다. 이것은 간단히 constant value `0x501208`를 `%edi` 레지스터에 복사하여 하는 것이다. `%rdi`의 상위 32bits는 그러고나서 자동으로 0으로 설정된다. line 5에 있는 instruction은 `gval2`의 값을 가져오고, 그것을 `simple_l` 호출에 의해 반환된 값에 더한다.

| Instruction  | Effect                                       | Description                    |
| ------------ | -------------------------------------------- | ------------------------------ |
| movq S, D    | D <- S                                       | Move quad word                 |
| movabsq I, R | R <- I                                       | Move quad word                 |
| movslq S, R  | R <- SignExtend(S)                           | Move sign-extended double word |
| movsbq S, R  | R <- SignExtend(S)                           | Move sign-extended byte        |
| movzbq S, R  | R <- ZeroExtend(S)                           | Move zero-extended byte        |
| pushq S      | R[%rsp] <- R[%rsp] - 8;<br />M[R[%rsp]] <- S | Push                           |
| popq D       | D <- M[R[%rsp]];<br />R[%rsp] <- R[%rsp] + 8 | Pop                            |

Figure 3 : **Data movement instructions.** 이러한 것들은 IA32의 movement instructions을 보충한다. 그 `movabsq` instruction은 source value로서 (I라고 보여지는) immediate value만 허용한다. 다른 것들은 immediate data, a register, or memory (S라고 보여지는) 것들을 허용한다. 어떤 instructions은 destination이 register가 될 것을 요구한다 (R로 보여지는), 반면에 다른 것은 register와 memory destinations둘 다 허용한다 (D라고 보여지는).



여기에서, 우리는 PC와 관련한 addressing을 본다 - immediate value 1051882 (hexadecimal 0x100cea)가 `0x100cea + 0x400516 = 0x501200`를 얻기 위해 다음의 instruction의 주소에 더해진다.

그림 3은 x86-64로 이용가능한 data movement instructions 몇 가지를 문서화한다. IA32에서 보여지는 것들 너머에 있는 (CS:APP Figure 3.4 참조).  몇 가지 instructions은 그 destination이 register가 되기를 요구한다. 이것은 R로 가리켜진다. 그 보여진 instructions은 data를 64-bit register 또는 memory destination으로 옮기기 위해 다른 variations의 `mov` instruction을 포함한다. immediate data를 64-bit register로 옮기는 것은 32-bit immediate value를 sign extend할 `movq` 또는 full 64-bit immediate가 요구될 때 `movabsq` instruction 둘 중 하나로 처리될 수 있다.

더 작은 data size에서 64 bits로 옮기는 것은 sign extension (`movsbq`, `movslq`) 또는 zero extension (`movzbq`) 둘 중 하나를포함할 수 있따. 아마도 예상치 못하게, 32-bit register value를 옮기거나 생성하는 instructions은 또한 register의 upper 32 bits를 0으로 설정할 것이다. 결과적으로, instruction `movzlq`에 대한 필요성이 없다. 비슷하게, instruction `movzbq`는 그 destination이 register일 때, `movzbl`과 정확히 같은 행동을 갖는다 - 둘 다 destination register의 upper 56bits를 zero로 설정한다. 이것은 `movb`같은 8또는 16-bit values를 생성하는 instructions과 대조적이다; 이러한 instructions은 register에 있는 다른 비트들을 바꾸지 않는다. 그 새로운 stack instructions `pushq`와 `popq`는 64-bit values의 pushing과 popping을 허용한다.



### Practice Problem 1:

다음의 C 함수는 type `src_t` 인자를 `dst_t`의 type인 return value로 변환한다. 여기에서 이러한 두 개의 types들은 `typedef`를 사용하여 정의된다:

```c
dest_t cvt(src_t x)
{
    return (dest_t) x;
}
```

argument x가 register `%rdi`의 적절히 이름지어진 부분에 있다고 가정하자 (즉, `%rdi, %edi, %di, %dil`). 그리고 move instruction의 어떤 형태가 type conversion을 수행하기 위해 사용되어지고, 그 값을 register `%rax`의 적절히 이름지어진 부분에 값을 복사하기 위해 사용된다고 하자. instrucction, source register, 그리고 destination register를 가리키는 다음의 테이블을 채워라. 다음의 source와 destination type의 조합을 위해:

| Tx            | Ty            | Instruction | S    | D    | Explanation             |
| ------------- | ------------- | ----------- | ---- | ---- | ----------------------- |
| long          | long          | movq        | %rdi | %rax | No conversion           |
| int           | long          | movslq      | %edi | %rax | Sign extend             |
| char          | long          | movsbq      | %dil | %rax | Sign Extend             |
| unsigned int  | unsigned long | movl        | %edi | %eax | Zero extends to 64 bits |
| unsigned char | unsigned long | movzbq      | %dil | %rax | z                       |
| unsigned char | unsigned long | movzbl      | %dil | %eax | Zero extends to 64 bits |
| long          | int           | movslq      | %edi | %rax | Sign extends to 64 bits |
| long          | int           | movl        | %edi | %eax | Zero extends to 64 bits |
| unsigned long | unsigned      | movl        | %edi | %eax | Zero extends to 64 bits |



| Instruction                                                  | Effect                                                       | Description                                                  |
| ------------------------------------------------------------ | ------------------------------------------------------------ | ------------------------------------------------------------ |
| leqa S, D                                                    | D <- &S                                                      | Load effective address                                       |
| incq D<br />decq D<br />negq D<br />notq D                   | D <- D + 1<br />D <- D - 1<br />D <- -D<br />D <- ~D         | Increment<br />Decrement<br />Negate<br />Complement         |
| addq S, D<br />subq S, D<br />imulq S, D<br />xorq S, D<br />orq S, D<br />andq S, D | D <- D + S<br />D <- D - S<br />D <- D * S<br />D <- D ^ S<br />D <- D \| S<br />D <- D & S | Add<br />Subtract<br />Multiply<br />Exclusive-or<br />Or<br />And |
| salq k, D<br />shlq k, D<br />sarq k, D<br />shrq k, D       | D <- D << k<br />D <- D << k<br />D <- D >> k<br />D <- D >> k | Left shift<br />Left shift (same as salq)<br />Arithmetic right shift<br />Logical right shift |

Figure 4: **Integer 64-bitarithmetic operations.** 그것들은 32-bit counterparts와 닮았다 (CS:APP Figure 3.7).

그림 4에서 보여지듯이, 64-bit data에 대한 arithmetic and logical instructions은 32-bi counterparts와 닮았다. 예를들어, 우리는 `addl` 외에 `addq` instruction과 `leal` 대신에, `leaq` instruction을 본다. 이전에 언급했듯이, `addl` 같이 32-bit register results를 생성하는 instrutions 또한 register의 high-order bits를 0으로 설정한다.

32와 64-bit data를 섞을 때, GCC는 arithmetic instructions, sign extensions, 그리고 zero extensions의 올바른 선택을 해야만 한다. 이러한 것은 type conversion의 미묘한 측면과 32와 64-bit instructions 사이의 상호작용에 의존한다. 이것은 다음의 C 함수에서 보여진다:

```c
1 long int gfun(int x, int y)
2 {
3 	long int t1 = (long) x + y; /* 64-bit addition */
4 	long int t2 = (long) (x + y); /* 32-bit addition */
5 	return t1 | t2;
6 }
```

integers가 32bits이고, long integers가 64bits라는 것을 가정하여, 이 함쉥서 두  덧셈은 다음과 같이 진행된다. type conversion이 덧셈보다 더 높은 precedence를 가진다는 것을 회상해라. 그래서 line 3은 x가 64 bits로 변환되는 것을 요구하고, operand promotion에 의해, y 또한 변환된다. 그 Value `t1`은 그러고나서 64-bit addition으로 연산된다. 반면에, `t2`는 32-bit addition을 수행하고, 그러고나서 이 값을 64-bits로 확장하고나서 line 4에서 연산된다.

이 함수를 위해 생성된 assembly code는 다음과 같다

```assembly
x86-64 version of function gfun
Arguments in registers %rdi (x) and %rsi (y) 
1 gfun: 
2 	movslq 	%edi,%rax Convert x to long 
3 	movslq 	%esi,%rdx Convert y to long 
4 	addl 	%esi, %edi lower bits of t2 (32-bit addition) 
5 	addq 	%rdx, %rax t1 (64-bit addition) 
6 	movslq 	%edi,%rdi Sign extend to get t2 
7 	orq 	%rdi, %rax Return t1 | t2 
8 	ret
```

Local value `t1`은 처음에 arguments를 확장하여 연산된다. lines 2-3에 있는 `movslq` instructions은 registers `%rdi`와 `%rsi`에 있는 arguments의 lower 32 bits를 취하고, registers `%rax`, `%rdx`에 있는 64 bits로 sign extend한다. line 5에 있는 `addq` instruction은 그러고나서 `t1`를 얻기 위해 64-bit addition을 수행한다. `t2`의 값은 두 operands의 lower 32 bits에 32-bit addtion을 수행하여 연산된다 (line 4). 이 값은 line 6에서 64 bits로 sign extended 된다.



### Practice Problem 2:

인자들로 `a`, `b`, `c`, 그리고 `d`를 가지는 `arithprob`인  C 함수는 다음의 body를 갖는다:

```c
	return a * b + c * d;
```

이것은 다음의 x86-64 code로 컴파일된다:

```assembly
Arguments: a in %edi, b in %sil, c in %rdx, d in %ecx 
1 arithprob: 
2 movslq 	%ecx,%rcx 
3 movsbl 	%sil,%esi 
4 imulq 	%rdx, %rcx 
5 imull 	%edi, %esi 
6 leal 		(%rsi,%rcx), %eax 
7 ret
```

그 arguments들과 return value 모두 다양한 길이를 가진 signed integers이다. 이 assembly code를 기반으로, `arithprobm`의 return과 argument types를 묘사하는 function prototype을 작성해라.



우리는 `arithprob`의 코드를 한 단계씩 나아갈 수 있고, 다음의 것을 결정할 수 있다:

1. 그 `movslq` instruction은 `d`를 long으로 sign extends한다. 이것은 `d`가 `int` type을 가지고 있고, `c`가 `long` type을 가진다는 것을 암시한다.
2. 그 `movsbl` instruction은 `b`를 integer로 sign extend한다. 이것은 `b`가 `char` type을 가지고 있고, `a`가 `int` type을 가진다는 것을 의미한다.
3. 그 합은 `leal` instruction으로 연산되는데, 그 return value가 `int` type을 가진다는 것을 가리킨다.

이것으로 부터, 우리는 `arithprob`에 대한 unique prototype이 다음이라는 것을 결정할 수 있다:

```c
int arithprob(int a, char b, long c, int d);
```

