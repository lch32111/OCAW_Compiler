# x86-64 Machine-Level Programming

Randal E. Bryant

David R. O'Hallaron

September 9, 2005



Intel의 IA32 instruction set architecture (ISA), 구어체로 "x86" 이라고 알려진, 이것은 세계의 컴퓨터들을 위한 주요 instruction format이다. IA32는 대부분의 Windows와 Linux machines을 위한 선택된 플랫폼이다. 그 오늘 날 우리가 사용하는 ISA는 i386 microprocessor의 도입과 함께 1985년에 정의되었고, 이것은 원래의 8086에 정의되었던 16-bit instructino set을 32bits로 확장했다. 비록 이후의 processor generations이 새로운 instruction types과 formats을 도입했을 지라도, GCC를 포함한 많은 컴파일러들은 backward compatibility를 유지하는 것에 흥미가 있어 이러한 기능들을 사용하는 것을 피했다.

shift는 Intel instruction set의 64-bit version으로 움직이는중이다. Advanced Micro Devices(AMD)에 의해 원래 개발되었고, *x86-64*라고 이름지어진, 이것은 이제 AMD의 high end processor와 Intel에 의해 지원된다. Intel 것은 *EM64T*를 가리킨다. 대부분의 사람들은 여전히 그것을 "x86-64"라고 가리키고, 우리는 이 컨벤션을 따른다. Linux와 GCC의 더 새로운 버전들은 이 extension을 지원한다. 이 switch를 만들 때, GCC의 개발자들은 IA32 processors의 좀 더 최근의 세대들에서 추가된 몇 가지 instruction-set features를 이용할 기회를 보았었다.

이새로운 하드웨어와 개정된 컴파일러의 조합은 x86-64 code를 상당히 IA32 code와는 다른 형태와 성능으로 만들었다. 64-bit extension을 생성할 때, AMD engineers는 또한 reduced-instruction set computers (RISC)에 있는 features 몇 가지를 채택했다. 이것은 그것들이 컴파일러들을최적화하기 위해 선호되는 타겟들로 만들었다. 예를들어, 이제 16개의 일반 목적의 registers들이 있다. 원래 8086의 8개의 성능을 제한하는 것이 아닌. GCC의 개발자들은 상당한 성능 향상을 얻기 위해, 이러한 기능들을 이용할 수 있었을 뿐만 아니라, IA32 architecture의 좀 더 최근 세대들의 기능들을 이용할 수 있었다. 예를들어, procedure parameters는 이제 stack 대신에 registers를 통해 전달되어지는데, 크게 memory read and write operations의 개수를 줄인다.

이 문서는 Chapter 3 of Computer Systems: A Programmer’s Perspective (CS:APP)의 보완으로서의 역할을 하고, 몇 가지 차이점을 설명한다. 우리는 AMD와 Intel이 어떻게 x86-64에 도달했는지에 대한 간략한 역사로 시작하고, 그이후에, IA32 code와 x86-64 code를구분짓는 주된 기능을 요약하고, 개별 기능에 대해 우리의 방식대로 작업한다.



## 1 History and Motivation for x86-64

i386의 도입된지 20년 후에, microprocessors의 능력은 극적으로 바뀌었다. 1985년에, 완전히 설정되고, high-end personal computer는 약 1 megabyte의 random access memory (RAM) 을 얻었고, 50 megabytes의 disk storage를 가졌다. Microprocessor-based "workstation" systems들은 computing과 engineering 전문가들을 위한 선택된 기계가 되어가고 있었다. 일반적인 micro processor는 5-megahertz clock과 초당 100만 명령어를 작동시켰다. 요즘에, 일반적인 high-end system은 1GB RAM, 500GB disk storage, 그리고 4-gigahertz clock과 초당 50억개의 명령어를 작동시킨다. Microprocessor 기반의 시스템들이 보편적이게 되었다. 심지어 오늘날의 supercomputers들은 많은 microprocessors의 연산력을 병렬로 이용하는 것을 기반으로 한다. 이러한 큰 양적 개선이 주어진다면, 세계의 computing base가 대게 20년에 존재했던 기계와 binary compatible한 code를 작동시킨다.

IA32의 32-bit word size는 microprocessors의 capacity를 증가시키는데 있어서 중요한 한계였다. 가장 중요하게도, 한 machine의 word size는 프로그램들이 사용할 수 있는 virtual addresses의 범위를 정의하고, 32 bits의 경우에, 4-gigabyte virtual address space를 준다. 이것은 한 machine을 위해 이것 보다 많은 RAM을 사는 것이 지금은 그럴 듯 하지만, 그 시스템은 그것을 효과적으로 이용할 수 없다. scientific computing, databases, 그리고 data mining 같은 큰 데이터 셋을 운용하는 것을 포함하는 어플리케이션에 대해,  32-bit word size는 프로그래머들에게 삶을 어렵게 만든다. 그들은 *out-of-core* algorithms을 사용하여 코드를 작성해야만 하고, 거기에서 그 데이터는 disk에거주하고, explicitly하게 처리를 위해 memroy로 read 된다.

computing technology에서의 이후의 진보는 더 큰 word size로의 변환을 요구한다. word sizes를 두 배로 키우는 전통을 따라서, 다음 논리적 단계는 64bits이다. 사실, 64-bit machines는 가끔 이용가능해 왔다. Digital Equipment Corporation은 1992년에 그것의 Alpha processor를 도입했고, 그것은 high-end computing을 위해 인기있는 선택이 되었다. Sun Microsystems는 1995년에 그것의 SPARC architecture의 64-bit version를 도입했다. 그러나, 그 당시에, Intel은 high-end computers를 위한 심각한 contender가 아니였고, 그래서 그 회사는 64 bits로 바꾸는 것에 대한 약한 압력에 있었다.

