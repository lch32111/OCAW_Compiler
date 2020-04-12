#pragma once
#ifndef __PARSER_HPP__
#define __PARSER_HPP__

#include <deque>
#include <unordered_map>

#include "Token.hpp"
#include "Parselets.hpp"
#include "Lexer.hpp"

class Parser
{
public:
	Parser(Lexer* lexer)
	{
		do
		{
			Token t = lexer->next();
			m_tokens.push_back(new Token(t.getType(), t.getText()));
			if (t.getType() == TokenType::END_OF_FILE)
				break;
		} while (true);
	}

	~Parser()
	{
		for (size_t i = 0; i < m_tokens.size(); ++i)
			delete m_tokens[i];

		for (auto it = m_prefixParselets.begin(); it != m_prefixParselets.end(); ++it)
		{
			delete (*it).second;
		}

		for (auto it = m_infixParselets.begin(); it != m_infixParselets.end(); ++it)
		{
			delete (*it).second;
		}
	}

	void registerParselet(const TokenType& token, PrefixParselet* parselet)
	{
		m_prefixParselets[token] = parselet;
	}

	void registerParselet(const TokenType& token, InfixParselet* parselet)
	{
		m_infixParselets[token] = parselet;
	}

	Expression* parseExpression(int precedence)
	{
		Token* t = consume();

		auto& prefixIt = m_prefixParselets.find(t->getType());
		if (prefixIt == m_prefixParselets.end())
		{
			throw std::runtime_error("Could not parse " + t->getText() + ".");
		}

		Expression* left = (*prefixIt).second->parse(this, t);

		while (precedence < getPrecedence())
		{
			Token* token = consume();

			auto& infixIt = m_infixParselets.find(token->getType());
			left = (*infixIt).second->parse(this, left, token);
		}

		return left;
	}

	Expression* parseExpression()
	{
		return parseExpression(0);
	}

	bool match(TokenType expected)
	{
		Token* token = lookAhead(0);
		if (token->getType() != expected)
			return false;

		consume();
		return true;
	}

	Token* consume(TokenType expected)
	{
		Token* token = lookAhead(0);
		if (token->getType() != expected)
		{
			std::string err = "Expected token " + punctuator(expected);
			err += " and found ";
			err += punctuator(token->getType());
			throw std::runtime_error(err);
		}

		return consume();
	}

	Token*consume()
	{
		lookAhead(0);
		
		Token* t = m_read.front();
		m_read.pop_front();

		return t;
	}

private:
	Token* lookAhead(int distance)
	{
		while ((int)m_read.size() <= distance && m_tokenIndex < (int) m_tokens.size())
		{
			m_read.push_back(m_tokens[m_tokenIndex]);
			++m_tokenIndex;
		}

		if (m_tokenIndex == m_tokens.size() - 1)
		{
			return m_tokens[m_tokenIndex - 1];
		}

		return m_read[distance];
	}

	int getPrecedence()
	{
		auto infixIt = m_infixParselets.find(lookAhead(0)->getType());
		if (infixIt != m_infixParselets.end())
			return (*infixIt).second->getPrecedence();

		return 0;
	}

	int m_tokenIndex = 0;
	std::vector<Token*> m_tokens;
	std::deque<Token*> m_read;
	std::unordered_map<TokenType, PrefixParselet*> m_prefixParselets;
	std::unordered_map<TokenType, InfixParselet*> m_infixParselets;
};

#endif