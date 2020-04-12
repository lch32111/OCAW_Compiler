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
	// "^" ���� right-associative operators�� ó���ϱ� ����, �츮��
	// right-hand side�� �Ľ��� ��, �ټ� �� ���� precedence�� ����Ѵ�.
	// �̰��� ���� �켱������ ���� parselet�� right���� ��Ÿ������ �ϰ�,
	// �׷�����, *��* parselet�� ����� �װ��� left-hand argument�� ���� ���̴�.
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
	// "^"���� right-associative operators�� ó���ϱ� ����,
	// �츮�� right-hand side�� �Ľ��� ��, �ټ� �� ���� precedence�� ����Ѵ�.
	// �̰��� right�� ���� precedence�� ���� parselet�� ��Ÿ������ �ϰ�,
	// �׷����� *��* parselet�� ����� �װ��� left-hand argument�� �ǵ��� �Ѵ�.
	Expression* right = parser->parseExpression(m_precedence);

	return new PrefixExpression(token->getType(), right);
}

int PrefixOperatorParselet::getPrecedence()
{
	return m_precedence;
}