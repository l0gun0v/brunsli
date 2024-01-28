#ifndef BRUNSLI_FILEACCESSOR_H
#define BRUNSLI_FILEACCESSOR_H

#include <string>

class FileAccessor {
public:
    static bool readFileInternal(FILE* file, std::string* content);
    static bool readFile(const std::string& file_name, std::string* content);
    static bool writeFileInternal(FILE* file, std::string& content);
    static bool writeFile(const std::string& file_name, std::string& content);
    static size_t StringWriter(void *data, const uint8_t *buf, size_t count);
};


#endif //BRUNSLI_FILEACCESSOR_H
