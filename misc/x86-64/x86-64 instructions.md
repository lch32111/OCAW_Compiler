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