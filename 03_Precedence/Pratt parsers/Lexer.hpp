#pragma once
#ifndef __LEXER_HPP__
#define __LEXER_HPP__

#include <cctype>
#include <unordered_map>

#include "Token.hpp"

/*
 * 매우 원시적인 lexer. string을 가져오고, 그것을 일련의 Tokens들로 분할한다.
 * Operators와 punctuation는 unique keywords로 매핑된다.
 * 일련의 문자들이 될 수 있는 Names은 NAME tokens으로 바뀐다.
 * 모든 다른 문자들은 무시된다 (separte names는 제외하고).
 * Numbers와 strings은 지원되지 않는다.
 * 이것은 parser에게 작동하는 무언가를 주기위한 최소한의 것이다.
 */

class Lexer
{
public:
	Lexer(const std::string& text)
		: m_text(text)
		, m_index(0)
	{
		int num_token_type = (int)TokenType::END_OF_FILE;
		for (int i = 0; i < num_token_type; ++i)
		{
			const TokenType tt = (TokenType)i;
			char ch = punctuator(tt);

			if (ch != NULL)
			{
				m_punctuators[ch] = tt;
			}
		}
	}

	Token next()
	{
		while (m_index < (int)m_text.size())
		{
			char c = m_text[m_index++];

			if (m_punctuators.count(c) > 0)
			{
				// Handle punctuation
				return Token(m_punctuators[c], c);
			}
			else if (std::isalpha(static_cast<unsigned char>(c)))
			{
				// Handle names
				int start = m_index - 1;
				while (m_index < (int) m_text.size())
				{
					if (false == std::isalpha(static_cast<unsigned char>(m_text[m_index])))
						break;
					++m_index;
				}

				return Token(TokenType::NAME, m_text.substr(start, (m_index - start)));
			}
			else
			{
				// Ignore all other characters (whitespace, etc...
			}
		}

		// 우리는 string의 끝에 도달했으므로, EOF tokens을 반환한다.
		// 우리는 요청받은 만큼 많이 그것들을 계쏙해서 반환할 것인데,
		// 이것은 그 parser의 lookahead가 tokens이 부족한 것을
		// 걱정하지 않도록 하기 위해서이다.
		return Token(TokenType::END_OF_FILE, "");
	}

private:
	std::unordered_map<char, TokenType> m_punctuators;
	std::string m_text;
	int m_index = 0;
};

#endif