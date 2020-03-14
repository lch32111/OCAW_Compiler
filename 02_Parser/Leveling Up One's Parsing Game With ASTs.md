https://medium.com/basecs/leveling-up-ones-parsing-game-with-asts-d7a6fc2400ff



# ASTs로 Parsing Game 레벨업 하기

내가 컴퓨터 공학을 배우려는 여행을 시작하기 전에, 내가 다른 방향으로 달리는 것을 원하게 만들었던 어떤 용어들과 문장들이 있었다.

그러나 달리는 대신에, 그것의 지식을 아는척 했고, 대화도중에 고개를 끄덕이고, 어떤 사람이 참조하고 있는 것을 아는 척 했다. 비록 사실 나는 전혀 몰랐고, 그 매우 무서운 컴퓨터 공학 용어를 들었을 때 실제로 전적으로 듣는 것을 멈추려했을지라도. 이 시리즈의 과정 도처에서, 나는 간신히 많은 기반을 다루었고, 그러한 많은 용어들은 실제로 전반적으로 덜 무섭게 되었다!

그런데 내가 잠시동안 피하고 있는 한 가지 큰게 있다. 지금 까지, 내가 이 용어를 들을 때 마다, 나는 마비가 되는 것을 느꼈었다. 그것은 meet ups에서 평상 시 대화에서 튀어나왔고, 가끔씩 회의에서도 나왔다. 매번, 나는 기계가 돌아가는 것을 생각하고, 이해할 수 없는 code의 strings을 컴퓨터가 뱉어내는 것을 생각한다. 내 주변의 모든 사람이 실제로 *그것들을 판독한다는* 것을 제외하고. 그래서 무슨 일이 진행되는지 모르고 있는 것은 실제로 나이다 (어떻게 이런게 발생하는가?!).

아마도, 나만이 그렇게 느낀건 아니다. 그러나, 나는 너에게 이 용어가 실제로 무엇인지 말해야 된다고 가정한다. 그렇지 않는가? 준비해라. 왜냐하면 나는 찾기 힘들고, 겉보기에 혼란스러운 *abstract syntax tre*를 또는 짧게 AST를 말하고 있기 떄문이다. 위협당한지 몇 년 후에, 나는 마침내 이 용어를 두려워하는 것을 멈추어서 기쁘고, 실제로 그것이 도대체 무엇인지를 이해해서 기쁘다.

abstract syntax tree의 뿌리를 직접 볼 시간이다. 우리의 parsing game을 한 단계 증가 시키자.



### From concrete to  abstract (구체적인 것에서 추상적인 것으로)

모든 좋은 질문은 구체적인 기반으로 시작하고, 이 구조를 분명하게 만드는 우리의 사명은 정확히 같은 방식으로 시작해야 한다: 물론 정의로!

한 **abstract syntax tree** (보통 ***AST***로 언급이 되는)는 한 parse tree의 simplified, condensed version 일 뿐이다. 컴파일러 설계의 맥락에서, "AST" 용어는 **syntax tree**와 상호 교환하여 상요된다.

