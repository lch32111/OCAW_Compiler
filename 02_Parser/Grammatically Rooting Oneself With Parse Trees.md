https://medium.com/basecs/grammatically-rooting-oneself-with-parse-trees-ec9daeda7dad



# Parse Trees로 자기 자신을 문법적으로 파헤치기

기술의 세계에서 우리를 둘러 싸는 모든 추상의 것들에 대해 생각하는 것은 가끔씩 부담이 될 수 있다. 이것은 특히, 너가 너의 머리를 새로운 패러다임에 대해 이해하는데 쓰려고 할 때, 또는 너가 이해하려고 하는 한 개 또는 많은 개념들의 layers를 풀려고할 때 사실이다.

컴퓨터 공학을 배우는 맥락에서, 알고, 보고, 또는 이해해야 할 너무 많은 추상의 것들이 있다 - 그것들 모두를 이해할 수 있다는 것을 언급하는 것은 말할 것도 없고!

추상화는 너가 그것들 너머에서 볼 수 있는 강력한 것들이고, 어떻게 추상화되 었는지와 왜 그런지를 이해할 수 있는 것은 너를 더 좋은 프래그래머로 만들 수 있다. 그러나, 같은 이유로, 모든 추상화는 한 이유 떄문에 만들어졌다 : 우리 중 어느 누구도 그것들에 대해 매일 걱정할 필요가 없다. 우리는 항상 추상화에 대해 생각하려고 하지 앟ㄴ느다. 대부분, 소수만이 실제로 그런다. 여기에 요점이 있다 - 어떤 추상화들은 다른 것보다 더 동일하다. 대부분의 엔지니어들이 걱정하는 것들은 그들이 그들의 컴퓨터와 어떻게 통신하는지와 관련되어 있는 것이고, 그들의 컴퓨터가 실제로 그것들을 *이해하는*  방법들이다. 비록 우리 중 어느 누구도 [a bubble sort algorithm](https://medium.com/basecs/bubbling-up-with-bubble-sorts-3df5ac88e592)를 작성할 필요가 없을지라도, 만약 코드를 작성한다면, 우리는 우리의 기계들과 의사소통 해야만 한다.

음, 마침내 우리가 이러한 수수께끼들의 밑바닥에 도달할 시간이고, programmers로서의 우리의 workflows를 보강하는 추상화를 이해할 시간이다.



### parsing의 의미에 대해  Parsing하기

트리 자료구조는 우리의 computer science adventures 에서 계속해서 다시 나오는 것이다. 우리는 그것들이 모든 유형의  [to store data](https://medium.com/basecs/how-to-not-be-stumped-by-trees-5f36208f68a7) (데이터를 저장)하기 위해서 사용되는 것을 보았고, 우리는  [are self-balancing](https://medium.com/basecs/busying-oneself-with-b-trees-78bbf10522e7) (스스로 밸런스를 잡는) 것들을 보았다. 반면에 다른 것들은 공간과 저장용량을 다루기 위해 최적화 되었다 ([optimized for space](https://medium.com/basecs/compressing-radix-trees-without-too-many-tears-a2e658adb9a0)). 우리는 트리가 규칙들의 한 세트에 맞도록 하기 위해 그것들을 회전하고 recoloring하여  [how to manipulate trees](https://medium.com/basecs/painting-nodes-black-with-red-black-trees-60eacb2be9a5)하는 방법들을 심지어 보앗다.

그러나 data structure flora의 이러한 모든 다른 형태들에도 불구하고, 우리가 아직 발견해야하는 트리 자료구조의 특정한 반복이 있다. 비록 우리가 컴퓨터 공학, 트리를 밸런싱 하는 방법, 또는 트리 자료구조가 어떻게 작동하는지에 대해 어떠한 것도 모를지라도,  모든 프로그래머들은 매일 한 유형의 트리 구조와 상호작용한다. 코드를 작성하는 모든 개발자가 그들의 코드가 그들의 기계들에 의해 이해된다는 것을 보장할 필요가 있는 간단한 사실덕분에.

이 자료구조는 parse tree라고 불려진다. 그리고 그것은 프로그래머로서 우리가 작성하느 코드가 우리의 컴퓨터들에 의해 "readable" 읽을 수 있도록 해주는 내재된 추상화 (중의 하나)이다.

**Parse tree**는 한 문장의 문법적 구조의 그려지고, 사진화가 된 버전이다. Parse tree 실제로 언어학의 분야에서 기원을 하지만, 그것들은 또한 교육학에서 사용된다. 교육한은 교육에 대한 연구이다. Parse trees는 종종 학생들에게 한 문장의 parts를 확인하는 방법을 가르치는데 사용된다. 그리고 문법적 개념들을 소개하는 흔한 방법이다. 우리 각각은 sentence diagramming의 관점으로부터 그것들과 상호작용할 가능성이 높다. 그리고 우리중 어떤 이는 초등학교에 그것을 배웟다.

