#pragma once
#ifndef __BANTAM_PARSER_HPP__
#define __BANTAM_PARSER_HPP__

#include "Parselets.hpp"
#include "Lexer.hpp"
#include "Precedence.hpp"
#include "Parser.hpp"

class BantamParser : public Parser
{
public:
	BantamParser(Lexer* lexer)
		: Parser(lexer)
		, m_lexer(lexer)
	{
		// Register all of the parselets for the grammar.

		// Register the ones that need special parselets.
		registerParselet(TokenType::NAME, new NameParselet());
		registerParselet(TokenType::ASSIGN, new  AssignParselet());
		registerParselet(TokenType::QUESTION, new ConditionalParselet());
		registerParselet(TokenType::LEFT_PAREN, new  GroupParselet());
		registerParselet(TokenType::LEFT_PAREN, new CallParselet());

		// Register the simple operator parselets.
		prefix(TokenType::PLUS, Precedence::PREFIX);
		prefix(TokenType::MINUS, Precedence::PREFIX);
		prefix(TokenType::TILDE, Precedence::PREFIX);
		prefix(TokenType::BANG, Precedence::PREFIX);

		// For kicks, we'll make "!" both prefix and postfix, kind of like ++.
		postfix(TokenType::BANG, Precedence::POSTFIX);

		infixLeft(TokenType::PLUS, Precedence::SUM);
		infixLeft(TokenType::MINUS, Precedence::SUM);
		infixLeft(TokenType::ASTERISK, Precedence::PRODUCT);
		infixLeft(TokenType::SLASH, Precedence::PRODUCT);
		infixRight(TokenType::CARET, Precedence::EXPONENT);
	}

	/**
	 * Registers a postfix unary operator parselet for the given token and
	 * Precedence::
	 */
	void postfix(TokenType token, int precedence) 
	{
		registerParselet(token, new  PostfixOperatorparselet(precedence));
	}

	/**
	 * Registers a prefix unary operator parselet for the given token and
	 * Precedence::
	 */
	void prefix(TokenType token, int precedence) 
	{
		registerParselet(token, new  PrefixOperatorParselet(precedence));
	}

	/**
	 * Registers a left-associative binary operator parselet for the given token
	 * and Precedence::
	 */
	void infixLeft(TokenType token, int precedence) 
	{
		registerParselet(token, new  BinaryOperatorParselet(precedence, false));
	}

	/**
	 * Registers a right-associative binary operator parselet for the given token
	 * and Precedence::
	 */
	void infixRight(TokenType token, int precedence) 
	{
		registerParselet(token, new  BinaryOperatorParselet(precedence, true));
	}

private:
	Lexer* m_lexer;
};

#endif