Intel의 64-bits computers로의 첫 번째 시도는 Itanium processors 인데, 이것은 IA64 instruction set를 기반으로 한다. Intel이 각 새로운 generation을도입할 때 backward compatibility를 유지하려는 Intel의 역사적인 전략과 다르게, IA64는 Hewlett-Packard와 함께 공동으로 개발된 급진적인 새로운 접근법을 기반으로 했다. 그것의 *Very Large Instruction Word* (VLIW) format은 여러 instructions을 bundles로 pack하고, 병렬 실행의 더 높은 정도를 허용한다. IA64를 구현하는 것은 매우 어렵다고 입증되었고, 그래서 그 첫 번째 Itanium chips은 2001이 되어서야 나타나고, 이러한 것들은 실제 어플리케이션에서 기대된 수준의 성능을 얻지 못했다. Itanium 기반 시스템의 성능이 개선되었을지라도, 그것들은 컴퓨터 시장의 많은 몫을 차지하지 않았다. Itanium machines은 compatibility mode에서 IA32 code를 실행할 수 있지만, 매우 좋은 성능이 아니였다. 대부분의 유저들은 덜 비싸고, 종종 더 빠른 IA32 기반의 시스템들을 선호해왔다.

반면에, Intel의 최대 rival인 AMD는 IA64로 Intel의 misstep을 이용할 기회를 보았다. 몇년 동안, AMD는 기술에서 Intel 뒤에 지체되었고, 그래서 그들은 가격을 기반으로 Intel과 경쟁하려고 해왔다. 일반적으로, Intel은 프리미엄 가격으로 새로운 microprocessor를 소개하곤 했다. AMD는 6 ~ 12달 이후에 나와서, 판매 실적을 얻기 위해 Intel보다 저가로 팔아야 했다,  이것은 효과가 있었지만매오 낮은 이익을 내는 전략이다. 2002년에, AMD는 그것의 "x86-64" instruction set을 기반으로 64-bit microprocessor를 소개했다. 이름이 암시하듯이, x86-64는 Intel instruction set의 64 bits로의 진화이다. 그것은 IA32와 완전환 backward compatibility를 유지하지만, 그것은 새로운 데이터 포맷을 더할뿐만 아니라, 더 높은 capacity와 더 좋은 성능을 가능하게 하는 다른 기능들을 추가한다. x86-64로, AMD는 역사적으로 Intel에 속한 high-end market를 잡으려고 했다. AMD의 Opteron과 Athlon 64 processors의 최근 세대들은 high performance machines에서 성공적이라고 입증되었다. 좀 더 최근에, AMD는 이 instruction set을 *AMD64*라고 rename했지만, "x86-64"는 선호되는 이름으로 남아있다.

Intel은 IA32에서 IA64로의 완전한 변환 전략이 유효하지않다는 것을 깨달아서, 2004년에 Pentium 4 Xeon line에서 프로세서들로 x86-64의 그들 자신의 variant를 포팅하기 시작했다. 그들이 Itanium를 지칭하기 위해 "IA64" 이름을 이미 사용했기 때문에, 그들은 이 64-bit extension을 위한 이름을 찾는데 어려움과 마주했다. 결국에, 그들은 x86-64를 IA32의 개선물로서 설명하기로 결정했고, 그래서 그들은 그것을 "Enhanced Memory 64-bit Technology"에 대해  *IA32-EM64T*라고 지칭했다.

GCC의 개발자들은 변한없이 i386과의 binary compatibility를 유지했다. 비록 유용한 features가 IA32 instruction set에 추가되었을지라도. PentiumPro는 conditional operations과 연관된 코드의 성능을 크게 향상시킬 수 있는 conditional move instructions의 한 set를 도입했었다. Pentium processors의 좀 더 최근 세대들은 새로운 floating point operations을 도입했다. 이것은 8087 시절의 오히려 애매하고, 이상한 접근법을 대체했고, 8086에 따라다니는 floating point coprocessor를 대체했고, main microprocessors chips내에서 새로 통합되었다. x86-64를 타겟으로서 바꾸는 것은 GCC가 backward compatibility를 포기하는 기회를 제공했찌만,대신에 이 새로운 기능들을 이용하게 했다.

이 문서에서, 우리는 Intel기반의 machines에서 작동하는 전통적인 Linux의 32-bit versions에서 발견되는 하드웨어와 GCC code의 조합을 지칭하기 위해 "IA32"를 사용한다. 우리는 AMD와 Intel로부터의 더 새로운 64-bit machines에서 작동하는 하드웨어 코드 조합을 지칭하기 위해 "x86-64"를 사용한다. Linux world에서, 이러한 두 개의 플랫폼들은 "i386" 그리고 "x86_64"라고 각각 지칭된다.



## 2 Finding Documentation

Intel과 AMD 둘 다 그들의 프로세서들에 대한 광범위한 문서를 제공한다. 이것은 hardware의 assembly language programmer의 view에 대한 일반적인 개관을 포함한다.  뿐만 아니라, 개별 instructions에대한 상세한 참조도 포함한다. amd64.org는 Linux systems에서 작동하는 x86-64 code를 위한 *Application Binary Interface* (ABI)를 정의하는데 책임이 있다. 이 interface는 procedure linkages, binary code files, 그리고 적절히 실행되기 위해 object code programs을 위해 요구되는 많은 다른 기능들을 위한 세부사항을 설명한다.

경고  : Intel과 AMD 문서 둘 다 Intel assembly code notation을 사용한다. 이것은 Gnu assembler GAS에서 사용되는 notation과는 다르다. 중요하게도, 이것은 operands를 반대 순서로 열거한다.
