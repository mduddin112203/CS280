// Md Uddin
// NJIT ID: 31569771
// UCID: Msu7
// CS280-012
#include "val.h"
Value Value::operator/(const Value& op) const {
    if (IsInt() && op.IsInt()) {
        return Value(GetInt() / op.GetInt());
    }
    else if (IsReal() && op.IsReal()) {
        return Value(GetReal() / op.GetReal());
    }
    else if (IsReal() && op.IsInt()) {
        return Value(GetReal() / (double)op.GetInt());
    }
    else if (IsInt() && op.IsReal()) {
        return Value((double)GetInt() / op.GetReal());
    }
    else {
        return Value();
    }
}
Value Value::operator+(const Value& op) const {
    if (IsInt() && op.IsInt()) {
        return Value(GetInt() + op.GetInt());
    }
    else if (IsReal() && op.IsReal()) {
        return Value(GetReal() + op.GetReal());
    }
    else if (IsReal() && op.IsInt()) {
        return Value(GetReal() + (double)op.GetInt());
    }
    else if (IsInt() && op.IsReal()) {
        return Value((double)GetInt() + op.GetReal());
    }
    else {
        return Value();
    }
}
Value Value::Catenate(const Value & op) const{
    if (IsString() && op.IsString()) {
        return Value(GetString() + op.GetString());
    }
    else {
        return Value();
    }
}
Value Value::operator-(const Value& op) const {
    if (IsInt() && op.IsInt()) {
        return Value(GetInt() - op.GetInt());
    }
    else if (IsReal() && op.IsReal()) {
        return Value(GetReal() - op.GetReal());
    }
    else if (IsReal() && op.IsInt()) {
        return Value(GetReal() - (double)op.GetInt());
    }
    else if (IsInt() && op.IsReal()) {
        return Value((double)GetInt() - op.GetReal());
    }
    else {
        return Value();
    }
}
Value Value::operator<(const Value& op) const{
    if (IsInt() && op.IsInt()) {
        return Value(GetInt() < op.GetInt());
    }
    else if (IsReal() && op.IsReal()) {
        return Value(GetReal() < op.GetReal());
    }
    else if (IsReal() && op.IsInt()) {
        return Value(GetReal() < (double)op.GetInt());
    }
    else if (IsInt() && op.IsReal()) {
        return Value((double)GetInt() < op.GetReal());
    }
    else {
        return Value();
    }
}
Value Value::operator==(const Value& op) const {
    if (IsInt() && op.IsInt()) {
        return Value(GetInt() == op.GetInt());
    }
    else if (IsReal() && op.IsReal()) {
        return Value(GetReal() == op.GetReal());
    }
    else if (IsReal() && op.IsInt()) {
        return Value(GetReal() == (double)op.GetInt());
    }
    else if (IsInt() && op.IsReal()) {
        return Value((double)GetInt() == op.GetReal());
    }
    else {
        return Value();
    }
}
Value Value::operator>(const Value& op) const {
    if (IsInt() && op.IsInt()) {
        return Value(GetInt() > op.GetInt());
    }
    else if (IsReal() && op.IsReal()) {
        return Value(GetReal() > op.GetReal());
    }
    else if (IsReal() && op.IsInt()) {
        return Value(GetReal() > (double)op.GetInt());
    }
    else if (IsInt() && op.IsReal()) {
        return Value((double)GetInt() > op.GetReal());
    }
    else {
        return Value();
    }
}
Value Value::Power(const Value & op) const{
    if (IsReal() && op.IsReal()) {
        return Value(pow(GetReal(), op.GetReal()));
    }
    else if (op.IsInt() && IsInt()) {
        return Value(pow((double)GetInt(), (double)op.GetInt()));
    }
    else if (IsInt() && op.IsReal()) {
        return Value(pow((double)GetInt(), op.GetReal()));
    }
    else if (IsReal() && op.IsInt()) {
        return Value(pow(GetReal(), (double)op.GetInt()));
    }
    else {
        return Value();
    }
}
Value Value::operator*(const Value& op) const{
    if (IsInt() && op.IsInt()) {
        return Value(GetInt() * op.GetInt());
    }
    else if (IsReal() && op.IsReal()) {
        return Value(GetReal() * op.GetReal());
    }
    else if (IsReal() && op.IsInt()) {
        return Value(GetReal() * (double)op.GetInt());
    }
    else if (IsInt() && op.IsReal()) {
        return Value((double)GetInt() * op.GetReal());
    }
    else {
        return Value();
    }
}