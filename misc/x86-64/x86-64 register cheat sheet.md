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
* backward compatibility를 위해, `%rax, %rcx, %rdx, %rbx` 레지스터들의 second byte는 single-byte operands를 가진 instructions으로 직접 접근될 수 있다