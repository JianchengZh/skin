#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <string>
#include <vector>
using namespace std;

//��Ģ������ ���� Ŭ����
const string simbols = "(+-*/) ";

string GetPostFix(const string infixExpression);
int Calculate(const string postfixExpression);

#endif