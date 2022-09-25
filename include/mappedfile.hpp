#ifndef SIMPLE_JSON_PARSER_MAPPEDFILE
#define SIMPLE_JSON_PARSER_MAPPEDFILE

#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>

namespace SimpleJsonParser {

    class MemoryMappedFile {
    public:
        MemoryMappedFile() : m_data(nullptr), m_size(0) {}
        bool open_file(const std::string& filename) {
            int fp = open(filename.c_str(), O_RDONLY);
            if(fp == -1) {
                return false;
            }

            struct stat file_stat;
            if(fstat(fp, &file_stat) == -1) {
                return false;
            }

            m_size = file_stat.st_size;
            m_data = (char*)mmap(nullptr, m_size, PROT_READ, MAP_PRIVATE, fp, 0);
            if(m_data == MAP_FAILED) {
                return false;
            }

            close(fp);

            return true;
        }

        bool is_open() const {
            return m_data != nullptr;
        }

        ~MemoryMappedFile() {
            if(is_open()) {
                munmap(m_data, m_size);
            }
        }

        const char* data() const {
            return m_data;
        }

        const char* begin() const {
            return m_data;
        }

        const char* end() const {
            if (is_open()) {
                return m_data + m_size;
            }
            return m_data;
        }

    private:
        char* m_data;
        size_t m_size;
    };
}

#endif 