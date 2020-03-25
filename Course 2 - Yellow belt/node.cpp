#include "node.h"

//==================================================================================
bool EmptyNode::Evaluate(const Date& date, const string& event) const {
    return true; 
}
//==================================================================================
DateComparisonNode::DateComparisonNode(const Comparison& cmp, const Date& value) :
    cmp_(cmp),
    value_(value) {}
//==================================================================================
bool DateComparisonNode::Evaluate(const Date& date, const string& event) const {
    switch (cmp_) {
    case Comparison::Less:
        return date < value_;
    case Comparison::LessOrEqual:
        return date <= value_;
    case Comparison::Greater:
        return date > value_;
    case Comparison::GreaterOrEqual:
        return date >= value_;
    case Comparison::Equal:
        return date == value_;
    case Comparison::NotEqual:
        return date != value_;
    } 
    return true;
}
//==================================================================================
EventComparisonNode::EventComparisonNode(const Comparison& cmp, const string& value) :
    cmp_(cmp),
    value_(value) {}
//==================================================================================
bool EventComparisonNode::Evaluate(const Date& date, const string& event) const {
    switch (cmp_) {
    case Comparison::Less:
        return event < value_;
    case Comparison::LessOrEqual:
        return event <= value_;
    case Comparison::Greater:
        return event > value_;
    case Comparison::GreaterOrEqual:
        return event >= value_;
    case Comparison::Equal:
        return event == value_;
    case Comparison::NotEqual:
        return event != value_;
    }    
    return true;
}
//==================================================================================
LogicalOperationNode::LogicalOperationNode(const LogicalOperation& op, 
    const shared_ptr<Node>& lhs, const shared_ptr<Node>& rhs) :
    op_(op),
    lhs_(lhs),
    rhs_(rhs) {}
//==================================================================================
bool LogicalOperationNode ::Evaluate(const Date& date, const string& event) const {
    switch (op_) {
    case LogicalOperation::And:
        return lhs_->Evaluate(date, event) and rhs_->Evaluate(date, event);
    case LogicalOperation::Or:
        return lhs_->Evaluate(date, event) or rhs_->Evaluate(date, event);
    }
    return true;
}