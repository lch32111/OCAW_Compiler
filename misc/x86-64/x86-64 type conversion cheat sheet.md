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