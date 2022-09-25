#ifndef SIMPLE_JSON_PARSER_PARSER
#define SIMPLE_JSON_PARSER_PARSER

#include "error.hpp"
#include "document.hpp"
#include <string>
#include <limits>

namespace SimpleJsonParser {

    struct ParseResult {
        ErrorInfo m_error_info;
        Value m_value;
    }; 

     struct ParserSettings {
        bool allow_comments=true;
        int64_t max_integer_value=std::numeric_limits<int64_t>::max();
        int64_t min_integer_value=std::numeric_limits<int64_t>::min();
        double max_fraction_value=std::numeric_limits<double>::max();
        double min_fraction_value=std::numeric_limits<double>::min();
        size_t max_string_length=std::numeric_limits<size_t>::max();
        size_t max_array_length=std::numeric_limits<size_t>::max();
        size_t max_num_members=std::numeric_limits<size_t>::max();
        unsigned max_depth_object=100;
    };

    class ParserBase { 
        public:
            ParserBase() : m_settings() {}
            ParserBase(const ParserSettings& settings) : m_settings(settings) {}

            virtual ParseResult parse(const std::string& json_str) { throw std::runtime_error("Not implemented"); }
            virtual ParseResult parse(const char* begin, const char* end) { throw std::runtime_error("Not implemented"); }

        private:
            ParserSettings m_settings;
    };
}

#endif