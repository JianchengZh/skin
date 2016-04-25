#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <string>
#include <vector>
using namespace std;

//사칙연산을 위한 클래스
const string simbols = "(+-*/) ";

string GetPostFix(const string infixExpression);
int Calculate(const string postfixExpression);

#endif