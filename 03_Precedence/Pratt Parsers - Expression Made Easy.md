https://journal.stuffwithstuff.com/2011/03/19/pratt-parsers-expression-parsing-made-easy/



# Pratt Parsers : Expression Parsing Made Easy

때때로, 나는 매우 똑똑하고 한 문제에 대해 완벽해서, 내가 더 똑똑해지고 그것을 배움으로써 새로운 힘을 얻는 것처럼 느끼는 알고리즘 또는 아이디어를 우연히 만나게 된다. [Heaps](http://en.wikipedia.org/wiki/Heap_(data_structure)) (나의 짧은CS 교육에서 내가 얻은 유일한 것이다) 은 이것과 같은 한 가지 것이다. 나는 최근에 또 다른 것을 만나게 되었다: [Pratt](http://en.wikipedia.org/wiki/Vaughan_Pratt) parsers.

너가 한 parser를 작성할 때, [recursive descent](http://en.wikipedia.org/wiki/Recursive_descent)는 땅콩 버터를 펴는 것 만큼 쉽다. 그것은 너가 파싱하는코드의 다음 청크를 기반으로 다음에 무엇을 하려고 할 지를 알아낼 수 있을때 뛰어나다. 그것은 보통 classes같은 것들이 있는 한 언어의 top level에 대해 해당되고, 그것들을 uniquely하게 인식하는 어떤 것 (`if`, `for`, `whilte`, 등)으로 시작한 이후의 statements에 대해서 또한 해당된다.

그러나, 너가 expressions 에 도달할 때 까다로워진다. `+`같은 중위 연산자, `++` 같은 후위연산자, 그리고 심지어 `?:` 같은 혼합연산자에 대해, 너가 그것에 중간에 도달할 때까지 너가 파싱하고 있는 식이 무슨 종류인지 알아내기 어려울 수 있따. 너는 recursive descent로 이것을 할 수 있지만, 그것은 하기 싫은 일이다. 너는 precedence의 각 level에 대해 별개의 함수들을 작성해야하고 (예를들어 JavaScript 그러한 것들 중 17개 가지고 있다.), 수동으로 연관성을 처리하고, 보기 어려울 때 까지, 많은 parsing code를 거쳐서 너의 문법을 더럽힌다.



### PB&J, The Secret Weapon

Pratt parsing은 그것을 해결한다. 만약 recursive descent가 땅콩 버터라면, Pratt parsing은 jelly이다. 너가 그 두 개를 함께 섞을 때, 너는 너가 그것에 던지는 어떤 문법이든 처리할 수 있는 간단하고, 간결하고, readable parser를 얻게 된다.

operator precedence와 infix expressions를 처리하기 위한 Pratt의 기법은 너무 간다하고 효과적이어서, 왜 거의 어느 누구도 그것에 대해 알지 못하는지는 의문이다. 70년대 이후, top down operator precedence parsers는 사라진 것처럼 보인다. Douglas Crockford의 [JSLint](http://www.jslint.com/) 는 [parse JavaScript](http://javascript.crockford.com/tdop/tdop.html)하기 위해 하나를 사용하지만, 그의 처리는 그것에 대한 the [very few](http://effbot.org/zone/simple-top-down-parsing.htm) remotely modern articles중의 하나이다.

내가 생각하기에, 문제의 일부는, Pratt의 용어는 이해하기 힘들고, Crockford의 글은 그 자체로 어두 컴컴하다. Pratt은 "null denominator"같은 용어들을 사용하고, Crockford는 core  idea를 모호하게 만드는 lexical scope를 추적하는 것과 같은 추가 적인것을 혼합한다.

이것은 내가 도달하게 된것이고, 나는 어떠한 혁멱정인 것을 하지 않을 것이다. 나는 top down operator precedence parsers 뒤에서 핵심 개념들을 얻으려고 노력할 것이고,  가능한 한 명확하게 그것들을 설명할 것이다. 나는 상황을 명확히 하기 위해 (바라건데) 어떤 용어들을 바꿀 것이다. 바라건데, 나는 어떠한 purist (순혈주의자) sensibilities 감각을 건드리지 않을 것이다. 나는 프로그래밍 언어의 vulgar Latin인 Java로 코딩할 것이다. 나는 너가 Java로 그것을 쓸 수 있는지를 알아 냈고, 너는 그것을 어떤 것이로든 쓸 수 있다.



### What We'll Be Making

나는 learn-by-doing person이다. 그리고 이것은 내가 또한 teach-by-doing하는 사람이라는 것을 의미한다. 그래서 Pratt parsers가 어떻게 작동하는지를 보여주기 위해, 우리는 [tiny little toy language called *Bantam*](https://github.com/munificent/bantam)를 위한 parser를 구성할 것이다. 그것은 Pratt parsing이 도움이되는 곳이지만, 그것의 유용성을 충분히 납득시키는 짖머에서부터 expressions을 가진다.

비록 그것이 간단할지라도, 그것은 완전한 전체 operators를 가지고 있따 : prefix (`+`, `-`, `~`, `!`), postfix (`!`), infix (`+`, `-`, `*`, `/`, `^`), 그리고 심지어 mixfix conditional operator (`?:`). 그것은 여러가지의 precedence levels를 가지고 있고, 둘 다 right and left associative operators를 가지고 있따. 그것은 또한 assignment, function calls 그리고 grouping을 위한 parentheses를 가지고 있따. 만약 우리가 이것을 parse할 수 있다면, 우리는 어떤 것이든 parse할 수 있다.



### What We'll Start With

우리가 신경쓰는 모든 것은 parsing이고, 그래서 우리는 tokenizing phase를 무시할 것이다. 나는 작동하는 [a crude lexer](https://github.com/munificent/bantam/blob/master/src/com/stuffwithstuff/bantam/Lexer.java)를 놓고, tokens들이 하늘이든 어디던지에서 떨어지는 중인 척을 할 것이다.

한 token은 type과 그것에 관련된 string이 있는 의미있는 code의 chunk이다. `a + b(c)`가 주어진다면, 그 tokens들은 이럴 것이다:

```
NAME "a"
PLUS "+"
NAME "b"
LEFT_PAREN "("
NAME "c"
RIGHT_PAREN ")"
```

마찬가지로, 우리는 이 코드를 *interpretting* or *compiling*하지 않을 것이다. 우리는 그것을 좋은 data strcuture로 parse하길 원한다. 우리의 목적을 위해서, 그것은 우리의 parse가 많은 `Token` objects들을 씹어먹고, `Expression`을 구현하는 어떤 클래스의 instance를 내뱉어야 한다는 것을 의미한다. 너에게 한 아이디어를 주기 위해, 여기에 [conditional expression](https://github.com/munificent/bantam/blob/master/src/com/stuffwithstuff/bantam/expressions/ConditionalExpression.java)를 위한 그 클래스의 간단화된 version이 있다:

```java
class ConditionalExpression implements Expression {
  public ConditionalExpression(
      Expression condition,
      Expression thenArm,
      Expression elseArm) {
    this.condition = condition;
    this.thenArm   = thenArm;
    this.elseArm   = elseArm;
  }

  public final Expression condition;
  public final Expression thenArm;
  public final Expression elseArm;
}
```

(너는 여기에서 Jva의 "please sign it in quadruplicate" level of bureaucracy를 좋아하게 된다. 내가 말한 것 처럼, 만약 너가 java로 이것을 할 수 있다면, 너는 그것을 어떤 언어에서든지 할 수 있다.)

우리는 간단한 `Parser` class로부터 시작하는 이것을 구성할 것이다. 이것은 token stream을 소유하고, 앞서 처리하고, 너가 앞서 본 단일 token으로 top-down recursive descent parser를 작성할 필요가 있는 기본 메소드를 제공할 것이다. 이것은 우리가 진행되게 하기에 충분하다. 만약 우리가 나중에 더 필요하다면, 그것을 확장하는 것은 쉽다.

parser를 스스로 만들어보자!



### First Things First

"완전한" Pratt parser가 꽤 tiny활지라도, 나는 해독하는게 조금 어렵다는 것을 알았다. [quicksort](http://en.wikipedia.org/wiki/Quicksort) 처럼, 그 구현은 기만적으로 간단하고 소량의 깊게 꼬아진 코드이다. 그것의 매듭을 풀기 위해, 우리는 한 번에 아주 작은 단계를 구성할 것이다.

parse할 가장 간단한 expressions은 prefix operators와 single-token 인 것들이다. 그러한 것들에 대햏, 그 현재 token은 우리에게 우리가 할 필요가 있는 모든 것을 말해준다. Bantam은 한 단일 token expression, named variables, 4개의 prefix operators: `+`, `-`, `~`, `!` 를 가지고 있다. parse할 수 있는 가장 간단한 가능한 코드는 다음일 것이다:

```java
xpression parseExpression() {
  if (match(TokenType.NAME))       // return NameExpression...
  else if (match(TokenType.PLUS))  // return prefix + operator...
  else if (match(TokenType.MINUS)) // return prefix - operator...
  else if (match(TokenType.TILDE)) // return prefix ~ operator...
  else if (match(TokenType.BANG))  // return prefix ! operator...
  else throw new ParseException();
}
```

그러나 그것은 조금 단순하다. 너가 볼 수 있듯이, 너는 다른 parsing behavior로 분기하기 위해 `TokenType`에서 switching하고 있다. `TokenTypes`의 `Map`을 만들어서 그것을 직접 parsing code의 chunk에 encode해보자. 우리는 이러한 chunks를 "parselets"라고 부를 것이다. 그래서 그것들은 이것을 구현한다:

```java
interface PrefixParselet
{
    Expression parse(Parser parser, Token token);
}
```

변수 이름을 파싱하는 이것의 구현은 :

```java
class NameParselet implements PrefixParselet
{
    public Expression parse(Parser parser, Token token)
    {
        return new NameExpression(token.getText());
    }
}
```

우리는 모든 prefix operators가 실제 operator token 그 자체에서 다르기 때문에, 그것들에 대한 단일 클래스를 사용할 수 있다.

```java
class PrefixOperatorParselet implements PrefixParselet
{
    public Expression parse(Parser parser, Token token)
    {
        Expression operand = parser.parseExpression();
        return new PrefixExpression(token.getType(), operand);
	}
}
```

너는 그것이 그 연산자 이후에 나타나는 operand를 파싱하는 `parseExpression()`으로 호출하는 것을 주목해야 할 것이다 (즉, `-a`에 있는 `a`를  parse하기 위해). 이 재귀는 `-+~!a`같은 nested operators를 신경쓴다.

`Parser`로 돌아가서, 그 chained `if` statements는 더 깔끔한 map으로 대체된다:

```java
class Parser
{
    public void register(TokenType token, PrefixParselet parselet)
    {
        mPrefixParselets.put(token, parselet);
	}
    
    public Expression parseExpression()
    {
        Token token = consume();
        PrefixParselet prefix = mPrefixParselets.get(token.getType());
        
        if (prefix == null) 
            throw new ParseException("Could not parse \"" + token.getText() + "\"."");
	
		return prefix.parse(this, token);
	}
                                     
	// Other stuff...
                                     
	private final Map<TokenType, PrefixParselet> mPrefixParselets = new HashMap<TokenType, PrefixParselet>();
}
```

우리가 이제까지 가진 문법을 정의하기 위해 (변수들과 4개의 prefix operators), 우리는 이 helper method를 만들 것이다:

```java
void prefix(TokenType token)
{
    register(token, new PrefixOperatorParselet());
}
```

그리고 이제, 우리는 다음과 같은 문법을 정의할 수 있다:

```java
register(Token.NAME, new NameParselet());
prefix(TokenType.PLUS);
prefix(TokenType.MINUS);
prefix(TokenType.TILDE);
prefix(TokenType.BANG);
```

이것은 몇 가지 강제적인 함수들에 대해 널리 퍼진 것들 대신에 우리의 문법이 좀 더 declarative하기 때문에, 이것은 이미 recursive descent parser에 대한 개선점이고, 그리고 우리는 한 장소에서 실제 문법을 모두 볼 수 있다. 더욱 좋은 것은, 우리가 새로운 parselets을 등록하여 그 문법을 확장할 수 있다는 것이다. 우리는 `Parser` class 그 자체를 바꿀 필요가 없다.

만약 우리가 *오직* prefix expressions만 가졌다면, 우리는 이제 끝났다. Alas, 우리는 그렇지 않다.



### Stuck In the Middle

우리가 지금까지 가지고 있는 것은 오직 만약 그 *첫 번째* token이 우리가 무슨 종류의 expression을 파싱하고 있는지를 말해준다면 작동하지만, 그것은 항상 그렇지 않다. `a + b` 같은 식으로, 우리는 우리가 `a`를 parse하고 `+`를 도달할 때까지 add expression을 가지고 있는지를 알지 못한다. 우리는 그것을 지원하기 위해 그 parser를 확장해야만 할 것이다.

운좋게도, 우리는 그것을 그렇게 할 좋은 장소에 있다. 우리의 현재 `parseExpression()` method는 어떤 nested prefix expressions을 포함한 완전한 prefix expression을 파싱할 것이고, 그러고나서 멈춘다. 그래서 만약 우리가 이것을 그것에 던진다면:

```java
-a + b
```

그것은 `-a`를 파싱할 것이고, `+`를 그대로 내버려 둘 것이다. 그것은 우리가 무슨 infix expression을 파싱하는지를 구분하기 위해 우리가 정확히 필요한 토큰이다. infix expression과 prefix 사이의 유일한 차이점은, 인자로서 가질 필요가 있는 infix operator 전의 또다른 expression이 있다는 것이다. 그것을 지원하는 parselet을 정의하자:

```java
interface InfixParselet
{
    Expression parse(Parse parser, Expression left, Token token);   
}
```

유일한 차이점은 그 `left` argument인데, 이것은 infix token에 우리가 도달하기전에 파싱했던 expression이다. 우리는 infix parselets의 또 다른 테이블을 소유하여 우리의 parse에 이것을 연결할 것이다.

prefix와 infix expressions에 대해 별개의 tables를 가지는 것은 중요한데, 왜냐하면 우리는 종종 단일 `TokenType`에 대해 prefix와 infix parselet 둘 다 가질 것이기 때문이다. 예를들어, `(`에 대한 prefix parselet은 `a * (b + c)`같은 식에서 grouping을 처리한다. 반면에, *infix* parselet은 `a(b)` 같은 함수 호출을 처리한다.

이제, 우리는 prefix expressiond을 parse한 후에, 우리는 그것을 포함하는 어떤 infix one에 넘긴다:

```java
class Parser
{
    public void register(TokenType token, InfixParselet parselet)
    {
        mInfixParselets.put(token, parselet);
    }
    
    public Expression parseExpression()
    {
        Token token = consume();
        PrefixParselet prefix = mPrefixParselets.get(token.getType());
        
        if(prefix == null) throw new ParseException(
        "Could not parse\"" + token.getText() + "\"."");
           
		Expression left = prefix.parse(this, token);
            
        token = lookAhead(0);
		InfixParselet infix = mInfixParselets.get(token.getType());
            
		// No infix expression at this point, so we're done.
		if (infix == null) return left;
		
		consume();
		return infix.parse(this, left, token);
	}
            
	private final Map<TokenType, InfixParselet> mInfixParselets =
      new HashMap<TokenType, InfixParselet>();
}
```

꽤 간단하다. 우리는 `+` 같은 binary arithmetic operators에 대해 infix parselet를 다음과 같은 것을 사용하여 구현할 수 있다:

```java
class BinaryOperatorParselet implements InfixParselet
{
    public Expression parse(Parser parser,
                           Expression left, Token token)
    {
        Expression rigght = parser.parseExpression();
        return new OperatorExpression(left, token.getType(), right);
    }
}
```

이것은 또한 postfix operators에 대해서도 작동한다. 나는 그것들을 "infix" parselets이라고 부를 것이지만, 그것들을 실제로 "anything but prefix"  (prefix가 결코 아니다). 만약 그 토큰 이전에 오는 어떤 식이 있다면, 그것은 infix parselet에 의해 처리될 것이고, 그것은 postfix expressions과 `?:`같은 mixfix expressions을 포함한다.

Postfix는 single-token prefix parselet만큼 간단하다: 그것은 `left` expression을 취하고 그것을 다른 식에 감싼다:

```java
class PostfixoperatorParselet implements InfixParselet
{
    public Expression parse(Parser parser, Expression left, TOken token)
    {
        return new PostfixExpression(left, token.getType());
    }
}
```

Mixfix 또한 쉽다. 그것은 꽤 많이 친숙한 recursive descent parser이다:

```java
class ConditionalParselet implements InfixParselet
{
    public Expression parse (Parser parser, Expression left, Token token)
    {
        Expression thenArm = parser.parseExpression();
        parser.consume(TokenType.COLON);
        Expression elseArm = parser.parseExpression();
        
        return new ConditionalExpression(left, thenArm, elseArm);
    }
}
```

이제 우리는 prefix, postfix, infix, 그리고  심지어 mixfix expressions을 파싱할 수 있다. 꽤 작은 양의 코드로, 우리는 `a + (b ? c! : -d).`같은 식을 파싱할 수 있다. 우리는 끝났다 그런가? 음... 거의.



### Excuse You, Aunt Sally

우리의 parser는 모든 이러한 것을 파싱할 수 있지만, 그것은 올바른 precedence와 associativity로 그것을 파싱하지 않는다. 만약 너가 `a - b - c`를 그것에 날린다면, 그것은 `a - (b - c)` 같이 파싱할 것이다. 이것은 옳지 않다.

그리고 이 마지막 단계는 Pratt parsers가 꽤 좋은 것에서 완전히 급진적인 것으로 변하는 곳이다. 우리는 두 가지 간단ㅎ한 변화를 만들 것이다. 우리는 *precedence*를 취하기 위해 `parseExpression()`를 확장할 것이다 - precedence는 그 호출에 의해 무슨 expressions이 파싱될 수 있는지를 말해주는 숫자이다. 만약 그것이 우리가 허용하는 것보다 더 낮은 precedence인 expression을 만난다면, 그것은 parsing하는 것을 그만두고, 그것이 지금까지 가진 것을 반환한다.

그것이 체크하게 만들기 위해, 우리는 어떤 주어진 infix expression의 precedence를 알 필요가 있다. 우리는 parselet이 그것을 명시하게 하여 할 것이다:

```java
public interface InfixParselet
{
    Expression parse(Parser parser, Expression left, Token token);
    int getPrecedence();
}
```

그것을 사용하여, 우리의 core expression parser는 이렇게 된다:

```java
public Expression parseExpression(int precedence)
{
    Token token = consume();
    PrefixParselet prefix = mPrefixParselets.get(token.getType());
    
    if (prefix == null) throw new ParseException(
    "Could not pare\"" + token.getText() + "\".");
    
    Expression left = prefix.parse(this, token);
    
    while(precedence < getPredence())
    {
        token = consume();
        
        InfixParselet infix = mInfixParselets.get(token.getType());
        left = infix.parse(this, left, token);
    }
    
    return left;
}
```

이것은 현재의 token의 precedence를 얻거나 만약 그것에 대한 infix aprselet이 없다면 default를 얻는 아주 작은 helper function에 의존한다:

```java
private int getPrecedence()
{
    InfixParselet parser = mInfixParselets.get(
    	lookAhead(0).getType());
    if(parser != null) return parser.getPrecedence();
    
    return 0;
}
```

그리고 이게 다이고, 이것을 사용하기 위해, 우리는 작은 precedence table를 설정할 것이다:

```java
public class Precedence
{
    public static final int ASSIGNMENT = 1;
    public static final int CONDITIONAL = 2;
    public static final int SUM = 3;
    public static final int PRODUCT = 4;
    public static final int EXPONENT = 5;
    public static final int PREFIX = 6;
    public static final int POSTFIX = 7;
    public static final int CALL = 8;
}
```

우리의 operators가 정확히 precedence를 처리하게 만들기 위해서, 그것들은 그것들이 `parseExpression()`을 재귀적으로 호출할 때, 그것에 적절한 값을 다시 넘길 것이다. 예를들어, 그 `+` operator를 처리하는  `BinaryOperatorParselet` instance는 그것이 그것의 right-hand operand를 파싱할 때, `Precedence.SUM`을 넘길 것이다.

Associativity는 너무 쉽다. 만약 한 infix parselet이 그것이 그것 자신의 `getPrecedence()` call에 대ㅐ 반환하는 *같은* precedence로 `parseExpression()`를 호출한다면, 너는 left associativity를 얻게 될 것이다. right-associativt가 되도록, 그것은 needs to pass in *one less* than that instead (?).



### Go Forth and Multiply

나는 이것을 사용하여  [parser for Magpie](https://github.com/munificent/magpie/blob/master/src/com/stuffwithstuff/magpie/parser/MagpieParser.java)를 다시 작성했고, 기적같이 작동한다. 나는 또한 이것을 기반으로 JavaScript parser에 작업했고, 그것은 또다시 훌륭하게 잘 맞는다.

나는 이것과 같은 parsing이 간단하고, 간결하고, 확장가능하고 (예를들어, Magpie는 runtime에서 너가 [let you extend its own syntax](http://journal.stuffwithstuff.com/2011/02/13/extending-syntax-from-within-a-language/) 하도록 이것을 사용한다 ), 그리고 읽기에 쉽다. 나는 어떤 다른 방식으로 parser를 작성하는 것을 상상할 수 없는 지점에 와 있다. 내가 이렇게 말할 것이라고 결코 생각하지 않았지만, parsers는 이제 쉽다.

너 자신을 위해 보려면,  [the complete program](http://github.com/munificent/bantam)를 보아라.