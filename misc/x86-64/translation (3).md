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

