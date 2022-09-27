#include <gtest/gtest.h>
#include "document.hpp"

using SimpleJsonParser::Value;
using SimpleJsonParser::ValueType;

TEST(ValueTypeTest, TestValueSetCorrectly) {
    ASSERT_TRUE(Value().is_uninitialized());
    ASSERT_TRUE(Value(ValueType::Uninitialized).is_uninitialized());
    ASSERT_TRUE(Value(ValueType::Integer).is_integer());
    ASSERT_TRUE(Value(ValueType::Fraction).is_fraction());
    ASSERT_TRUE(Value(ValueType::String).is_string());
    ASSERT_TRUE(Value(ValueType::Array).is_array());
    ASSERT_TRUE(Value(ValueType::Object).is_object());
    ASSERT_TRUE(Value(ValueType::Boolean).is_boolean());
    ASSERT_TRUE(Value(ValueType::Null).is_null());


    ASSERT_TRUE(Value{static_cast<int64_t>(0.0)}.is_integer());
    ASSERT_TRUE(Value{1.0}.is_fraction());
    ASSERT_TRUE(Value{"Hello"}.is_string());
    ASSERT_TRUE(Value{true}.is_boolean());
}


TEST(ValueStringTest, TestValueString) {
    Value value{"Hello"};
    ASSERT_EQ(value.string(), "Hello");
}

TEST(ValueIntegerTest, TestValueInteger) {
    Value value{static_cast<int64_t>(0.0)};
    ASSERT_EQ(value.integer(), 0);
}

TEST(ValueFractionTest, TestValueFraction) {
    Value value{1.0};
    ASSERT_EQ(value.fraction(), 1.0);
}

TEST(ValueBooleanTest, TestValueBoolean) {
    Value value{true};
    ASSERT_EQ(value.boolean(), true);
}

TEST(ValueArrayTest, TestValueArray) {
    Value value{ValueType::Array};
    value.push_back(Value{1.0});
    value.push_back(Value{2.0});
    value.push_back(Value{3.0});
    ASSERT_EQ(value.size_array(), 3.0);
    ASSERT_EQ(value[0].fraction(), 1.0);
    ASSERT_EQ(value[1].fraction(), 2.0);
    ASSERT_EQ(value[2].fraction(), 3.0);
}

TEST(ValueObjectTest, TestValueObject) {
    Value value{ValueType::Object};
    value["one"] = Value{1.0};
    value["two"] = Value{2.0};
    value["three"] = Value{3.0};
    ASSERT_EQ(value.size_object(), 3);
    ASSERT_EQ(value["one"].fraction(), 1.0);
    ASSERT_EQ(value["two"].fraction(), 2.0);
    ASSERT_EQ(value["three"].fraction(), 3.0);
}