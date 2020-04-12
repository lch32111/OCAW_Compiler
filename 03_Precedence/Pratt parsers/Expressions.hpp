#pragma once
#ifndef __EXPRESSIONS_HPP__
#define __EXPRESSIONS_HPP__

#include <string>
#include <vector>
#include "Token.hpp"

/**
 * Interface for all expression AST node classes
 */
class Expression
{
public:
	Expression() { }
	/*
	 * Pretty-print the expression to a string
	 */
	virtual void print(std::string& builder) = 0;
};

class AssignExpression : public Expression
{
public:
	AssignExpression(const std::string& name, Expression* right)
		: m_name(name)
		, m_right(right)
	{}

	void print(std::string& builder)
	{
		builder.append("(").append(m_name).append(" = ");
		m_right->print(builder);
		builder.append(")");
	}
private:
	std::string m_name;
	Expression* m_right;
};

class CallExpression : public Expression
{
public:
	CallExpression(Expression* function,std::vector<Expression*>& args)
		: m_function(function)
		, m_args(args)
	{}

	void print(std::string& builder)
	{
		m_function->print(builder);
		builder.append("(");

		for (size_t i = 0; i < m_args.size(); ++i)
		{
			m_args[i]->print(builder);

			if (i < m_args.size() - 1) builder.append(", ");
		}
		builder.append(")");
	}
private:
	Expression* m_function;
	std::vector<Expression*> m_args;
};

class ConditionalExpression : public Expression
{
public:
	ConditionalExpression
	(
		Expression* condition,
		Expression* thenArm,
		Expression* elseArm
	)
		: m_condition(condition)
		, m_thenArm(thenArm)
		, m_elseArm(elseArm)
	{}

	void print(std::string& builder)
	{
		builder.append("(");
		m_condition->print(builder);
		builder.append(" ? ");
		m_thenArm->print(builder);
		builder.append(" : ");
		m_elseArm->print(builder);
		builder.append(")");
	}

private:
	Expression* m_condition;
	Expression* m_thenArm;
	Expression* m_elseArm;
};

class NameExpression : public Expression
{
public:
	NameExpression(const std::string& name)
		: m_name(name)
	{}

	const std::string& getName() const { return m_name; }

	void print(std::string& builder)
	{
		builder.append(m_name);
	}
private:
	std::string m_name;
};

class OperatorExpression : public Expression
{
public:
	OperatorExpression(Expression* left, TokenType op, Expression* right)
		: m_left(left)
		, m_operator(op)
		, m_right(right)
	{}

	void print(std::string& builder)
	{
		builder.append("(");
		m_left->print(builder);
		builder.append(" ");
		char buf[4];
		buf[0] = punctuator(m_operator);
		buf[1] = ' ';
		buf[2] = '\0';
		builder.append(buf);
		m_right->print(builder);
		builder.append(")");
	}

private:
	Expression* m_left;
	TokenType m_operator;
	Expression* m_right;
};

class PostfixExpression : public Expression
{
public:
	PostfixExpression(Expression* left, TokenType op)
		: m_left(left)
		, m_operator(op)
	{}

	void print(std::string& builder)
	{
		builder.append("(");
		m_left->print(builder);
		char buf[4];
		buf[0] = punctuator(m_operator);
		buf[1] = ')';
		buf[2] = '\0';
		builder.append(buf);
	}

private:
	Expression* m_left;
	TokenType m_operator;
};

class PrefixExpression : public Expression
{
public:
	PrefixExpression(TokenType op, Expression* right)
		: m_operator(op)
		, m_right(right)
	{}

	void print(std::string& builder)
	{
		char buf[4];
		buf[0] = '(';
		buf[1] = punctuator(m_operator);
		buf[2] = '\0';
		builder.append(buf);
		m_right->print(builder);
		builder.append(")");
	}

private:
	TokenType m_operator;
	Expression* m_right;
};


#endif