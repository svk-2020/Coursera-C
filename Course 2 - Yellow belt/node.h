#pragma once

#include "date.h"
#include <string>

using namespace std;

enum class Comparison {
    Less,
    LessOrEqual,
    Greater,
    GreaterOrEqual,
    Equal,
    NotEqual,
};

enum class LogicalOperation {
    And,
    Or
};
//==================================================================================
struct Node {
    virtual bool Evaluate(const Date& date, const string& event) const = 0;
};
//==================================================================================
class EmptyNode : public Node {
    bool Evaluate(const Date& date, const string& event) const override;
};
//==================================================================================
class DateComparisonNode : public Node {
public:
    DateComparisonNode(const Comparison& cmp, const Date& value);
    bool Evaluate(const Date& date, const string& event) const override;
    const Comparison cmp_;
    const Date value_;
};
//==================================================================================
class EventComparisonNode : public Node {
public:
    EventComparisonNode(const Comparison& cmp, const string& value);
    bool Evaluate(const Date& date, const string& event) const override;
    const Comparison cmp_;
    const string value_;
};
//==================================================================================
class LogicalOperationNode : public Node {
public:
    LogicalOperationNode(const LogicalOperation& op, const shared_ptr<Node>& lhs, const shared_ptr<Node>& rhs);
    bool Evaluate(const Date& date, const string& event) const override;
    const LogicalOperation op_;
    const shared_ptr<Node> lhs_, rhs_;
};