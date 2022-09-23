#ifndef MINI_JSON_PARSER_DOCUMENT
#define MINI_JSON_PARSER_DOCUMENT

#include <algorithm>
#include <vector>
#include <map>
#include <utility>
#include <string>
#include <stdexcept>
#include <memory>
#include <iostream>

namespace MiniJsonParser {

    enum class ValueType {
        Uninitialized = 0,
        Integer = 1 << 0,
        Fraction = 1 << 1,
        String = 1 << 2,
        Array = 1 << 3,
        Object = 1 << 4,
        Number = Integer | Fraction
    };

    class Value {
    public:

        Value() : m_type(ValueType::Uninitialized) {}
        
        Value(ValueType type) : m_type(type) {
            if(type == ValueType::Integer) {
                m_data.integer = 0;
            }
            else if(type == ValueType::Fraction) {
                m_data.fraction = 0.0;
            }
            else if(type == ValueType::String) {
                string_ptr = std::make_unique<std::string>("");
            }
            else if(type == ValueType::Array) {
                array_ptr = std::make_unique<std::vector<Value>>();
            }
            else if(type == ValueType::Object) {
                object_ptr = std::make_unique<std::map<std::string, Value>>();
            }
        }

        Value(int64_t val) : m_type(ValueType::Integer) {
            m_data.integer = val;
        }

        Value(double val) : m_type(ValueType::Fraction) {
            m_data.fraction = val;
        }
        
        Value(const std::string& str) : m_type(ValueType::String) {
            string_ptr = std::make_unique<std::string>("");
            *string_ptr = str;
        }

        ValueType type() const {
            return m_type;
        }

        bool operator==(const Value& other) const {
            if(m_type != other.m_type) 
                return false;
            
            if(m_type == ValueType::Integer) {
                return m_data.integer == other.integer();
            }
            else if(m_type == ValueType::Fraction) {
                return m_data.fraction == other.fraction();
            }
            else if(m_type == ValueType::String) {
                return *(string_ptr) == *(other.string_ptr);
            }
            else if(m_type == ValueType::Array) {
                return *(array_ptr) == *(other.array_ptr);
            }
            else if(m_type == ValueType::Object) {
                return *(object_ptr) == *(other.object_ptr);
            }
    
            return true;
        }

        Value clone() const {
            if(m_type == ValueType::Integer) {
                return Value(m_data.integer);
            } 
            else if(m_type == ValueType::Fraction){
                return Value(m_data.fraction);
            }
            else if(m_type == ValueType::String) {
                return Value(*string_ptr);
            }
            else if(m_type == ValueType::Array) {
                Value temp = Value(ValueType::Array);
                for(const Value& val : *array_ptr) {
                    temp.push_back(val.clone());
                }
                return temp;
            }
            else if(m_type == ValueType::Object) {
                Value temp = Value(ValueType::Object);
                for(const auto& val : *object_ptr) {
                   temp.add_member(val.first, val.second);
                }

                return temp;
            }

            return Value();
        }
        
        bool is_integer() const { return m_type == ValueType::Integer; }
        bool is_fraction() const { return m_type == ValueType::Fraction; }
        bool is_number() const { return m_type == ValueType::Number; }
        bool is_string() const { return m_type == ValueType::String; }
        bool is_array() const { return m_type == ValueType::Array; }
        bool is_object() const { return m_type == ValueType::Object; }

        int64_t integer() const {
            if(m_type != ValueType::Integer)
                throw std::runtime_error("The value type is not integer");

            return m_data.integer;
        }

        double fraction() const {
            if(m_type != ValueType::Fraction)
                throw std::runtime_error("The value type is not fraction");
            
            return m_data.fraction;
        }

        const std::string& string() const {
            if(m_type != ValueType::String)
                throw std::runtime_error("The value type is not string");
            
            return *string_ptr;
        }

        Value& operator[](const std::string& key) const {
            if(m_type != ValueType::Object)
                throw std::runtime_error("The value type is not object");

            return (*object_ptr)[key];
        }

        Value& operator[](size_t index) const {
            if(m_type != ValueType::Array)
                throw std::runtime_error("The value type is not array");

            return (*array_ptr)[index];
        }

        void push_back(Value&& member) {
            if(m_type != ValueType::Array)
                throw std::runtime_error("The value type is not array");

            array_ptr->push_back(std::move(member));
        }

        void push_back(const Value& member) {
            if(m_type != ValueType::Array)
                throw std::runtime_error("The value type is not array");

            array_ptr->push_back(member.clone());
        }

        void add_member(const std::string& key, Value&& member) {
            if(m_type != ValueType::Object)
                throw std::runtime_error("The value type is not object");
            
            (*object_ptr)[key] = std::move(member);
        }

        void add_member(const std::string& key, const Value& member) {
            if(m_type != ValueType::Object)
                throw std::runtime_error("The value type is not object");
            
            (*object_ptr)[key] = member.clone();
        }
                
    private:
        union Data  {
            int64_t integer;
            double fraction;
        };
        std::unique_ptr<std::string> string_ptr;
        std::unique_ptr<std::vector<Value>> array_ptr;
        std::unique_ptr<std::map<std::string, Value>> object_ptr;
        union Data m_data;
        ValueType m_type;
    };

    std::ostream& operator<<(std::ostream& s, ValueType type) {
        if(type == ValueType::Uninitialized) {
            return s << "Uninitialized";
        }
        else if(type == ValueType::Integer) {
            return s << "Integer";
        }
        else if(type == ValueType::Fraction) {
            return s << "Fraction";
        }
        else if(type == ValueType::String) {
            return s << "String";
        } 
        else if(type == ValueType::Array) {
            return s << "Array";
        }
        else if(type == ValueType::Object) {
            return s << "Object";
        }
        else {
            return s << "Undefined Type";
        }
    }
}


#endif