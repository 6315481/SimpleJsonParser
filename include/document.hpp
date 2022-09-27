#ifndef SIMPLE_JSON_PARSER_DOCUMENT
#define SIMPLE_JSON_PARSER_DOCUMENT

#include <algorithm>
#include <vector>
#include <map>
#include <utility>
#include <string>
#include <stdexcept>
#include <memory>
#include <iostream>
#include <string_view>

namespace SimpleJsonParser {

    enum class ValueType {
        Uninitialized = 0,
        Integer = 1 << 0,
        Fraction = 1 << 1,
        String = 1 << 2,
        Array = 1 << 3,
        Object = 1 << 4,
        Boolean = 1 << 5,
        Null = 1 << 6,
    };

    static std::map<ValueType, const std::string> ValueTypeToString = {
        {ValueType::Uninitialized, "Uninitialized"},
        {ValueType::Integer, "Integer"},
        {ValueType::Fraction, "Fraction"},
        {ValueType::String, "String"},
        {ValueType::Array, "Array"},
        {ValueType::Object, "Object"},
        {ValueType::Boolean, "Boolean"},
        {ValueType::Null, "Null"},
    };

     std::ostream& operator<<(std::ostream& s, ValueType type) {
        return s << ValueTypeToString[type];
    }

    class Value {
    public:

        using ValueIterator = Value*;

        Value() : m_type(ValueType::Uninitialized) {}
        
        Value(ValueType type): m_type(type) { 
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
            else if(type == ValueType::Boolean) {
                m_data.boolean = false;
            }
        }

        Value(int64_t val) : m_type(ValueType::Integer) {
            m_data.integer = val;
        }

        Value(double val) : m_type(ValueType::Fraction) {
            m_data.fraction = val;
        }

        Value(const char* str) : m_type(ValueType::String) {
            string_ptr = std::make_unique<std::string>(str);
        }
        
        Value(const std::string& str) : m_type(ValueType::String) {
            string_ptr = std::make_unique<std::string>("");
            *string_ptr = str;
        }

        Value(bool val) : m_type(ValueType::Boolean) {
            m_data.boolean = val;
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
            else if(m_type == ValueType::Boolean) {
                return m_data.boolean == other.boolean();
            }
            else if(m_type == ValueType::Null) {
                return true;
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
            else if(m_type == ValueType::Boolean) {
                return Value(m_data.boolean);
            }
            else if(m_type == ValueType::Null) {
                return Value(ValueType::Null);
            }

            return Value();
        }
        
        bool is_uninitialized() const { return m_type == ValueType::Uninitialized; }
        bool is_integer() const { return m_type == ValueType::Integer; }
        bool is_fraction() const { return m_type == ValueType::Fraction; }
        bool is_string() const { return m_type == ValueType::String; }
        bool is_array() const { return m_type == ValueType::Array; }
        bool is_object() const { return m_type == ValueType::Object; }
        bool is_boolean() const { return m_type == ValueType::Boolean; }
        bool is_null() const { return m_type == ValueType::Null; }

        int64_t integer() const {
            check_type(ValueType::Integer);
            return m_data.integer;
        }

        void set_integer(int64_t val) {
            check_type(ValueType::Integer);
            m_data.integer = val;
        }

        double fraction() const {
            check_type(ValueType::Fraction);
            return m_data.fraction;
        }

        bool boolean() const {
            check_type(ValueType::Boolean);
            return m_data.boolean;
        }

        void set_fraction(double val) {
            check_type(ValueType::Fraction);
            m_data.fraction = val;
        }

        const std::string& string() const {
           check_type(ValueType::String);
            return *string_ptr;
        }

        void set_string(const std::string& str) {
            check_type(ValueType::String);
            *string_ptr = str;
        }

        void set_string(std::string&& str) {
            check_type(ValueType::String);
            *string_ptr = std::move(str);
        }

        /* Array */
        void push_back(Value&& member) {
            check_type(ValueType::Array);
            array_ptr->push_back(std::move(member));
        }

        void push_back(const Value& member) {
            check_type(ValueType::Array);
            array_ptr->push_back(member.clone());
        }

        size_t size_array() {
            check_type(ValueType::Array);
            return array_ptr->size();
        }

        ValueIterator begin() const {
            check_type(ValueType::Array);
            return this->array_ptr->data();
        }

        ValueIterator end() const {
            check_type(ValueType::Array);
            return this->array_ptr->data() + this->array_ptr->size();
        }

        Value& operator[](size_t index) const {
            check_type(ValueType::Array);
            return (*array_ptr)[index];
        }

        /* Object  */
        void add_member(const std::string& key, Value&& member) {
            check_type(ValueType::Object);
            (*object_ptr)[key] = std::move(member);
        }

        void add_member(const std::string& key, const Value& member) {
            check_type(ValueType::Object);
            (*object_ptr)[key] = member.clone();
        }
        
        Value& operator[](const std::string& key) const {
            check_type(ValueType::Object);
            return (*object_ptr)[key];
        }

        size_t size_object() {
            check_type(ValueType::Object);
            return object_ptr->size();
        }
                
    private:
        inline void check_type(ValueType type) const {
            if(m_type != type)
                throw std::runtime_error("The value type is not " + ValueTypeToString[type]);
        }

        union Data  {
            bool boolean;
            int64_t integer;
            double fraction;
        };
        std::unique_ptr<std::string> string_ptr;
        std::unique_ptr<std::vector<Value>> array_ptr;
        std::unique_ptr<std::map<std::string, Value>> object_ptr;
        union Data m_data;
        ValueType m_type;
    };
}


#endif