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

우리는 종종 parse tree 반대로서 syntax trees를 생각한다 (그리고 그것들이 어떻게 생성되는지를). 그리고 parse tree는 우리가 이미 [친숙한 것](https://medium.com/basecs/grammatically-rooting-oneself-with-parse-trees-ec9daeda7dad)이다.

> 친숙한 것은 Vaidehi Joshi가 작성한 Parse Trees에 관한 것이다. [여기에서](Grammatically Rooting Oneself With Parse Trees.md) 그것을 또한 번역했다.