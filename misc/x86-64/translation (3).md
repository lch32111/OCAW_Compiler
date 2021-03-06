| Instruction | Effect                                                       | Description                 |
| ----------- | ------------------------------------------------------------ | --------------------------- |
| imulq S     | R[%rdx] : R[%rax] <- S X R[%rax]                             | Signed full multiply        |
| mulq S      | R[%rdx] : R[%rax] <- S X R[%rax]                             | Unsigned full multiply      |
| cltq        | R[%rax] <- SignExtend(R[%eax])                               | Convert `%eax` to quad word |
| cqto        | R[%rdx] : R[%rax] <- SignExtend(R[%rax])                     | Convert to oct word         |
| idivq S     | R[%rdx] <- R[%rdx] : R[%rax] mod S;<br />R[%rax] <- R[%rdx] : R[%rax] / S | Signed divide               |
| divq S      | R[%rdx] <- R[%rdx] : R[%rax] mod S;<br />R[%rax] <- R[%rdx] : R[%rax] / S | Unsigned divide             |

Figure 5 : **Special airthmetic operations** 이 연산들은 signed and unsigned numbers 둘 다에 대해 full 64-bit multiplication과 division을 지원한다. registers `%rdx`와 `%rax`의 쌍은 single 128-bit oct word를 형성하는 것으로서 보여진다.



### 4.1 Special Arithmetic Instructions

그림 5는 두 개의 64-bit words의 full 128-bit product를 생성하기 위해 사용되는 instructions을 보여줄 뿐만 아니라, 64-bit division을 지원하는 것도 보여준다. 그것들은 32-bit 대응되는 것들과 유사하다 (CS:APP Figure 3.9). 몇 가지 이러한 instructions은 registers `%rdx`와 `%rax`의 조합을 128-bit *oct word*를 생성하는 것으로서 본다. 예를들어, 그 `imulq`와 `mulq` instructions은 두 64-bit values의 곱의 결과를  저장하는데, source operand에 의해 주어진 것을 첫 번째로, 그리고 register `%rax`로부터 두 번째에 저장한다.

두 개의 divide instructions `idivq`와 `divq`는 128-bit dividend로서 `%rdx:%rax`로 시작하고, 64-bit divisor로서 source operand와 시작한다. 그것들은 그러고나서 register `%rax`에 몫을 저장하고, register `%rdx`에 나머지를 저장한다. dividend를 준비하는 것은 unsigned (`divq`) 또는 signed `idiviq` division 이 수행될 것인지에 의존한다. 전자의 경우에, register `%rdx`는 간단히 0으로 설정된다. 후자의 경우에, instruction `cqto`가 sign extension을 수행하기 위해 사용되는데, `%rax`의 sign bit를 `%rdx`의 모든 비트로 복사한다. (GAS instruction `cqto`는 Intel과 AMD documentation에서 또한 `cqo`라고 불려진다.)



**Aside 생략**



## 5 Control

x86-64에서 control instructions과 control transfers를 구현하는 방법들은 IA32에 있는 것들과 본질적으로 같다 (CS:APP Section 3.6). 두 개의 새로운 instructions `cmpq`와 `testq`는 quad words를 비교하교 테스트하기 위해 추가되었다. 이것은 byte, word, 그리고 double word sizes를 위한 것들을 복아한다 (CS:APP Section 3.6.1):

| Instruction | Based on | Description        |
| ----------- | -------- | ------------------ |
| cmpq S2, S1 | S1 - S2  | Compare quad words |
| testq       | S1 & S2  | Test quad word     |



### 5.1 Conditional Move Instructions

1995년에 PentiumPro에서 시작하여, IA32 processors의 최근 세대들은 아무것도 하지 않거나, condition codes의 값에 따라 register에 값을 복사하거나 둘 주 ㅇ하나를 하는 *conditional move* insturctions을 가지게 되었다. 이러한 것들은 branching code를 사용하는 것보다 상당히 더 빠를 수 있는 conditional operations을 구현하는 대안의 방법을 제공한다. 몇년 동안, 이러한 instructions들이 크게 사용되어오지 않았다. GCC는 그것들을 사용하는 코드를 생성하지 않았다. 왜냐하면 그것이 backward compatibility를 막기 때문이다. x86-64의 출현은 컴파잉러 작성자들이 이 호환성 요구사항을 포기하게 했고, 그래서 우리는 이제 컴파일된 x86-64 code에서 conditional moves를 찾게 된다. 이 섹션은 conditional moves를 설명하고, 그것들이 어떻게 사용되는지를 설명한다.

