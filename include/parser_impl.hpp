#ifndef SIMPLE_JSON_PARSER_PARSER_IMPL
#define SIMPLE_JSON_PARSER_PARSER_IMPL

#include "error.hpp"
#include "parser.hpp"
#include "document.hpp"
#include <cassert>
#include <cctype>
#include <cstdlib>
#include <cstring>

namespace SimpleJsonParser {
   
    class NormalParser : public ParserBase {
        public:
            NormalParser() : ParserBase() {}
            NormalParser(const ParserSettings& settings) : ParserBase(settings) {}

            ParseResult parse(const std::string& json_str) override;
            ParseResult parse(const char* begin, const char* end) override;

        private:
            void init_parser_state(const char* begin, const char* end);
            Value parse_value();
            Value parse_object();
            Value parse_array();
            Value parse_string();
            Value parse_number();
            Value parse_special();
            void skip_whitespace();
            bool is_whitespace(char c);
            bool consume_4hex(std::string& str);
            bool consume_digits();
            bool consume_positive_digits();
            bool consume_exponent();
            char to_control(char c);
            bool consume_control_character(std::string& str);
            
            const char* m_cur;
            const char* m_end;
            size_t m_offset;
            unsigned m_line_number;
            ErrorInfo m_error_info;
    };

    void NormalParser::init_parser_state(const char* begin, const char* end) {
        m_cur = begin;
        m_end = m_end;
        m_offset = 0;
        m_line_number = 1;
        m_error_info = ErrorInfo{};
    }

    ParseResult NormalParser::parse(const std::string& json) {
        init_parser_state(json.c_str(), json.c_str() + json.size());
        return ParseResult{m_error_info, parse_value()};
    }

    ParseResult NormalParser::parse(const char* begin, const char* end) {
        init_parser_state(begin, end);
        return ParseResult{m_error_info, parse_value()};
    }

    
    bool NormalParser::is_whitespace(char c) { 
        return c == '\x20' || c == '\x09' || c == '\x0A' || c == '\x0D';
    }

    void NormalParser::skip_whitespace() {
        while(m_cur != m_end && is_whitespace(*m_cur)) {
            ++m_cur;
        }
    }

    bool NormalParser::consume_4hex(std::string& str) {
        assert(*m_cur == 'u');
        ++m_cur;
        if(m_cur + 3 >= m_end) {
            return false;
        }

        if(isdigit(m_cur[0]) && isdigit(m_cur[1]) && isdigit(m_cur[2]) && isdigit(m_cur[3])) {
            str += '\\';
            str += m_cur[0];
            str += m_cur[1];
            str += m_cur[2];
            str += m_cur[3];
            m_cur += 4;
            return true;
        }

        return false;
    }

    char NormalParser::to_control(char c) {
        assert(*m_cur == 'b' || *m_cur == 'f' || *m_cur == 'n' || *m_cur == 'r' || *m_cur == 't');
        switch(*m_cur) {
            case 'b': return '\b';
            case 'f': return '\f';
            case 'n': return '\n';
            case 'r': return '\r';
            case 't': return '\t';
        }

        return '\0';
    }

    bool NormalParser::consume_control_character(std::string& str) {
        assert(*m_cur== '\\');
        ++m_cur;

        if(m_cur == m_end) {
            return false;   
        }

        if(*m_cur == '"' || *m_cur == '\\' || *m_cur == '/') {
            str += *m_cur;
            ++m_cur;
            return true;
        }
        else if(*m_cur == 'b' || *m_cur == 'f' || *m_cur == 'n' || *m_cur == 'r' || *m_cur == 't') {
            str += to_control(*m_cur);
            ++m_cur;
            return true;
        }
        else if(*m_cur == 'u') {
            return consume_4hex(str);
        }

        return false;
    }

    Value NormalParser::parse_string() {
        std::string tmp;
        assert(*m_cur == '"');
        ++m_cur;

        while(m_cur != m_end && *m_cur != '"') {
            if(*m_cur == '\\') {
                if(!consume_control_character(tmp)) {
                    return Value();
                }
            }
            else {
                tmp += *m_cur;
                ++m_cur;
            }
        }

        ++m_cur;
        return Value(tmp);
    }

