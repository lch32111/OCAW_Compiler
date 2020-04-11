https://journal.stuffwithstuff.com/2011/03/19/pratt-parsers-expression-parsing-made-easy/



# Pratt Parsers : Expression Parsing Made Easy

때때로, 나는 매우 똑똑하고 한 문제에 대해 완벽해서, 내가 더 똑똑해지고 그것을 배움으로써 새로운 힘을 얻는 것처럼 느끼는 알고리즘 또는 아이디어를 우연히 만나게 된다. [Heaps](http://en.wikipedia.org/wiki/Heap_(data_structure)) (나의 짧은CS 교육에서 내가 얻은 유일한 것이다) 은 이것과 같은 한 가지 것이다. 나는 최근에 또 다른 것을 만나게 되었다: [Pratt](http://en.wikipedia.org/wiki/Vaughan_Pratt) parsers.

너가 한 parser를 작성할 때, [recursive descent](http://en.wikipedia.org/wiki/Recursive_descent)는 땅콩 버터를 펴는 것 만큼 쉽다. 그것은 너가 파싱하는코드의 다음 청크를 기반으로 다음에 무엇을 하려고 할 지를 알아낼 수 있을때 뛰어나다. 그것은 보통 classes같은 것들이 있는 한 언어의 top level에 대해 해당되고, 그것들을 uniquely하게 인식하는 어떤 것 (`if`, `for`, `whilte`, 등)으로 시작한 이후의 statements에 대해서 또한 해당된다.

그러나, 너가 expressions 에 도달할 때 까다로워진다. `+`같은 중위 연산자, `++` 같은 후위연산자, 그리고 심지어 `?:` 같은 혼합연산자에 대해, 너가 그것에 중간에 도달할 때까지 너가 파싱하고 있는 식이 무슨 종류인지 알아내기 어려울 수 있따. 너는 recursive descent로 이것을 할 수 있지만, 그것은 하기 싫은 일이다. 너는 precedence의 각 level에 대해 별개의 함수들을 작성해야하고 (예를들어 JavaScript 그러한 것들 중 17개 가지고 있다.), 수동으로 연관성을 처리하고, 보기 어려울 때 까지, 많은 parsing code를 거쳐서 너의 문법을 더럽힌다.



### PB&J, The Secret Weapon

Pratt parsing은 그것을 해결한다. 만약 recursive descent가 땅콩 버터라면, Pratt parsing은 jelly이다. 너가 그 두 개를 함께 섞을 때, 너는 너가 그것에 던지는 어떤 문법이든 처리할 수 있는 간단하고, 간결하고, readable parser를 얻게 된다.

operator precedence와 infix expressions를 처리하기 위한 Pratt의 기법은 너무 간다하고 효과적이어서, 왜 거의 어느 누구도 그것에 대해 알지 못하는지는 의문이다. 70년대 이후, top down operator precedence parsers는 사라진 것처럼 보인다. Douglas Crockford의 [JSLint](http://www.jslint.com/) 는 [parse JavaScript](http://javascript.crockford.com/tdop/tdop.html)하기 위해 하나를 사용하지만, 그의 처리는 그것에 대한 the [very few](http://effbot.org/zone/simple-top-down-parsing.htm) remotely modern articles중의 하나이다.

내가 생각하기에, 문제의 일부는, Pratt의 용어는 이해하기 힘들고, Crockford의 글은 그 자체로 어두 컴컴하다. Pratt은 "null denominator"같은 용어들을 사용하고, Crockford는 core  idea를 모호하게 만드는 lexical scope를 추적하는 것과 같은 추가 적인것을 혼합한다.

이것은 내가 도달하게 된것이고, 나는 어떠한 혁멱정인 것을 하지 않을 것이다. 

