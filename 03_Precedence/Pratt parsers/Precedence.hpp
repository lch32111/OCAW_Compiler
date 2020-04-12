#pragma once
#ifndef __PRECEDENCE_HPP__
#define __PRECEDENCE_HPP__

/*
 * infix parsers에 의해 사용되는 다른 precedence levels를 정의한다.
 * 이러한 것들은 연속된 infix expressions이 어떻게 그룹화되는지를 결정한다.
 * 예를들어, "a + b * c - d"는 "(a + (b * c)) - d"로서 파싱될 것인데, 왜냐하면
 * "*"가 "+"와 "-" 보다 더 높은 precedence를 가지고 있기 때문이다.
 * 여기에서 더 높은 숫자는 더 높은 precedence를 의미한다.
 */

class Precedence
{
public:
	static const int ASSIGNMENT = 1;
	static const int CONDITIONAL = 2;
	static const int SUM = 3;
	static const int PRODUCT = 4;
	static const int EXPONENT = 5;
	static const int PREFIX = 6;
	static const int POSTFIX = 7;
	static const int CALL = 8;
};

#endif