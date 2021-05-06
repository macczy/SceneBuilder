#pragma once
#include <variant>
#include <memory>
#include "../Scanner/Token.h"
#include "Value.h"

class Addition;

using Term = std::variant<Addition, Value>;

class Addition {
public:
	Addition(const Position& position, std::unique_ptr<Term> value1, std::unique_ptr<Term>  value2) :
		term1(std::move(value1)), term2(std::move(value2)), position(position) {};
	Term* getFirstTerm() { return term1.get(); }
	Term* getSecondTerm() { return term2.get(); }
	const Position& getPosition() const { return position; }
	virtual ~Addition() {}
private:
	std::unique_ptr<Term> term1;
	std::unique_ptr<Term> term2;
	Position position;
};


class Sum : public Addition
{
public:
	Sum(const Position& position, std::unique_ptr<Term> value1, std::unique_ptr<Term>  value2) :
		Addition(position, std::move(value1), std::move(value2)) {};

	virtual ~Sum() {}
};

class Substraction : public Addition
{
public:
	Substraction(const Position& position, std::unique_ptr<Term> value1, std::unique_ptr<Term>  value2) :
		Addition(position, std::move(value1), std::move(value2)) {};
	virtual ~Substraction() {}
};

namespace AdditionFactory
{
	bool isAdditionOperator(const Token::TokenType& oper);
	std::unique_ptr<Addition> getAddition(const Position& position, std::unique_ptr<Term> value1, std::unique_ptr<Term> value2, const Token::TokenType& oper);
};