우리는 GCC에 의해 생성된 IA32 코드에서, conditional statements (if를 사용하여) 그리고 conditional expressions (? : 를 사용하여)들이 항상 conditional mumps로 구현된 것을 보았다. 거기에서 condition code 값들이 jump가 취해질지 아닐지를 결정한다.

예제로, assignment와 conditional expression의 다음의 일반적인 형태를 고려해라

```
	v = test-expr ? then-expr : eles-expr;
```

그 컴파일러는 다음의 추상적인 코드에 의해 보여지는 형태를 갖는 코드를 생성한다

```
	if (!test-expr)
		goto else;
	v = then-expr;
	goto done;
else:
	v = else-expr;
done:
```

이 코드는 두 개의 code sequences를 포함한다 - *then-expr*를 evaluate하는 것과, *else-expr*를 evaluate하는 것. conditional과 unconditional jumps의 조합은 sequences중의 하나가 evaluated되는 것을 보장하기 위해 사용된다. branch prediction과 mispredication penalties의 토론에서 말했 듯이, modern processors는 그 프로세서들이 높은 확률로 그 테스트의 결과를 예측할 수 있는 경우에서만 효율적으로 위에서 보여진 스타일의 코드를 실행할 수 있다. 그 프로세서는 predicted branch direction을 따르는 deep pipeline을 사용한느데, 선택된 코드 시퀀스를 실행하는데, *speculative execution*이라고 알려진 프로세스이다. 만약 그 test outcome이 정확하게 예측된다면, 그러면 그것은 어떤 speculative results를 *commit*하고, 어떠한 효율성의 손실 없이 진행한다. 만약 그 예측이 정확하지 않다면,  그것은 speculative results를 버려야만 하고, 다른 코드 시퀀스로 실행을 재시작해야 한다. 이것은 상당한 지연을 발생시킬 수 있는데, 아마도 10-40 clock cycles이다. Processors들은 한 주어진 branch에 대해 어떤 규칙적인 패턴이 취해지는지 아닌지를 탐지하는 정교한 branch prediction hardware를 이용한다.

이 비효율성의 극한의 예제로서, 두 값의 최대를 연산하는 다음의 C 함수를 고려해라

```c
int max(int x, int y) 
{ 
    return (x < y) ? y : x; 
}
```

일반적인 어플리케이션에서, `x < y` test의 결과는 매우 예측할 수 없는 것이고, 그래서 가장 정교한 branch prediction hardware는 50% 정도로만 올바르게 추측할 것이다. 게다가, 두 코드 시퀀스의 각각에서 수행되는 연산들은 단일 clock cycle을 요구한다. 결과적으로, branch mispredicition penaly는 이 함수의 성능을 지배한다. 한 Intel Pentium 4 Xeon에서 작동시켜서, 우리는 그 결과가 쉽게 예측가능할 때, 그 함수가 호출마다 약 10 cycles를 요구한다는 것을 발견했지만, 그 outcome이 random할 때 호출 마다 약 31 cycles를 요구한다는 것을 알게되었다. 이것으로 부터, 우리는 branch misprediction penalty가 약 42 clock cycles이라는 것을 추론할 수 있다.



​	**Aside : How did you determine this penalty?**

​	misprediction의 확률이 $p$이고, mispredication없이 코드를 실행하는 시간이 $T_{OK}$이고, 그 misprediction penalty는 `T_{MP}`라고 가정하자. 그러고나서 그 코드를 실행시키는 평균 시간은 $T_{avg} = (1-p)T_{OK} + p(T_{OK} + T_{MP}) = T_{OK} + pT_{MP}$. $p = 0.5$, $T_{OK} = 10$,$T_{avg} = 31$ 에 대해, 우리는 
$T_{MP} = 42$를 얻는다. **End Aside.**



conditional operations의 어떤 형태를 구현하는 것의 대안 방법은 *conditional move* instructions을 사용하는 것이다. 이 접근법으로, *then-expr*과 *else-expr* 둘 다 evaluated 되고, *test-expr*의 evaluation을 기반으로 최종값이 선택된다. 이것은 다음의 추상화 코드로 설명될 수 있다:

```
	vt = then-expr;
	v = else-expr;
	if (test_expr) v = vt;
```

이 시퀀스에서 마지막 문장은 conditional move를 보여준다 - `vt` 값은 만약 그 tested condition이 유효하다면 `v`로 옮겨진다.



| Instruction | Synonym | Move condtion     | Description                  |
| ----------- | ------- | ----------------- | ---------------------------- |
| cmove S, D  | cmovz   | ZF                | Equal / zero                 |
| cmovne S, D | cmovnz  | ~ZF               | Not equal / not zero         |
| cmovs S, D  |         | SF                | Negative                     |
| cmovns S, D |         | ~SF               | Nonnegative                  |
| cmovg S, D  | cmovnle | ~ (SF ^ OF) & ~ZF | Greater (signed >)           |
| cmovge S, D | cmovnl  | ~ (SF ^ OF)       | Greater or equal (signed >=) |
| cmovl S, D  | cmovnge | SF ^ OF           | Less (signed <)              |
| cmovle S, D | cmovng  | (SF ^ OF) \| ZF   | Less or equal (signed <=)    |
| cmova S, D  | cmovnbe | ~ CF & ~ ZF       | Above (unsigned > )          |
| cmovae S, D | cmovnb  | ~CF               | Above or equal (Unsigned >=) |
| cmovb S, D  | cmovnae | CF                | Below (unsigned <)           |
| cmovbe S, D | cmovna  | CF \| ZF          | below or equal (unsigned <=) |

Figure 6 : **`cmov` instructions**. 이 instructions은 move condition이 유효할 때 source value S를 그것의 destinaion D로 복사한다. 어떤 instructions은 "synonyms"을 갖는데, 같은 machin instruction에 대해 대안의 이름을 가진다.



그림 6은 PentiumPro microprocessors의 도입과 함께 IA 32 instruction set에 추가된 conditional move instructions을 보여준다. 이러한 instructions의 각각은 두 개의 operands를 갖는다: source register 또는 memory location S, 그리고 destinaion register D. 다른 *set*과 *jump* selection에서 처럼, 이러한 instructions의 결과는 conditino codes의 결과에 의존한다.  그 source value는 memory 또는 source register에서 읽혀지지만, 명시된 조건이 유효한 경우메나 destination에 복사된다.

source and destination values는 16, 32, or 64 bits long이 될 수 있다. Single byte conditional moves는 지원되지 않는다. operand length가 instruction name에 explicitly하게 인코딩되는 (예를들어, `movw`, `movl`, `movq`) unconditional instructions과 다르게,  conditional move operand lengths는 destination registers의 이름으로 유추될 수 있고, 그래서 그 같은 instruction name은 모든 operand lengths에 대해 사용될 수 있다.

예제로서, GCC는 function `max`에 대해 다음의 코드를 생성한다

```assembly
	x86-64 code generated for function max
	x in register %edi, y in %esi 
1 max: 
2 	cmpl %esi, %edi Compare x:y 
3 	cmovge %edi, %esi if >=, then y=x 
4 	movl %esi, %eax set y as return value 
5 	ret
```

이 코드는 `x >= y`일 때, x로 y를 포함하는 register에 덮어쓰기 위해 conditional move를 사용한다.

그 conditional move는 test의 outcome을 예측할 필요없이 구현될 수 있다. 그 processor는 간단히 그 코드를 evaluate하고, 그러고나서 destination register를 업데이하거나, 그것을 똑같이 유지한다. 같은 Intel Pentium 4 Xeon에서 그 코드를 동작시켜서, 우리는 테스트의 결과와 상관없이 호출당 약 10 cycles 요구하는 것을 알게 된다.



**Practice Problem 3 :**

다음의 C 함수에서, `OP`는 `#define`으로 미리 선언된 어떤 C 연산자이다.

```c
int arith(int x)
{
    return x OP 4;
}
```

컴파일 될 떄, GCC는 다음의 x86-64 코드를 생성한다:

```ㅁ느
	x86-64 implementation of arith
	x in register %edi
1 arith:
2	leal	3(%rdi), %eax
3	cmpl	$-1, %edi
4 	cmovle	%eax, %edi
5	sarl	$2,	%edi
6	movl	%edi,	%eax	return value in %eax
7 	ret
```

