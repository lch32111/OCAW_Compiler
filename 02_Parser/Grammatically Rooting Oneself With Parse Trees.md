https://medium.com/basecs/grammatically-rooting-oneself-with-parse-trees-ec9daeda7dad



# Parse Trees로 자기 자신을 문법적으로 파헤치기

기술의 세계에서 우리를 둘러 싸는 모든 추상의 것들에 대해 생각하는 것은 가끔씩 부담이 될 수 있다. 이것은 특히, 너가 너의 머리를 새로운 패러다임에 대해 이해하는데 쓰려고 할 때, 또는 너가 이해하려고 하는 한 개 또는 많은 개념들의 layers를 풀려고할 때 사실이다.

컴퓨터 공학을 배우는 맥락에서, 알고, 보고, 또는 이해해야 할 너무 많은 추상의 것들이 있다 - 그것들 모두를 이해할 수 있다는 것을 언급하는 것은 말할 것도 없고!

추상화는 너가 그것들 너머에서 볼 수 있는 강력한 것들이고, 어떻게 추상화되 었는지와 왜 그런지를 이해할 수 있는 것은 너를 더 좋은 프래그래머로 만들 수 있다. 그러나, 같은 이유로, 모든 추상화는 한 이유 떄문에 만들어졌다 : 우리 중 어느 누구도 그것들에 대해 매일 걱정할 필요가 없다. 우리는 항상 추상화에 대해 생각하려고 하지 앟ㄴ느다. 대부분, 소수만이 실제로 그런다. 여기에 요점이 있다 - 어떤 추상화들은 다른 것보다 더 동일하다. 대부분의 엔지니어들이 걱정하는 것들은 그들이 그들의 컴퓨터와 어떻게 통신하는지와 관련되어 있는 것이고, 그들의 컴퓨터가 실제로 그것들을 *이해하는*  방법들이다. 비록 우리 중 어느 누구도 [a bubble sort algorithm](https://medium.com/basecs/bubbling-up-with-bubble-sorts-3df5ac88e592)를 작성할 필요가 없을지라도, 만약 코드를 작성한다면, 우리는 우리의 기계들과 의사소통 해야만 한다.

음, 마침내 우리가 이러한 수수께끼들의 밑바닥에 도달할 시간이고, programmers로서의 우리의 workflows를 보강하는 추상화를 이해할 시간이다.



### parsing의 의미에 대해  Parsing하기

트리 자료구조는 우리의 computer science adventures 에서 계속해서 다시 나오는 것이다. 우리는 그것들이 모든 유형의  [to store data](https://medium.com/basecs/how-to-not-be-stumped-by-trees-5f36208f68a7) (데이터를 저장)하기 위해서 사용되는 것을 보았고, 우리는  [are self-balancing](https://medium.com/basecs/busying-oneself-with-b-trees-78bbf10522e7) (스스로 밸런스를 잡는) 것들을 보았다. 반면에 다른 것들은 공간과 저장용량을 다루기 위해 최적화 되었다 ([optimized for space](https://medium.com/basecs/compressing-radix-trees-without-too-many-tears-a2e658adb9a0)). 우리는 트리가 규칙들의 한 세트에 맞도록 하기 위해 그것들을 회전하고 recoloring하여  [how to manipulate trees](https://medium.com/basecs/painting-nodes-black-with-red-black-trees-60eacb2be9a5)하는 방법들을 심지어 보앗다.

그러나 data structure flora의 이러한 모든 다른 형태들에도 불구하고, 우리가 아직 발견해야하는 트리 자료구조의 특정한 반복이 있다. 비록 우리가 컴퓨터 공학, 트리를 밸런싱 하는 방법, 또는 트리 자료구조가 어떻게 작동하는지에 대해 어떠한 것도 모를지라도,  모든 프로그래머들은 매일 한 유형의 트리 구조와 상호작용한다. 코드를 작성하는 모든 개발자가 그들의 코드가 그들의 기계들에 의해 이해된다는 것을 보장할 필요가 있는 간단한 사실덕분에.

이 자료구조는 parse tree라고 불려진다. 그리고 그것은 프로그래머로서 우리가 작성하는 코드가 우리의 컴퓨터들에 의해 "readable" 읽을 수 있도록 해주는 내재된 추상화 (중의 하나)이다.

**Parse tree**는 한 문장의 문법적 구조의 그려지고, 사진화가 된 버전이다. Parse tree 실제로 언어학의 분야에서 기원을 하지만, 그것들은 또한 교육학에서 사용된다. 교육한은 교육에 대한 연구이다. Parse trees는 종종 학생들에게 한 문장의 parts를 확인하는 방법을 가르치는데 사용된다. 그리고 문법적 개념들을 소개하는 흔한 방법이다. 우리 각각은 sentence diagramming의 관점으로부터 그것들과 상호작용할 가능성이 높다. 그리고 우리중 어떤 이는 초등학교에 그것을 배웟다.

Sentence diagramming은 단일의 문장은 그것의 가장 작고, 가장 구분되는 부분들로 분해하는 것을 포함한다. 만약 우리가 diagramming sentences의 관점에서 parse trees에 대해 생각한다면, 우리는 한 문장의 문법과 언어에 따라, parse tree가 많은 다른 방식으로 구성될 수 있다는 것을 깨닫기 시작할 것이다!

그러나, "sentence"의 컴퓨터 버전은 무엇인가? 그리고 우리는 그것을 어떻게 diagramming화 할 것인가? 정확히?

음, 그것은 우리가 이미 편해 하는 어떤 것의 예제로 시작하는 것이 도움이 ㅗ딘다. 그래서 일반 영어 문장을 다이어그램화 하여 우리의 기억을 전환시켜보자.

여기에서 보여진 그림에서, 우리는 간단한 한 문장을 가진다: `"Vaidehi ate the pie"`. 우리는 parse tree가 diagrammed sentence라는 것을 알기 때문에, 우리는 이 예제 문장에서 parse tree를 구성할 수 있다. 우리가 효과적으로 하려고 하는 것이 이 문장의 다른 부분들을 결정하고, 그것을 가장 작고, 가장 구분되는 부분들로 분해하는 것이라는 것을 기억해라.

우리는 그 문장을 두 개의 파트들로 나누어 시작한다 : **noun(명사)**, `"Vaidehi"` 그리고 **verb phrase(동사구)**, `"ate the pie"`. 명사는 더 이상 분해되어질 수 없기 때문에, 우리는 그 단어 `"Vaidehi"`를 그대로 남겨둘 것이다. 그것에 대해 생각하는 또 다른 방법은 우리가 더 이상 명사를 분해할 수 없기 때문에, 이 단어로부터 자식 노드들이 있지 않을 것이라는 사실이다.

그러나, verb phrase인 `"ate the pie"`는 어떤가? 음 그 phrase(구)는 아직 가장 간단한 형태로 분해되지 않았다, 그렇지 않은가? 우리는 그것은 더 나눌 수 있다. 한 가지 것에 대해, `"ate"` 단어는 verb(동사)이고, 반면에 `"the pie"`는 명사 이상의 것이다 - 사실, 그것은 완전히 정확하게는 **noun phrase (명사구)**이다. 만약 우리가 `"ate the pie"`를 쪼갠다면, 우리는 그것을 verb와 noun phrase로 나눌 수 있다. verb는 어떤 추가적인 세부사항으로 다이어그램화 될 수 없기 때문에, 그 단어 `"ate"`는 우리의 parse tree에서 leaf node가 될 것이다.

좋아, 이제 남은 것은 명사구 `"the pie"`이다.  우리는 이 구를 두 개의 별개의 조각들로 나눌 수 있다: 명사 `"pie"`. 그리고 그것의 **determiner(한정사)**, 이 한정사는 명사를 수정하는 단어로 알려져 있다. 이 경우에, 그 한정사는 `"the"`라는 단어이다.

우리가 우리의 명사구를 나눴으니, 우리는 우리의 문장을 쪼개는 것을 끝냈다! 다시 말해서, 우리는 우리의 parse tree를 다이어그램화하는 것을 했다는 것이다. 우리가 우리의 parse tree를 볼 때, 우리는 우리의 문장이 여전히 같은 방식으로 읽혀질 수 있다는 것을 알아차릴 것이고, 우리는 그것을 사실 전혀 바꾸지 않았다. 우리는 우리가 받은 문장을 가져와서, 그것을 그것의 가장 작고, 가장 구분되는  부분으로 쪼개기 위해 영어 문법의 규칙들을 사용한다.

그러나, 이것은 한 특정한 언어에서, 한 특정한 문장이, 그것의 문법 규칙과 함께 parse tree로 어떻게 다이어그램화 되는 지에 대한 예시이다. 이 같은 문장은 다른 언어들에서 다르게 보일지도 모른다. 특히 만약 그것이 그것의 문법 규칙을 따라야만 한다면.

궁그적으로, 한 언어의 *grammar* and *syntax* - 그 언어의 문장들이 구성되는 방법을 포함하여 - 는 그 언어가 어떻게 정의되는지와 우리가 그것에서 어떻게 쓰는지, 그 언어를 말하는 사람들이 그것을 어떻게 이해하고 해석하는지를 결정하는 규칙들이다.

흥미롭게도, 우리는 그 간단한 문장 `"Vaidehi ate the pie."`를 어떻게 다이어그램화 하는지 알았다. 왜냐하면 우리는 이미 영어 문법에 친숙하기 때문이다. 우리의 문장이 명사 또는 동사가 없는 것을 상상해보아라. 무엇이 발생하는가? 음, 우리는 그 문장을 처음에 읽을 것이고, 빠르게 그것이 전혀 문장이 아니라는 것을 깨닫게 된다. 오히려, 우리는 그것을 읽고, 거의 즉시 우리가 **sentence fragment**를 즉, 한 문장의 불완전한 조각을 다루고 있다는 것을 알게 된다.

그러나, 우리가 한 sentence fragment를 인지할 수 있는 유일한 이유는, 우리가 그 영어의 규칙을 아는가이다 - 이름하여, 모든 문장은 유효하게 고려되기 위해 명사와 동사를 필요로 한다. 한 언어의 문법은 우리가 한 문장이 언어에서 유효한지를 보기위해 체크할 수 있는 방법이다; 즉, 유효성을 위해 "checking"하는 것의 프로세스는 한 문장을 **parsing**하다고 지칭된다.

우리가 처음에 문장을 읽을 때 그것을 이해하기 위해 그것을 parsing하는 것의 프로세스는 한 문장을 다이어그램화  것과 같은 정신적 단계를 포함한다. 그리고, 한 문장을 다이어그램화 하는 것은 parse tree를 구성하는 것과 같은 단계를 포함한다. 우리가 처음으로 한 문장을 읽을 때, 우리는 정신적으로 그것을 분해하고 parsing하는 일을 하고 있다.

이제 알게 되었으니, 컴퓨터들은 우리가 작성하는 코드로 정확히 같은 것을 하게 된다.



### Parsing expressions like it's our job

좋아, 그래서 우리는 이제 영어 문장을 어떻게 다이어그램화하고 parse하는지를 안다. 그러나, 그것이 어떻게 코드에 적용되는가? 심지어 우리의 코드에서 "sentence"라는 것이 무엇인가?

음, 우리는 한 parse tree를 우리의 코드가 어떻게 보이는지에 대한 그려진 "사진"이라고 생각할 수 있다. 만약 우리의 코드, 프로그램, 심지어 한 문장의 형태로 된 스크립트의 가장 간단한 것으로 생각한다면, 우리는 아마도 꽤 바르게 우리가 작성하는 모든 코드가 표현들의 집합으로 simplified 되어질 수 있다는 것을 깨닫게 될 것이다.

이것은 한 예시로 좀 더 이해가 된다. 그래서 매우 간단한 calculator program을 보자. single expression을 사용하여, 우리는 그 식으로부터 parse tree를 생성하기 위해 수학의 문법 규칙을 사용할 수 있다. 우리는 우리의 식에서 가장 간단하고, 가장 구분되는 단위를 찾을 필요가 있을 것이다. 이것은 우리가 우리의 식을 더 작은 부분들로 분해할 필요가 있다는 것이다, 아래에서 보여지듯이.

우리는 단일의 수학 식이 따라야 할 그것 자신의 규칙들을 가지고 있는 것을 알게 될 것이다; (함께 곱해지는 두 개의 수, 그러고나서 또 다른 수에 더해지는 것 같은) 즉 한 가지 간단한 식 은 심지어 그들 자체로 더 간단한 식으로 분리될 수 있다.

그러나, 시작하기에 간단한 계산으로 작업해보자. 우리는 `2 x 8`같은 식에 대해 수학 문법을 사용하여 어떻게 parse tree를 생성할 수 있는가?

만약 우리가 이 식이 어떻게 보이는지에 대해 생각한다면, 우리는 여기에 세 개의 다른 조각들이 있다는 것을 알 수 있다: 왼쪽에 expression, 오른쪽에 expression, 그리고 그것들 두 개를 함께 곱하는 operation.

여기에서 보여지는 이미지는 그 식 `2 x 8`를 parse tree로 다이어그램화 한다. 우리는 그 연산자 `x`가 더 이상 simplified 될 수 없는 식의 한 조각인 것을 알 것이다. 그래서 그것은 어떤 자식 노드들을 가지지 않는다.

왼쪽과 오른쪽에 있는 expression은 특정한 terms(항)으로 simplified 될 수 있다. 이름 그대로 `2`와 `8`. 우리가 이전에 봤던 영어 문장 예제 처럼, 단일의 수학적 expression은 그것 내부에 *internal expressions* 뿐만 아니라, 개별 *terms*, `2 x 8`같은 phrase 또는  그 자체로 개별 expression으로서 숫자`2`같은 *factors*들을 포함할 수 있다.

그러나, 우리가 이 parse tree를 만든 후에 무엇이 발생하는가? 여기에서 자식 노드들의 계층은 이전의 우리의 문장 예제보다 덜 명백하다는 것을 알게 될 것이다. `2`와 `8` 둘 다 같은 level에 있다. 그래서 우리는 이것을 어떻게 해석하는가?

음, 우리는 한 트리를 다양한 [different ways to depth-first traverse](https://medium.com/basecs/demystifying-depth-first-search-a7c14cccf056)가 있다는 것을 이미 알고 있다. 우리가 이 트리를 어떻게 탐색할지에 따라, 이 단일 수학식 `2 x 8`은 많은 방식으로 해석되고 읽혀질 수 있다. 예를들어, 우리가 *inorder* traversal를 사용하여 이 트리를 탐색했다면, 우리는 left tree, rott level, 그러고나서 right tree를 읽을 것인데, 다음의 결과를 만든다 `2 -> x -> 8`.

그러나 우리가 만약 *preorder* traversal를 사용하여 트리를 탐색하기를 선택 한다면, 우리는 root level에 있는 값을 처음에 읽고, 그 다음에 왼쪽 subtree를 그러고나서 right subtree를 읽을 것이고, 이것은 우리에게 `x -> 2 -> 8`를 줄 것이다. 그리고 만약 우리가 *postorder* 탐색을 사용했다면, 우리는 left subtree, right subtree, 그리고 마지막으로 root level를 읽을 것이고, 이것은 `2 -> 8 -> x`를 만들 것이다.

Parse trees는 우리의 expressions이 어떻게 생겼는지를 보여주고, 우리의 expressions의 구체적인 syntax를 보여준다. 이것은 종종 단일의 parse tree가 다양한 다른 방식들로 한 "sentence"를 표현할 수 있다는 것을 의미한다. 이 이유 때문에, parse trees는 또한 종종 **Concrete Syntax Trees**, 짧게 해서 **CSTs**라고 지칭된다. 이러한 트리들이 해석되거나, 즉 우리의 기계들에 의해 "읽혀질" 때, 이러한 트리들이 어떻게 parsed 되어야 되는지에 대한 엄격한 규칙들이 있을 필요가 있다. 이것은 올바른 expression, 올바른 순서와 올바른 위치에 있는 모든 terms을 가지도록 하기 위해서이다!

그러나, 우리가 다루는 대부분의 expressions은 `2 x 8`보다 더 복잡하다. 심지어 calculator program에 대해, 우리는 아마도 좀 더 복잡한 연산들을 할 필요가 있을지도 모른다. 예를들어, 만약 우리가 `5 + 1 x 12`같은 식을 풀길 원한다면 어떨까? 우리의 parse tree는 어떻게 보일까?

음, 밝혀졌듯이, parse trees의 문제는 가끔씩 한 개 이상으로 끝날 수 있다.

좀 더 구체적으로, parsed되고 있는 한 expression에 대해 한 개 이상의 결과가 있을 수 있다. 만약 우리가 parse trees가 lowest-level에서 처음에 읽혀진다고 가정한다면, 우리는 leaf nodes의 hierarchy가 같은 식이 두 개의 전적으로 다른 방식으로 해석되게끔 할 수 있느 방법을 보기 시작할 수 있다. 그리고 이것은 결과로 두 개의 다른 값을 만들어 낼 것이다.

예를들어, 위의 그림에서, 식 `5 + 1 x 12`에 대해 두 가지 가능한 parse trees가 있다. 우리가 왼쪽의 parse tree에서 볼 수 있듯이, 그 노드들의 hierarchy는 식 `1 x 12`이 처음에 구해질 것이고, 그러고나서 그 합이 계속될 것이다 :`5 + ( 1 x 12)`. 반면에, 오른쪽 parse tree는 매우 다르다; 그 노드들의 hierarchy는 덧셈이 처음에 발생하게 하고 (`5 + 1`), 그러고나서 곱셉으로 트리를 이동시킨다 : `(5 + 1) x 12`.

한 언어에서 애매한 문법은 정확히 이러한 종류의 상황을 발생시키는 것이다 : syntax tree가 구성되는 방법이 명확하지 않을 때, 그것이 (적어도) 한 개 이상의 방법으로 구성되는 것이  가능하다.

학교에서 대부분 우리가 배웠던 수학의 규칙을 기반으로, 우리는 내재적으로, 곱이 항상 덧셈 전에 되어야만 한다는 것을 안다. 다시 말해서,  위의 예제에서 오직 왼쪽의 parse tree가 수학의 문법을 기반으로 실제로 올바르다. 기억해라 : 문법은 syntax와 어떤 언어의 규칙들을 정의하는 것이다. 그것이 영어 문장이든 수학 식이든.

그러나, 도대체 컴파일러가 이러한 규칙들을 내재적으로 어떻게 아는가? 음, 그것이 할 수 있는 방법이 없다! 한 컴파일러는 만약 우리가 그것에게 따라야 할 문법 규칙을 주지 않는다면, 우리가 작성하는 코드를 읽을 방법에 대해 어떠한 생각이 없다. 만약 컴파일러가 우리가 수학식을 작성한 것을 보면,  예를들어, 그것은 두 개의 다른 parse trees를 만들 수 있고, 그것은 두 개의 parse trees중 어떠한 것을 고를 줄 모를 것이고, 따라서, 그것은 우리의 코드를 심지어 어떻게 읽거나 해석해야 할지 모를 것이다.

애매한 문법은 일반적으로 이러한 이유 때문에 대부분의 프로그래밍 언어들에서 피해진다. 사실, 대부분의 parsers와 programming languages는 의도적으로 처음부터 애매함의 문제를 처리할 것이다. **한 프로그래밍 언어는 보통 *precedence*(우선)을 강화하는 문법을 가질 것이다. 여기에서 *precedence*는 어뗜 연산들 또는 symbols이 다른 것들 보다 더 높은 weight/value를 가지는 것을 강제할 것이다.**  이것의 한 예시는, 한 parse tree가 구성될 때 마다, 곱셈이 덧셈보다 더 높은 우선순위를 받는 것이다. 그래서 오직 하나의 parse tree만이 생성되어지게 할 수 있다.

애매함의 문제들과 싸우는 또 다른 방법은 grammar가 해석되는 방식을 강효하여 된다. 예를들어, 수학에서, 만약 우리가 `1 + 2 + 3 + 4`같은 식을 가졌따면, 우리는 내재적으로 우리는 왼쪽에서 더하기 시작하고, 오른쪽으로 해 나갈 것이다. 만약 우리의 컴파일러가 이것을 우리 자신의 코드로 어떻게 하는지 이해하길 원했다면, 우리는 *left associativity*를 가용할 것인데, 이것은 우리의 컴파일러가 그것이 우리의 코드를 parse할 때, 그것이 parse tree hierarchy에서 1 factor보다 더 낮은 4 factor를 넣는 parse tree를 만들도록 할 것이다. (여긴 뭔 말인지 모르겠다)

이러한 두 예제들은 종종 컴파일러 설계에서 **disambiguating rules (애매함 없애기 규칙**이라고 지칭된다. 왜냐하면 그것들은 우리의 컴파일러가 매우 혼동할 애매한 문법들을 우리가 겪지 않도록 보장하는 특정한 syntatical rules를 만들기 때문이다.



### A token of my affection (for my parser)

만약 ambiguity(애매함)이 모든 parse tree evil의 근원이라면, 그러면 clarity(명확함)은 명백히 더 선호되는 연산의 모드이다. 확실히, 우리는 우리의 좋지 않은 컴퓨터가 그것이 코드를 읽을 때 휘청거리게 하는 애매한 상황을 피하는 disambiguating rules를 추가할 수 있짐난, 우리는 실제로 그것보다 더 많이 그렇게 또한 한다. 오히려, 실제로 매우 많이 하는 것이 우리가 사용하는 프로그래밍 언어이다!

설명해보자. 우리는 이 방식으로 그것을 생각할 수 있다 : 우리가 수학식에 명확성을 추가할 수 있는 방법들 중 하나는 괄호를 통한 것이다. 사실, 그것은 대부분의 우리가 아마 우리가 이전에 다루었떤 식에 해야할 것이다 : `5 + 1 x 12`. 우리는 아마도, 그 식을 읽었고, 학교에서 우리가 배웠던 [the order of operations](http://www.purplemath.com/modules/orderops.htm) (연산의 순서)들을 회상하여, 우리는 우리의 머리에 그것을 `5 + (1 x 12)`로 다시 작성할 것이다. 그 괄호 `()`는 우리가 그 식 자체와 그것 안에 내재된 두 expressions들에 대해 대해 명확성을 얻는데 도움울 줬다. 이러한 두 개의 symbols은 우리에게 인식가능하지만, 만약 우리가 그것들을 parse tree에 넣는다면, 그것들은 어떠한 자식 노드들도 가지지 않을 것이다. 왜냐하면 그것들은 더 이상 분해될 수 없기 때문이다.

이러한 것들을 우리가 ***terminals***라고 지칭하는 것인데, 이것은 또한 흔히 ***tokens***라고 알려져 있다. 그것들은 모두 프로그래밍 언어들에 중요한데, 왜냐하면 그것들은 한 expression의 일부분들이 서로 연관짓는 방법을 이해하는데 도와주고, 개별 elements사이의 syntatic relationships을 이해하는데 도와주기 때문이다. 프로그래밍에서 어떤 흔한 tokens은 연산 기호들 (`+`, `-`, `x`, `/`), 괄호 (`()`), 그리고 보유된 conditional words (`if`, `then `, `else`, `end`)를 포함한다. 어떤 토큰들은 expressions을 명확히 만드는데 도움을 주기 위해 사용된다. 왜냐하면 그것들은 다른 elements들이 서로 연관짓는 방법을 명시할 수 있기 때문이다.

그래서, 우리의 parse tree에서 다른 것들은 무엇인가? 우리는 명백히 우리의 코드에서 `if`들과 `+`기호들 이상을 가지고 있다. 음, 우리는 보통 **non-terminals**의 집합을 다뤄야만 한다. 이것들은 이후에 잠재적으로 분해될 수 있는 expressions, terms, and factors이다. 이러한 것들은 그것들 안에 다른 식들, `(8 + 1) / 3`같은 다른 식을 포함하는 phrases/ideas이다.

terminals과 non-terminals 둘 다 그것들이 parse tree에서 나타나는 곳에 특정한 연관을 가진다. 그들의 이름이 암시하듯이, *terminal* symbol은 항상 parse tree의 leaves로서 끝날 것이다; 이것은 operators, parenthesis, and reserved conditionals "terminals" 뿐만 아니라, string, number, 또는 모든 leaf node에 있는 개념들을 나타내는 factor values들도 그렇다는 의미이다. 가능한 가장 작은 조각으로 분해될 수 있는 어떤 것은 효과적으로 항상 "terminal"이 될 것이다.

반대 쪽에서, parser tree의 interior nodes - parent nodes들이 non-leaf nodes - 는 *non-terminal* symbols이다. 그리고 그것들은 프로그래밍 언어의 문법 규칙들의 적용을 나타내는 것들이다.

우리가 한 parser tree가 우리의 프로그램, 그리고 그것 안에 있는 모든 symbols, concepts, 그리고 expressions일 뿐이라는 사실로 우리의 머리를 채운다면, parser tree는 이해하고, 시각화하고, 확인하는데 더 쉽게 된다.

음, 밝혀지듯이, parser tree에 대해 가장 신경쓰는 것은 **parser**인데, 이것은 우리가 작성하는 모든 코드를 *parsing*하는 프로세스를 처리하는 컴파일러의 한 일부이다.

**parsing**의 프로세스는 어떤 인풋을 가져와서, 그것에서 parse tree를 구성하는 것이다. 그 input은 string, sentence, expression, 또는 심지어 하나의 전체 프로그램 같이 많은 다른 것들이 될 수 있다.

우리가 그것에게 무슨 input을 주건간에, 우리의 parser는 그 input을 grammatical phrases로 parse할 것이고, 그것에서 parse tree를 구성할 것이다. 그 parser는 정말로 우리의 컴퓨터와 컴파일 프로세스 맥락에서 두 가지 주된 역할을 갖는다:

1. tokens의 유효한 sequence를 받을 때, 그것은 대응되는 parse tree를 생성할 수 있어야 하고, 그 언어의 syntax가 나와야 한다.
2. tokens의 유효하지 않은 sequence를 받을 때, 그것은 syntax error를 탐지할 수 있어야 하고, 프로그래머의 코드에서 문제의 코드를 작성한 프로그래머에게 알릴 수 있어야 한다.

그리고 , 그것을 정말로 그런다! 그것은 정말 간단히 들리지 모르지만, 만약 우리가 어떤 프로그램들이 얼마나 ㄱ더ㅐ하고 복잡할 수 있는지 고려하기 시작한다면, 우리는 빠르게 parser가 실제로 이러한 두 개의 겉보기에 쉬운 역할을 이행하기 위해서 상황들이 얼마나 잘 정의되어야 할 필요가 있는지를 깨닫기 시작할 것이다.

예를들어, 심지어 한 간단한 parser는 `1 + 2 + 3 x 4`같은 expression의 syntax를 처리하기 위해 많은 것을 할 필요가 있다.

* 처음에, 그것은 이 expression에서, parse tree를 구성할 필요가 있다. 그 parser가 받는 input string은 연산들 사이에 어떠한 연관을 보여주지 않을지도 모르지만, 그 parser은 그렇게 하는 parse tree를 만들 필요가 있다.
* 그러나, 이것을 하기 위해서, 그것은 그 언어의 syntax와 따라야 할 문법 규칙들을 알 필요가 있다.
* 그것이 실제로 단일의 parse tree를 만들 수 있따면 (어떠한 애매함 없이), 그것은 tokens and non-terminal symbols를 가져올 필요가 있고, 그 parse tree의 hierarchy가 올바르게 되기 위해 그것들을 배치할 필요가 있다.
* 마지막으로, 그 parser는 이 트리가 구해졌을 때, 같은 우선순위의 operators에 대해 왼쪽에서 오른쪽으로 구해지도록 보장할 필요가 있다.
* 그러나 기다려라! 그것은 또한, 이 트리가 bottom에서 inorder traversal method를 사용하여 탐색 될 때 어떠한 single syntax error가 발생하지 않도록 보장할 필요가 있다.
* 물론, 만약 그것이 멈춘다면, 그 parser는 인풋을 볼 필요가 있고, 어디에서 break되는지를 알아내고, 프로그래머에게 그것에 대해 말할 필요가 있다.

만약 이것이 끔찍한 일이라고 느껴진다면, 그것이 그렇기 때문이다. 그러나 그 모든 것을 하는 것에 대해 너무 걱정하지 말아라. 왜냐하면 그것은 parser의 일이기 때문이다. 대부분 그것은 추상화 되어진다. 운 좋게도, 그 parser는 컴파일러의 다른 부분으로부터 도움받는다 : 다음주에 더 많은 것을 이야기 한다!



여기 밑에는 필요 없으니 번역 안함

### Resources

Lucky for us, compiler design is something that is well-taught in almost every computer science curriculum, and there are a decent number of solid resources out there to help us understand the different parts of a compiler, including a parser and the parse tree. However, as is the case with most CS content, a lot of it can be hard to digest — particularly if you’re not familiar with the concepts or the jargon that’s used. Below are some more beginner-friendly resources that still do a good job of explaining parse trees if you find yourself wanting to learn even more.

1. [Parse Tree](http://interactivepython.org/runestone/static/pythonds/Trees/ParseTree.html), Interactive Python
2. [Grammars, Parsing, Tree Traversals](http://www.cs.cornell.edu/courses/cs2110/2014fa/L21-ParsingTrees/cs2110fa14Parsing.pdf), Professors David Gries & Doug James
3. [Let’s Build A Simple Interpreter, Part 7](https://ruslanspivak.com/lsbasi-part7/), Ruslan Spivak
4. [A Guide to Parsing: Algorithms and Terminology](https://tomassetti.me/guide-parsing-algorithms-terminology/#parser), Gabriele Tomassetti
5. [Lecture 2: Abstract and Concrete Syntax](http://www.cse.chalmers.se/edu/year/2011/course/TIN321/lectures/proglang-02.html), Aarne Ranta
6. [Compilers and Interpreters](http://flint.cs.yale.edu/cs421/lectureNotes/c04.pdf), Professor Zhong Shao
7. [Compiler Basics — The Parser](http://www.cs.man.ac.uk/~pjj/farrell/comp4.html), James Alan Farrell