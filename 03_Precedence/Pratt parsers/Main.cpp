#include <iostream>
#include "Lexer.hpp"
#include "BantamParser.hpp"

int sFailed;
int sPassed;

static void test(const std::string& source, const std::string& expected)
{
	Lexer* lexer = new Lexer(source);
	BantamParser* parser = new BantamParser(lexer);

	try
	{
		Expression* result = parser->parseExpression();
		std::string builder;
		builder.reserve(4096);
		result->print(builder);


		if (expected == builder)
		{
			++sPassed;
		}
		else
		{
			++sFailed;
			std::cout << "[FAIL] Expected: " + expected << '\n';
			std::cout << "			Error: " + builder << '\n';
		}
	}
	catch (std::runtime_error& ex)
	{
		++sFailed;
		std::cout << "[FAIL] Expected: " + expected << '\n';
		std::cout << "			Error: " + std::string(ex.what()) << '\n';
	}
}

int main(void)
{
	test("a()", "a()");
	test("a(b)", "a(b)");
	test("a(b, c)", "a(b, c)");
	test("a(b)(c)", "a(b)(c)");
	test("a(b) + c(d)", "(a(b) + c(d))");
	test("a(b ? c : d, e + f)", "a((b ? c : d), (e + f))");

	// Unary precedence.
	test("~!-+a", "(~(!(-(+a))))");
	test("a!!!", "(((a!)!)!)");

	// Unary and binary predecence.
	test("-a * b", "((-a) * b)");
	test("!a + b", "((!a) + b)");
	test("~a ^ b", "((~a) ^ b)");
	test("-a!", "(-(a!))");
	test("!a!", "(!(a!))");

	// Binary precedence.
	test("a = b + c * d ^ e - f / g", "(a = ((b + (c * (d ^ e))) - (f / g)))");

	// Binary associativity.
	test("a = b = c", "(a = (b = c))");
	test("a + b - c", "((a + b) - c)");
	test("a * b / c", "((a * b) / c)");
	test("a ^ b ^ c", "(a ^ (b ^ c))");

	// Conditional operator.
	test("a ? b : c ? d : e", "(a ? b : (c ? d : e))");
	test("a ? b ? c : d : e", "(a ? (b ? c : d) : e)");
	test("a + b ? c * d : e / f", "((a + b) ? (c * d) : (e / f))");

	// Grouping.
	test("a + (b + c) + d", "((a + (b + c)) + d)");
	test("a ^ (b + c)", "(a ^ (b + c))");
	test("(!a)!", "((!a)!)");

	if (sFailed == 0)
	{
		std::cout << "Passed All " + std::to_string(sPassed) + std::string(" tests.\n");
	}
	else
	{
		std::cout << "----\n"
			<< "Failed " + sFailed + std::string(" out of ") + std::to_string(sFailed + sPassed) + " tests.\n";
	}
}