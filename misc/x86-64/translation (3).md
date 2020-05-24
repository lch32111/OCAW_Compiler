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



conditional operations의 어떤 형태를 구현하는 것의 대안 방법은 *conditional move* instructions을 사용하는 것이다.

