우리는 종종 parse tree 반대로서 syntax trees를 생각한다 (그리고 그것들이 어떻게 생성되는지를). 그리고 parse tree는 우리가 이미 [친숙한 것](https://medium.com/basecs/grammatically-rooting-oneself-with-parse-trees-ec9daeda7dad)이다. 우리는 *parse trees*가 우리의 코드의 문법적 구조를 포함하는 트리 자료구조라는 것을 안다; 다시 말해서, 그것들은 code "sentence"에서 나타나는 모든 syntatic information을 포함하고, 프로그래밍 언어 그 자체의 문법으로 부터 파생된다.

반면에, *abstract syntax tree*는 parse tree가 그렇지 않으면 포함할 syntatic information의 많은 양을 무시한다.

이 구분은 만약 우리가 AST의 "abstractness(추상)"에 집중한다면 좀 더 많이 이해가 되기 시작한다.

우리는 **parse tree**가 한 문장의 그려지고 사진화된 버전이라고 기억할 것이다. 다시 말해서, 우리는 parse tree가 정확히 한 expression, sentence, 또는 text가 어떻게 생겼는지를 나타낸다고 말할 수 있다. 그것은 기본적으로 텍스트 그 자체로의 직접적인 번역이다; 우리는 sentence를 가져와서, 그것의 모든 작은 조각들을 - 구두점에서 expressions로 그리고 tokens까지 - 트리 자료구조로 바꾼다. 그것은 한 text의 concrete syntax를 보여주는데, 이것이 그것이 또한 **concrete syntax tree** 또는 **CST**라고 지칭되는 이유이다. 우리는 이 구조를 묘사하기 위해 concrete라는 용어를 사용하는데, 그것이 tree format에서 토큰 별로 우리의 코드의 grammatical copy이기 때문이다.

그러나 무엇이 concrete vs abstract하게 만드는 것인가? 음, abstract syntax tree는 한 expression이 무엇처럼 보이는지를 parse tree가 하는 것처럼 *정확하게* 보여주지 않는다.

오히려, abstract syntax tree는 우리에게 "중요한" 것을 보여준다 - 우리가 정말 신경 쓰는 것들, 그리고 이것은 우리의 코드 "sentence"에 의미를 준다. Syntax tree는 우리에게 한 expression의 중요한 조각들을 또는 우리의 source text의 *abstracted syntax*를 준다. 그러므로, concrete syntax trees와 비교하여, 이러한 구조들은 우리의 코드의 추상화된 표현이다 (어떤 면에서, 덜 정확한), 그리고 정확히 그것들이 그것들의 이름을 얻은 방법이다.

우리가 이러한 두 개의 자료구조들의 구분을 이해하고, 그것들이 우리의 코드를 나타낼 수 있는 다른 방식들을 이해했으니, 질문할 가치가 있다 : abstract syntax tree는 컴파일러에 어디에 맞는가? 처음에, 우리 자신에게, 우리가 지금까지 아는 컴파일 프로세스에 대한 모든 것을 떠올려보자.

우리가 이렇게 보이는 `5 + (1 x 12)`의 매우 짧고 좋은 source text를 가지고 있다고 하자.

우리는 컴파일 프로세스에서 발생하는 첫 번째 것이 *scanner*에 의해 수행되는 일인 text를 *scanning*하는 ㅓㄳ이라느 ㄴ것으 ㄹ안다. 이것은 그 text가 그것의 가장 작은 가능한 부분들로 분해하도록 할 것이고, 그 가장 작은 조각들은 *lexemes*라고 불려진다. 이 부분은 language agnostic하게 될 것이고, 우리는 우리의 source text를 stripped-out version을 가지게 될 것이ㅏㄷ.

다음으로, 이러한 lexemes는 lexer/tokenizer

에게 넘겨지고, 우리의 source text의 작은 representations를 *tokens*으로 바꿀 것이다. 그리고 이것은 우리의 언어에 특정하게 될 것이다. 우리의 tokens은 이것ㅊ럼 보일 것이다 : `[5, +, (, 1, x, 12, )]`. scanner와 tokenizer가 함께하는 노력은 컴파일의 *lexical analysis*를 구성한다.

그러고나서, 우리의 input이 tokenized 되었다면, 그것의 최종 tokens들은 우리의 parser에게 넘겨지고, 그러고나서 source text를 가져와서 그것에서 parse tree를 구성한다. 아래의 그림은 parse tree format에서 우리의 tokenized code가 어떻게 보이는지를 예제화한다.

 tokens을 parse tree로 바꾸는 작업은 또한 parsing이라고 불려지고, *syntax analysis* 단계라고 알려져있다. 그 syntax analysis 단계는 직접적으로 lexical analysis phase에 의존한다; 따라서, lexical analysis는 항상 컴파일 프로세스에서 처음에 와야 한다. 왜냐하면 우리의 컴파일러의 parser가 tokenizer가 그것의 일을 하고 나서야 일을 할 수 있기 때문이다!

그러나 우리의 원래 질문으로 돌아가서 : 그 abstract syntax tree는 이 friend group의 어디에 맞는가? 음, 그 질문에 답하기 위해서, 처음 장소에서 AST의 *필요성*을 이해하는 것이 도움이 된다.



### Condensing one tree into another

좋아, 그래서 이제 우리는 우리의 머리에서 간단히 할 두 개의 트리들을 가지고 있다. 우리는 이미 parse tree를 가지고 있었고, 아직 배울 또 다른 자료구조가 있다! 명백히, 이 AST 자료구조는 simplified parse tree이다. 그래서, 왜 우리가 그것을 필요하는 가? 그것의 요점은 무엇인가?

음, 우리의 parse tree를 봐볼까?

우리는 parse trees가 그것의 가장 구분되는 부분들에서 윌의 프로그램을 나타낸다는 것을 안다; 정말로, 이것은 그 scanner와 tokenizer가 우리의 expression을 그것의 가장 작은 부분들로 분해하는 중요한 잡을 가지고 있는 이유이다.

밝혀지듯이, 가끔씩, 한 프로그램의 모든 다른 부분들은 실제로 항상 우리에게 모두 유용한 것은 아니다.

위에서 보여진 그림을 봐보자! 이것은 우리의 parse tree format에 있는 `5 + (1 x 12)`인 원래 식을 나타낸다. 만약 우리가 비판적인 눈으로 이 트리를 가까이 본다면, 한 노드가 정확히 한 개의 자식을 갖는 몇 가지 예시들이 있다는 것을 볼 것이다. 그리고 이것은 또한 **single-successor nodes**라고 불려진다. 왜냐하면 그것들은 오직 그것들로부터 기원하는 한 개의 자식 노드만을 갖기 때문이다. (하나의 "successor").

우리의 parse tree example의 경우에, single-successor nodes는 `Expression`, 즉 `Exp`의 부모 노드를 갖는다. 그리고 이것은 `5`, `1`, 또는 `12`같은 어떤 값의 single successor이다. 그러나, 그 `Exp` parent nodes는 여기에서 실제로 우리에게 어떤 것을 더하고 있지 않다. 그렇지 않은가? 우리는 그것들이 token/terminal child nodes를 포함한다는 것을 알 수 있지만, 우리는 정말로 그 "expression" arent node를 신경쓰지 않는다. 우리가 정말 알기 원하는 것은 그 expression이 무엇인가 이다.

우리가 우리의 tree를 파싱하고 나서, 그 부모 노드는 우리에게 어떠한 추가정보를 즉시 주지 않는다. 대신에, 우리가 정말 걱정하는 것은 단일 *child*이다, 즉 single-successor node. 정말로, 그것은 우리에게 중요한 정보를 주는 노드이다. 우리에게 중요한 부분은 : 그 숫자와 그 값 그 자체이다! 이러한 부모 노드들이 우리에게 어느정도 불핑료하다는 사실을 고려하여, 이 parse tree가 조금 verbose 하다는 것이 명백해진다.

모든 이러한 single-successor nodes는 우리에게 너무 많다. 그리고 우리에게 전혀 도움 되지 않는다. 그래서 그것을 없애보자!

만약 우리가 우리의 parse tree에서 single-successor nodes를 압축한다면, 우리는 정확히 같은 구조의 좀 더 압축된 버전을 갖게 될 것이다. 위의 그림을 보면, 우리는 우리가 여전히 이전과 정확히 같은 nesting을 유지하고 있고, 우리의 nodes/tokens/terminals이 여전히 그 트리내에서 정확한 위치에 나타난다는 것을 알게 될 것이다. 그러나, 우리는 그것을 간신히 조금 줄였다.

그리고 우리는 우리의 트리를 좀 더 다듬을 수 있다. 예를들어, 만약 우리가 지금 보이듯이 우리의 parse tree를 본다면, 우리는 그것 내에 mirror structure가 있다는 것을 알게 될 것이다. `(1 x 12)`의 sub-expression은 괄호 `()`로 nested 된다. 그리고 이것은 그들 스스로 tokens이다.

그러나, 이러한 괄호들은 우리가 트리를 잘 위치시키면 우리에게 도움이 되지 않는다. 우리는 이미 `1`과 `12`가 곱셈 `x` 연산에 보내질 인자라는 것을 안다. 그래서 그 괄호는 이 시점에서 많은 것을 말하지 않는다. 사실, 우리는 우리의 parse tree를 더 압축할 수 있고, 이러한 넘치는 leaf nodes를 제거할 수 있다.

일단 우리가 우리의 parse tree를 압축하고 simplify를 하고, 넘치는 syntatic "dust"를 제거하고 나면, 우리는 이 시점에서 다르게 보이는 구조를 갖게 된다. 그 구조는 사실, 우리의 새로운 많이 예상되는 친구인 abstract syntax tree이다.

위의 임지는 우리의 parse tree와 정확히 같은 expression을 보여준다 : `5 + (1 x 12)`. 그 차이는, 그것은 concrete syntax로부터 expression을 추상화했다. 우리는 이 트리에서 괄호 `()`의 어떠한 것도 보지 않는다. 왜냐하면 그것들은 필요하지 않기 때문이다. 유사하게, 우리는 `Exp`같은 non-terminals를 보지 않는다. 왜냐하면 우리는 이미 그 "expression"이 무엇인지 알았기 때문이다. 그리고 우리는 우리에게 정말 중요한 *value*를 가져올 수 있다 - 예를들어, 숫자 `5`.

이것은 정확히 AST와 CST사이의 구분되는 요소이다. 우리는 abstract syntax tree가 parse tree가 포함하는 syntatic information의 많은 양을 무시하고, parsing에 사용되는 "extra content"를 생략한다는 것을 안다. 그러나 지금, 우리는 그것이 어떻게 발생하는지를 볼 수 있다!

우리는 우리 자신의 parse tree를 압축했으니, 우리는 CST에서 AST를 구분짓는 패턴을 골라는데 더 잘할 수 있을 것이다.

abstract syntax tree가 시각적으로 parse tree와 다른 몇 가지 면들이 있다:

1. AST는 결코 commas, 괄호, 세미콜론 같은 syntatic details (물론 언어에 따라) 를 포함하지 않을 것이다.
2. AST는 그렇지 않으면 single-successor nodes로 나타날 버전들을 없앤다; 그것은 결코 single child의 nodes의 chains를 포함하지 않을 것이다.
3. 마지막으로, 어떤 연산자 토큰들 (`+`, `-`, `x`, `/` 같은)은 그 트리에서 internal (parent) node가 될 것이다. parse tree에서 종료되는 leaves 라기 보단.

시각적으로, AST는 항상 parse tree보다 더 compact해 보일 것이다. 왜냐하면 정의대로 덜 syntatic detail를 가진 parse tree의 압축된 버전이기 때문이다. 

이것은 정말로 abstract syntax tree가 더 큰 컴파일에 들어맞는 방법이다. AST는 우리가 이미 배운 [the parse trees](https://medium.com/basecs/grammatically-rooting-oneself-with-parse-trees-ec9daeda7dad)에 대한 직접적인 연결성을 가지고 있다. 반면에 그것은 AST가 생성되기 전에 그것의 일을 하는 [the lexer](https://medium.com/basecs/reading-code-right-with-some-help-from-the-lexer-63d0be3d21d) 에 동시에 의존하고 있다.

그 abstract syntax tree는 syntax analysis phase의 최종 결과로서 만들어진다. syntax analysis동안 main "character"로서 앞에 있고 중심이 되는 parser는 항상 parse tree 또는 CST를 생성할지도 또는 생성하지 않을지도 모른다. 컴파일러 그 자체와, 그것이 설계되는 방법에 따라, 그 parser는 syntax tree 즉 AST를 직접적으로 곧바로 만들지도 모른다. 그러나, 그 parser는 항상 그것의 output으로서 AST를 생성할 것이다. 그것이 중간에 parser tree를 만들든 말든, 또는 그것이 그렇게 하기 위해 얼마나 많은 pass들이 필요한지와 상관없이



### Anatomy of an AST

우리는 abstract syntax tree가 중요하다는 것을 (반드시 위협적이지 않다!) 알았기 때문에, 우리는 그것을 조금 더 분해하기 시작할 수 있다. AST가 구성되는 방법에 대한 흥미로운 측면은 이 트리의 노드들과 관련이 있다.

아래의 이미지는 abstract syntax tree의 단일 노드의 anatomy를 예제화 한다.

우리는 이 node가 이전에 보았던 다른 것들과 유사하다는 것을 눈치 챌 것인데, 그것이 어떤 데이터 (`token`, 그리고 그것의 `value`)를 포함한다는 점에서. 그러나, 그것은 또한 매우 특정한 포인터들을 포함한다. AST에 있는 모든 노드는 그것의 *next* sibling node에 대한 참조를 포함한다. 뿐만 아니라 그것의 *first* child node또한.

예를들어, `5 + (1 x 12)`의 간단한 expression은 아래의 것 처럼 AST의 시각화된 그림으로 구성될 수 있다.

우린느 이 AST를 읽고, 탐색하고, "해석"하는 것이 그 트리의 bottom levels에서 시작할 수 있고, 한 값 또는 끝의 결과 `result`를 구성하기 위해 그것의 길을 반대로 구성하면서.

그것은 또한 우리의 시각화를 보충하는데 도움을 주기 위해 한 parser의 output의 coded version을 보는 것이 도움이 될 수 있다. 우리는 다양한 도구들에 의지할 수 있고, 우리의 expression이 한 parser를 통해 작동할 때 어떻게 보일지에 대한 빠른 예쩨들을 보기위해 미리 존재하는 parsers를 사용할지도 모른다. 우리의 source text에 대한 예제는 아래에 있고, ECMAScript parser인 [Esprima](http://esprima.org/)를 통해 작동되었다. 그리고 그것의 최종 abstract syntax tree가 있고, 그 다음에 그것의 별개의 토큰들의 목록이 있다.

```c++
// Using Esprima, a ECMAScript parser written in ECMAScript 
// var esprima = require('esprima');
// var program = 'const answer = 42';

// Syntax
{
  "type": "Program",
  "body": [
    {
      "type": "VariableDeclaration",
      "declarations": [
          {
              "type": "VariableDeclarator",
              "id": {
                  "type": "Identifier",
                  "name": "result"
              },
              "init": {
                  "type": "BinaryExpression",
                  "operator": "+",
                  "left": {
                      "type": "Literal",
                      "value": 5,
                      "raw": "5"
                  },
                  "right": {
                      "type": "BinaryExpression",
                      "operator": "*",
                      "left": {
                          "type": "Literal",
                          "value": 1,
                          "raw": "1"
                      },
                      "right": {
                          "type": "Literal",
                          "value": 12,
                          "raw": "12"
                      }
                  }
              }
          }
      ],
      "kind": "var"
    }
  ],
  "sourceType": "script"
}


// Token List
[
  {
    "type": "Keyword",
    "value": "var"
  },
  {
    "type": "Identifier",
    "value": "result"
  },
  {
    "type": "Punctuator",
    "value": "="
  },
  {
    "type": "Numeric",
    "value": "5"
  },
  {
    "type": "Punctuator",
    "value": "+"
  },
  {
    "type": "Punctuator",
    "value": "("
  },
  {
    "type": "Numeric",
    "value": "1"
  },
  {
    "type": "Punctuator",
    "value": "*"
  },
  {
    "type": "Numeric",
    "value": "12"
  },
  {
    "type": "Punctuator",
    "value": ")"
  },
  {
    "type": "Punctuator",
    "value": ";"
  }
]
```

이 포맷에서, 우리는 만약 그 nested objects들을 본다면, 그 tree의 neting을 볼 수 있다. 우리는 `1`과 `12`를 포함하는 값들이 `*`인 parent operation의 갭려적으로 `left`와 `right` children이라는 것을 볼 수 있다. 우리는 또한 그 곱셈 연산 (`*`)이 전체 expression 자체의 *right subtree*를 구성하는 것을 보게 될 것이고, 그리고 이것은 더 큰 `BinaryExpression` object 안에서 nested 되어있는 이유이다. `"right"`라는 키 아래에서. 유사하게, `5`의 값은 더 큰 `BinaryExpression` object의 단일 `"left"` child이다.

그러나, abstract syntax tree에서 가장 흥미로운 측면은 비록 그것들이 매우 compact하고 깔끔할지라도, 그것들은 항상 시도하고 구성하기에 쉬운 자료구조가 아니라는 사실이다. 사실, AST를 구성하는 것은 꽤 복잡할 수 있다. 그 parser가 다루고 있는 언어를 기반으로!

대부분의 parsers들은 보통 parser tree (CST를 구성하고나서 그것을 AST format으로 변경할 것이다. 왜냐하면 그것이 가끔식 더 쉽기 때문이다 - 비록 그것이 좀 더 많은 단계들을 의미할지라도, 그리고 일반적으로 source text를 통해 더 많은 passes들을 의미할지라도. CST를 구성하는 것은 그 parser가 그것이 parse하려고하는 언어의 문법을 안다고 하면 꽤 쉽다. 그것은 한 token이 "significant"한지 아닌지를 알아내는 복잡한 일을 할 필요가 없다; 대신에, 그것은 그것이 무엇을 보는지를 ㅈ어확히 그것이 본 정확한 순서대로 가져와서, 그것 모두를 tree로 내뱉어야 한다.

반면에, 이 모든 것을 single-step process로서 시도하려고 하는  parsers들이 있는데, 이것들은 abstract syntax tree로 바로 구성하느 ㄴ것을 넘어간다.

컴파일러 설계에서, AST는 한 가지 이유 이상 때문에 매우 중요하게 된다. 그렇다, 구성하기에 까다롭지만, lexical and syntax analysis phases가 혼합된 마지막 최종 결과이다! 그 lexical and syntax analysis phases는 종종 함께 compiler의 **analysis phase** 또는 **the front-end**라고 불려진다.

우리는 abstract syntax tree를 컴파일러의 front-end의 "최종 프로젝트"로서 생각할 수 있다. 그것은 가장 중요한 부부인데, 왜냐하면 그 front-end가 그 자체로 보여줘야 하는 마지막 것이기 때문이다. 이것에 대한 기술적 용어는 **intermediate code representation** 또는 **IR**이라고 불려진다. 왜냐하면 그것은 컴파일러에 의해 source text를 나타내기 위해 궁극적으로 사용되는 자료구조가 되기 때문이다.

abstract syntax tree는 IR의 가장 흔한 형태일뿐만 아니라, 가끔씩 가장 오해되는 것이다. 우리는 이것을 좀 더 이해했으니, 우리는 이 무서운 구조의 우리의 인식을 바꾸기 시작할 수 있다. 운 좋게도, 우리에 이제 조금 덜 위협적이다.

> 개인적으로 이 문서들을 번역하면서 C++ 창시자의 책인 PPP C++의 Calculator를 만드는 경험이 나에게 엄청 중요한 것이라는 것을 느꼈다. 계산기를 만드는 그 파트는  위의 이러한 상세한 개념없이 이러한 것들을 어떻게 만드는지, 어떤 알고리즘을 써가는지를 알려주었다. 여기에서 말한 어려운 CST없이 AST만드는 과정을 C++ 처음 배우는 사람이 따라할 수 있게 튜토리얼을 쓴 것이다. 아직 다 끝내지 못한 PPP C++ 이지만 다시 시작해서 끝내도록 하고 싶다.

# Resources

There are a whole lot of resources out there on ASTs, in a variety of languages. Knowing where to start can be tricky, especially if you’re looking to learn more. Below are a few beginner-friendly resources that dive into a whole lot more detail without being *too* overhwelming. Happy asbtracting!

1. [The AST vs the Parse Tree](http://pages.cs.wisc.edu/~fischer/cs536.s06/course.hold/html/NOTES/4.SYNTAX-DIRECTED-TRANSLATION.html#astVsParse), Professor Charles N. Fischer
2. [What’s the difference between parse trees and abstract syntax trees?](https://stackoverflow.com/questions/5967888/whats-the-difference-between-parse-trees-and-abstract-syntax-trees), StackOverflow
3. [Abstract vs. Concrete Syntax Trees](https://eli.thegreenplace.net/2009/02/16/abstract-vs-concrete-syntax-trees/), Eli Bendersky
4. [Abstract Syntax Trees](http://www1.cs.columbia.edu/~sedwards/classes/2003/w4115f/ast.9up.pdf), Professor Stephen A. Edwards
5. [Abstract Syntax Trees & Top-Down Parsing](http://user.it.uu.se/~kostis/Teaching/KT1-11/Slides/lecture05.pdf), Professor Konstantinos (Kostis) Sagonas
6. [Lexical and Syntax Analysis of Programming Languages](http://www.pling.org.uk/cs/lsa.html), Chris Northwood
7. [AST Explorer,](https://astexplorer.net/) Felix Kling