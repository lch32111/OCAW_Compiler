#pragma once
#ifndef __PRECEDENCE_HPP__
#define __PRECEDENCE_HPP__

/*
 * infix parsers�� ���� ���Ǵ� �ٸ� precedence levels�� �����Ѵ�.
 * �̷��� �͵��� ���ӵ� infix expressions�� ��� �׷�ȭ�Ǵ����� �����Ѵ�.
 * �������, "a + b * c - d"�� "(a + (b * c)) - d"�μ� �Ľ̵� ���ε�, �ֳ��ϸ�
 * "*"�� "+"�� "-" ���� �� ���� precedence�� ������ �ֱ� �����̴�.
 * ���⿡�� �� ���� ���ڴ� �� ���� precedence�� �ǹ��Ѵ�.
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