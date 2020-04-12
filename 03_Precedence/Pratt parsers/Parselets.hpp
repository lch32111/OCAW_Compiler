#pragma once
#ifndef __PARSELETS_HPP__
#define __PARSELETS_HPP__

#include "Expressions.hpp"

class Parser;

/*
 * Pratt parser에 의해 사용되는 두 개의 parselet interfaces들 중 하나.
 * InfixParselet은 파싱하는 expression의 중간에 나타나는 token과 연관된다.
 * 그것의 parse() method는 left-hand side가 파싱된 후에 호출될 것이고,
 * 그것은 차례대로 그 token 이후에 오는 모든 것을 파싱하는데 책임이 있다.
 * 이것은 또한 postfix expressions에 대해 사용되는데, 이 경우에
 * 그것은 간단하게 그것의 parse() call에서 어떠한 tokens을 consume하지 않는다.
 */
class InfixParselet
{
public:
	virtual Expression* parse(Parser* parser, Expression* left, Token* token) = 0;
	virtual int getPrecedence() = 0;
};

/*
 * Pratt parser에 의해 사용되는 두 개의 interfaces 중 하나. PrefixParselet은
 * 한 식의 초기에 나타나는 토큰과 연관된다. 그것의 parse() method는
 * consumed leadin token에 의해서 호출될 것이고, 그 parselet은
 * 그 토큰 이후에 나오는 것을 파싱하는데 책임이 있다.
 * 이 interface는 또한 변수들과 같은 single-token expressions에 사용되는데,
 * 그 경우에 parse()는 간단히 그 이상의 토큰들을 consume하지 않는다.
 */
class PrefixParselet
{
public:
	virtual Expression* parse(Parser* parser, Token* token) = 0;
};

/*
 * "a = b"같은 assignment expressions을 파싱한다. assignment expression의 좌변은
 * "a"와 같이 간단한 이름이여야 하고, expressions은 right-associatve 이다.
 * (다시 말해서, "a = b = c"가 "a = (b = c)로 파싱된다).
 */
class AssignParselet : virtual public InfixParselet
{
public:
	AssignParselet() { }
	Expression* parse(Parser* parser, Expression* left, Token* token);
	int getPrecedence();
};

/*
 * binary arithmetic operator를 위한 Generic infix parselet. 
 * parsing할 때의 유일한 차이는 "+", "-", "*", "/", 그리고 "^"가 precedence이고
 * associativity라는 것이다. 그래서 우리는 그러한 모든 것에 대해
 * single parselet class를 사용할 수 있다.
 */
class BinaryOperatorParselet : virtual public InfixParselet
{
public:
	BinaryOperatorParselet(int precedence, bool isRight)
		: m_precedence(precedence)
		, m_isRight(isRight)
	{}

	Expression* parse(Parser* parser, Expression* left, Token* token);
	int getPrecedence();
private:
	int m_precedence;
	bool m_isRight;
};

/*
 * "a(b, c, d)"같은 함수 호출을 파싱하는 parselet
 */
class CallParselet : virtual public InfixParselet
{
public:
	CallParselet() { }
	Expression* parse(Parser* parser, Expression* left, Token* token);
	int getPrecedence();
};

/*
 * condtion 또는 "a ? b : c"같은 "ternary" operator를 위한 Parselet
 */
class ConditionalParselet : virtual public InfixParselet
{
public:
	ConditionalParselet() { }
	Expression* parse(Parser* parser, Expression* left, Token* token);
	int getPrecedence();
};

/*
 * unary arithmetic operator를 위한 Generic infix parselet.
 * postfix unary "?" expression를 파싱한다.
 */
class PostfixOperatorparselet : virtual public InfixParselet
{
public:
	PostfixOperatorparselet(int precedence)
		: m_precedence(precedence)
	{}

	Expression* parse(Parser* parser, Expression* left, Token* token);
	int getPrecedence();

private:
	int m_precedence;
};

/*
 * "a * (b + c)" 같은 expression을 그룹화하는데 사용되는 괄호를 파싱
 */
class GroupParselet :  virtual public PrefixParselet
{
public:
	GroupParselet() { }
	Expression* parse(Parser* parser, Token* token);
};

/*
 * "abc"같은 이름있는 변수를 위한 Simple parselet
 */
class NameParselet :  virtual public PrefixParselet
{
public:
	NameParselet() { }
	Expression* parse(Parser* parser, Token* token);
};

/*
 * unary arithmetic operator를 위한 Generic prefix parselet.
 * prefix unary "-", "+", "~", 그리고 "!" expressions를 파싱한다.
 */
class PrefixOperatorParselet :  virtual public PrefixParselet
{
public:
	PrefixOperatorParselet(int precedence)
		: m_precedence(precedence)
	{}

	Expression* parse(Parser* parser, Token* token);
	int getPrecedence();
private:
	int m_precedence;
};

#endif