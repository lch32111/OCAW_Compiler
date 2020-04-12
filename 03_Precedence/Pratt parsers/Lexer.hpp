#pragma once
#ifndef __LEXER_HPP__
#define __LEXER_HPP__

#include <cctype>
#include <unordered_map>

#include "Token.hpp"

/*
 * �ſ� �������� lexer. string�� ��������, �װ��� �Ϸ��� Tokens��� �����Ѵ�.
 * Operators�� punctuation�� unique keywords�� ���εȴ�.
 * �Ϸ��� ���ڵ��� �� �� �ִ� Names�� NAME tokens���� �ٲ��.
 * ��� �ٸ� ���ڵ��� ���õȴ� (separte names�� �����ϰ�).
 * Numbers�� strings�� �������� �ʴ´�.
 * �̰��� parser���� �۵��ϴ� ���𰡸� �ֱ����� �ּ����� ���̴�.
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

		// �츮�� string�� ���� ���������Ƿ�, EOF tokens�� ��ȯ�Ѵ�.
		// �츮�� ��û���� ��ŭ ���� �װ͵��� ����ؼ� ��ȯ�� ���ε�,
		// �̰��� �� parser�� lookahead�� tokens�� ������ ����
		// �������� �ʵ��� �ϱ� ���ؼ��̴�.
		return Token(TokenType::END_OF_FILE, "");
	}

private:
	std::unordered_map<char, TokenType> m_punctuators;
	std::string m_text;
	int m_index = 0;
};

#endif