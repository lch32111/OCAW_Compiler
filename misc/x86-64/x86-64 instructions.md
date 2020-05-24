| Instruction  | Effect                                       | Description                    |
| ------------ | -------------------------------------------- | ------------------------------ |
| movq S, D    | D <- S                                       | Move quad word                 |
| movabsq I, R | R <- I                                       | Move quad word                 |
| movslq S, R  | R <- SignExtend(S)                           | Move sign-extended double word |
| movsbq S, R  | R <- SignExtend(S)                           | Move sign-extended byte        |
| movzbq S, R  | R <- ZeroExtend(S)                           | Move zero-extended byte        |
| pushq S      | R[%rsp] <- R[%rsp] - 8;<br />M[R[%rsp]] <- S | Push                           |
| popq D       | D <- M[R[%rsp]];<br />R[%rsp] <- R[%rsp] + 8 | Pop                            |



| Instruction                                                  | Effect                                                       | Description                                                  |
| ------------------------------------------------------------ | ------------------------------------------------------------ | ------------------------------------------------------------ |
| leqa S, D                                                    | D <- &S                                                      | Load effective address                                       |
| incq D<br />decq D<br />negq D<br />notq D                   | D <- D + 1<br />D <- D - 1<br />D <- -D<br />D <- ~D         | Increment<br />Decrement<br />Negate<br />Complement         |
| addq S, D<br />subq S, D<br />imulq S, D<br />xorq S, D<br />orq S, D<br />andq S, D | D <- D + S<br />D <- D - S<br />D <- D * S<br />D <- D ^ S<br />D <- D \| S<br />D <- D & S | Add<br />Subtract<br />Multiply<br />Exclusive-or<br />Or<br />And |
| salq k, D<br />shlq k, D<br />sarq k, D<br />shrq k, D       | D <- D << k<br />D <- D << k<br />D <- D >> k<br />D <- D >> k | Left shift<br />Left shift (same as salq)<br />Arithmetic right shift<br />Logical right shift |



| Instruction | Effect                                                       | Description                 |
| ----------- | ------------------------------------------------------------ | --------------------------- |
| imulq S     | R[%rdx] : R[%rax] <- S X R[%rax]                             | Signed full multiply        |
| mulq S      | R[%rdx] : R[%rax] <- S X R[%rax]                             | Unsigned full multiply      |
| cltq        | R[%rax] <- SignExtend(R[%eax])                               | Convert `%eax` to quad word |
| cqto        | R[%rdx] : R[%rax] <- SignExtend(R[%rax])                     | Convert to oct word         |
| idivq S     | R[%rdx] <- R[%rdx] : R[%rax] mod S;<br />R[%rax] <- R[%rdx] : R[%rax] / S | Signed divide               |
| divq S      | R[%rdx] <- R[%rdx] : R[%rax] mod S;<br />R[%rax] <- R[%rdx] : R[%rax] / S | Unsigned divide             |



| Instruction | Based on | Description        |
| ----------- | -------- | ------------------ |
| cmpq S2, S1 | S1 - S2  | Compare quad words |
| testq       | S1 & S2  | Test quad word     |



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

