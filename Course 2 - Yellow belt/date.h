#pragma once

#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <iomanip>
#include <string>

using namespace std;
//==================================================================================
class Date {
public:    
    Date(int new_year, int new_month, int new_day);
    int GetYear() const;
    int GetMonth() const;
    int GetDay() const;

private:
    int year;
    int month;
    int day;
};
//==================================================================================
// сравнение для дат необходимо для использования их в качестве ключей словаря
bool operator<(const Date& lhs, const Date& rhs);
bool operator>(const Date& lhs, const Date& rhs);
bool operator<=(const Date& lhs, const Date& rhs);
bool operator>=(const Date& lhs, const Date& rhs);
bool operator==(const Date& lhs, const Date& rhs);
bool operator!=(const Date& lhs, const Date& rhs);
//==================================================================================
// даты будут по умолчанию выводиться в нужном формате
ostream& operator<<(ostream& stream, const Date& date);
//==================================================================================
// парсинг даты
Date ParseDate(istream& date);