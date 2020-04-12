#pragma once
#ifndef __TOKEN_HPP__
#define __TOKEN_HPP__

enum TokenType
{
	LEFT_PAREN,
	RIGHT_PAREN,
	COMMA,
	ASSIGN,
	PLUS,
	MINUS,
	ASTERISK,
	SLASH,
	CARET,
	TILDE,
	BANG,
	QUESTION,
	COLON,
	NAME,
	END_OF_FILE
};

static inline char punctuator(const TokenType& token)
{
	switch (token)
	{
	case LEFT_PAREN:  return '(';
	case RIGHT_PAREN: return ')';
	case COMMA:       return ',';
	case ASSIGN:      return '=';
	case PLUS:        return '+';
	case MINUS:       return '-';
	case ASTERISK:    return '*';
	case SLASH:       return '/';
	case CARET:       return '^';
	case TILDE:       return '~';
	case BANG:        return '!';
	case QUESTION:    return '?';
	case COLON:       return ':';
	default:          return NULL;
	}
}

/*
 * simple token class. 이것들은 Lexer에 의해 생성되고,
 * Parsre에 의해 consumed 된다.
 */

class Token
{
public:
	Token(TokenType type, const std::string& text)
		: m_type(type)
		, m_text(text)
	{}

	Token(TokenType type, const char ch)
		: m_type(type)
	{
		m_text = ch;
	}

	TokenType getType() const { return m_type; }
	std::string getText() { return m_text; }
	const std::string& getText() const { return m_text; }


private:
	TokenType m_type;
	std::string m_text;
};

#endif