    bool NormalParser::consume_positive_digits() {
        if(m_cur == m_end || *m_cur == '0' || !isdigit(*m_cur)) {
            return false;
        }
        ++m_cur;

        while(m_cur != m_end && isdigit(*m_cur)) {
            ++m_cur;
        }

        return true;
    }

    bool NormalParser::consume_digits() {
        if(m_cur == m_end || !isdigit(*m_cur)) {
            return false;
        }

        while(m_cur != m_end && isdigit(*m_cur)) {
            ++m_cur;
        }

        return true;
    }

    bool NormalParser::consume_exponent() {
        assert(*m_cur == 'e' || *m_cur == 'E');
        ++m_cur;
        if(m_cur == m_end || (*m_cur != '+' && *m_cur != '-')) {
            return false;
        }
        ++m_cur;

        return consume_digits();
    }

    Value NormalParser::parse_number() {
        assert(isdigit(*m_cur) || *m_cur == '-');
        const char* start = m_cur;

        if(*m_cur == '-') {
            ++m_cur;
            if(!consume_positive_digits())
                return Value();
        }
        else if(*m_cur == '0') {
            ++m_cur;
        }
        else {
            consume_positive_digits();
        }
    
        if(m_cur != m_end && *m_cur == '.') {
            ++m_cur;
            if(!consume_digits())
                return Value();
        }

        if(m_cur != m_end && (*m_cur == 'e' || *m_cur == 'E')) {
            if(!consume_exponent())
                return Value();
        }

        return Value(std::strtod(start, nullptr));
    }

    Value NormalParser::parse_special() {
        assert(*m_cur == 't' || *m_cur == 'f' || *m_cur == 'n');
        if(m_cur + 3 < m_end && !std::memcmp(m_cur, "true", 4)) {
            m_cur += 4;
            return Value(true);
        }
        else if(m_cur + 4 < m_end && !std::memcmp(m_cur, "false", 5)) {
            m_cur += 5;
            return Value(false);
        }
        else if(m_cur + 3 < m_end && !std::memcmp(m_cur, "null", 4)) {
            m_cur += 4;
            return Value(ValueType::Null);
        }

        return Value();
    }

    Value NormalParser::parse_array() {
        assert(*m_cur == '[');
        ++m_cur;

        Value arr{ValueType::Array};

        skip_whitespace();

        if(m_cur == m_end || *m_cur == ']') {
            ++m_cur;
            return arr;
        }

        while(m_cur != m_end) {
            Value val = parse_value();
            arr.push_back(std::move(val));
            if(m_cur == m_end || *m_cur != ',') {
                break;
            }
            ++m_cur;
        }

        if(m_cur == m_end || *m_cur != ']') {
            return Value();
        }
        ++m_cur;
        return arr;
    }

    Value NormalParser::parse_object() {
        assert(*m_cur == '{');
        ++m_cur;

        Value obj{ValueType::Object};

        skip_whitespace();
        if(m_cur != m_end && *m_cur == '}') {
            ++m_cur;
            return obj;
        }

        while(m_cur != m_end) {
            skip_whitespace();
            Value key = parse_string();
            if(key.type() == ValueType::Uninitialized) {
                return Value();
            }

            skip_whitespace();  

            if(m_cur == m_end || *m_cur != ':') {
                return Value();
            }
            ++m_cur;

            Value val = parse_value();
            if(val.type() == ValueType::Uninitialized) {
                return Value();
            }

            obj.add_member(key.string(), std::move(val));

            if(m_cur == m_end || *m_cur != ',') {
                break;
            }
            assert(*m_cur == ',');
            ++m_cur;
        }

        if(m_cur == m_end || *m_cur != '}')
            return Value();

        ++m_cur;
        return obj;
    }

    Value NormalParser::parse_value() {
        Value v{};

        skip_whitespace();

        if(m_cur == m_end) {
            return v;
        }

        if(*m_cur == '{') {
            v = parse_object();
        }
        else if(*m_cur == '[') {
            v = parse_array();
        }
        else if(*m_cur == '"') {
            v = parse_string();
        }
        else if(isdigit(*m_cur) || *m_cur == '-') {
            v = parse_number();
        }
        else if(*m_cur == 'n' || *m_cur == 't' || *m_cur == 'f') {
            v = parse_special();
        }

        skip_whitespace();

        return v;
    }

}
#endif