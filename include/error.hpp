#ifndef SIMPLE_JSON_PARSER_ERROR
#define SIMPLE_JSON_PARSER_ERROR

#include <cstddef>

namespace SimpleJsonParser {
    enum class ErrorCode {
        /* No error */
        NoError,

        /* Error against parser setting */
        eCommentIsDisallowed,
        eIntegerIsTooLarge,
        eIntegerIsTooSmall,
        eFractionIsTooLarge,
        eFractionIsTooSmall,
        eStringIsTooLong,
        eArrayIsTooLong,
        eObjectHasTooManyMembers,
        eObjectDepthLimitExceed,

        /* Number format error */
        eMissingDigitsAfterMinus,
        eMissingDigitsAfterDot,
        eMissingPlusOrMinusAfterExponent,
        eMissingDigitsAfterPlusOrMinus,
        
        /* String format error */
        eMissingDoubleQuote,
        eMissingControlCharacterAfterBackslash,
        eMissingHexDigitsAfterBackslashU,

        /* Array format error */
        eMissingCommaOrSquareBracket,

        /* Object format error */
        eMissingColon,
        eMissingCommaOrCurlyBracked,

        /* Value format error */
        eNoCorrespondingValue,
    };

    class ErrorInfo {
    public:
        ErrorInfo() : m_error_code(ErrorCode::NoError), m_offset(0), m_line(0) {}
        ErrorInfo(ErrorCode error_code, size_t line, size_t offset) : m_error_code(error_code), m_line(line), m_offset(offset) {}

        inline bool is_error() const noexcept { return m_error_code != ErrorCode::NoError; }
        inline size_t line_number() const noexcept{ return m_line;}
        inline size_t offset() const noexcept { return m_offset; }
        inline ErrorCode error_code() const noexcept { return m_error_code; }
        inline const char* reason() const noexcept;

    private:
        ErrorCode m_error_code;
        size_t m_line;
        size_t m_offset;
    };
}

#endif