> 난 못풀겠어서 바로 답을 본다. B가 주석이 있는 버전이라 그것부터 적는다. 이 문서가 이전것을 기반으로 해서 그런지 내가 아직 잘 이해가 되지 않는 문제들이 많다.

B.  그 `comvle` instruction은 조건부로, x <= -1일 때, x를 x + 3로 숫자를 바꾼다.

```
	x86-64 implementation of arith
	x in register %edi
1 arith:
2	leal	3(%rdi),	%eax 	x+3 -> return value(%eax) = x + 3
3	cmpl	$-1,	%edi		compare x:-1 -> 
4	cmovle	%eax,	%edi		if <= then x = x + 3
5	sarl	$2,		%edi		x >>= 2
6	movl	%edi,	%eax		return value in %eax
7	ret
```

A. operator는 '/'이다. 우리는 이것이 right shifting하여 (see CS:APP Section 2.3.7) 2의 지수로 나누는 예제라는 것을 안다. k = 2로 shifting하기 전에, 우리는 dividend가 음수일 때 2^k - 1 = 3의 bias를 더한다.



모든 conditional expressions이 conditional moves를 사용하여 컴파일될 수 없다. 좀 더 중요하게, 이전에 보여진 abstract code는 test outcome의 *then-expr*과 *else-expr* 둘 다 검사한다. 만약 그러한 두 expressions들 중 하나가 error condition 또는 side effect를 발생시킬 수 있다면, 이것은 유효하지 않은 행동을 이끌 수 있다. 예시처럼, 다음의 C 함수를 고려해라

```c
int cread(int* xp)
{
    return (xp ? *xp : 0);
}
```

처음에, 이것은 pointer xp에 의해 지정된 값을 읽기위해 conditional move를 사용하여 컴파일하는 좋은 후보가 되는 것처럼 보인다. 다음의 assembly code에서 보이듯이:

```asm
	Invalid implementation of function cread
	xp in register in %rdi
1 cread:
2	xorl	%eax,	%eax	Set 0 as return value
3	testq	%rdi,	%rdi	Test xp
4	cmovne	(%rdi),	%eax,	if !0, dereference xp to get return value
5	ret
```

이 구현은 그러나 유효하지 않다. `cmovne` instruction (line 4)에 의한 `xp`의 dereference는 그 테스트가 실패할 때 발생하는데, null pointer dereferencing error를 발생시킨다. 대신에, 이 코드는 branching code를 사용하여 컴파일 되어야 한다.

유사한 케이스는 두 개의 branches중 하나가 side effect를 발생시킬 때에도 유효하다. 다음의 함수에서 보여진다

```c
int lcount = 0;
int se_max(int x, int y)
{
    return (x < y) ? (lcount++, y) : x;
}
```

이 함수는 global 변수 lcount를 *then-expr*의 일부로서 증가시킨다. 따라서, branching code는 이 side effect가 test condition이 유효할때만 발생하도록 사용되어야 한다.

conditional moves를 사용하는 것은 또한 항상 code 효율성을 개선하지 않는다. 예를들어, 만약 *then-expr*또는 *else-expr* evaluation 둘 중 하나가 상당한 연산을 요구한다면, 그러면 이 노력은 대응되는 조건이 유효하지 않을 때 낭비가 된다. GCC로 한 우리의 실험은, 그것이 오직 두 개의 expressions이 각각 단일 instruction으로 연산될 수 있을 때 conditional moves만을 사용하는 것을 가리킨다. 이것은 현재의 프로세서들에게 너무 보수적이다. 그것들의 high branch misprediction penalties를 고려한다면.



### 5.2 Looping Constructs

CS:APP Section 3.6.5에서, 우리는 C의 세 가지 looping constructs를 발견했다: `do-while`, `while`, 그리고 `for`. 이것들은 모두 `do-while`의 구조를 기반으로 공통 템플릿을 사용하여 IA32를 위해 컴파일 된다. 다른 loop 형태들은 CS:APP Figure 3.14에서 보여지듯이, `do-while`로 변형되었다.

x86-64로, 우리는 더욱 풍부한 loop templates를 보게된다. `do-while`를 위한 template은 같게 남아있고, 몇몇 `while`과 `for` statements는 이 형태로 구현된다. 예를들어,....

























