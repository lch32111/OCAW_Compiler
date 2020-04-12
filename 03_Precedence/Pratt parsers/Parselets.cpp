#include "Parselets.hpp"
#include "Parser.hpp"
#include "Precedence.hpp"

template<typename Base, typename Derived>
inline bool instanceof(Base* ptr)
{
	return dynamic_cast<Derived*>(ptr) != nullptr;
}

Expression* AssignParselet::parse(Parser* parser, Expression* left, Token* token)
{
	Expression* right = parser->parseExpression(Precedence::ASSIGNMENT - 1);

	if (instanceof<Expression, NameExpression>(left) == false)
	{
		throw std::runtime_error("The left-hand side of an assignment must be a name.");
	}

	return new AssignExpression(((NameExpression*)left)->getName(), right);
}

int AssignParselet::getPrecedence()
{
	return Precedence::ASSIGNMENT;
}

Expression* BinaryOperatorParselet::parse(Parser* parser, Expression* left, Token* token)
{
	// https://en.wikipedia.org/wiki/Operator_associativity
	// "^" 같은 right-associative operators를 처리하기 위해, 우리는
	// right-hand side를 파싱할 때, 다소 더 낮은 precedence를 허용한다.
	// 이것은 같은 우선순위를 가진 parselet이 right에서 나타나도록 하고,
	// 그러고나서, *이* parselet의 결과를 그것의 left-hand argument로 취할 것이다.
	Expression* right = parser->parseExpression(m_precedence - (m_isRight ? 1 : 0));

	return new OperatorExpression(left, token->getType(), right);
}

int BinaryOperatorParselet::getPrecedence()
{
	return m_precedence;
}

Expression* CallParselet::parse(Parser* parser, Expression* left, Token* token)
{
	// Parse the comma-separated arguments until we hit, ")".
	std::vector<Expression*> args;

	// There may be no arguments at all
	if (false == parser->match(TokenType::RIGHT_PAREN))
	{
		do
		{
			args.push_back(parser->parseExpression());
		} while (parser->match(TokenType::COMMA));

		parser->consume(TokenType::RIGHT_PAREN);
	}

	return new CallExpression(left, args);
}

int CallParselet::getPrecedence()
{
	return Precedence::CALL;
}

Expression* ConditionalParselet::parse(Parser* parser, Expression* left, Token* token)
{
	Expression* thenArm = parser->parseExpression();
	parser->consume(TokenType::COLON);
	Expression* elseArm = parser->parseExpression(Precedence::CONDITIONAL - 1);

	return new ConditionalExpression(left, thenArm, elseArm);
}

int ConditionalParselet::getPrecedence()
{
	return Precedence::CONDITIONAL;
}

Expression* GroupParselet::parse(Parser* parser, Token* token)
{
	Expression* expression = parser->parseExpression();
	parser->consume(TokenType::RIGHT_PAREN);
	return expression;
}

Expression* NameParselet::parse(Parser* parser, Token* token)
{
	return new NameExpression(token->getText());
}

Expression* PostfixOperatorparselet::parse(Parser* parser, Expression* left, Token* token)
{
	return new PostfixExpression(left, token->getType());
}

int PostfixOperatorparselet::getPrecedence()
{
	return m_precedence;
}

Expression* PrefixOperatorParselet::parse(Parser* parser, Token* token)
{
	// "^"같은 right-associative operators를 처리하기 위해,
	// 우리는 right-hand side를 파싱할 때, 다소 더 낮은 precedence를 허용한다.
	// 이것은 right에 같은 precedence를 가진 parselet이 나타나도록 하고,
	// 그러고나서 *이* parselet의 결과가 그것의 left-hand argument가 되도록 한다.
	Expression* right = parser->parseExpression(m_precedence);

	return new PrefixExpression(token->getType(), right);
}

int PrefixOperatorParselet::getPrecedence()
{
	return m_precedence;
}