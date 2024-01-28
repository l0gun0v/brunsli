#include "brunsli/fileAccessor.h"

#if defined(_WIN32)
#define fopen ms_fopen
static FILE* ms_fopen(const char* filename, const char* mode) {
  FILE* result = 0;
  fopen_s(&result, filename, mode);
  return result;
}
#endif  /* WIN32 */

/// reading from file
bool FileAccessor::readFile(const std::string &file_name, std::string *content) {
    FILE* file = fopen(file_name.c_str(), "rb");
    if (file == nullptr) {
        fprintf(stderr, "Failed to open input file.\n");
        return false;
    }
    bool ok = FileAccessor::readFileInternal(file, content);
    if (fclose(file) != 0) {
        if (ok) {
            fprintf(stderr, "Failed to close input file.\n");
        }
        return false;
    }
    return ok;
}

bool  FileAccessor::readFileInternal(FILE* file, std::string *content) {
    if (fseek(file, 0, SEEK_END) != 0) {
        fprintf(stderr, "Failed to seek end of input file.\n");
        return false;
    }
    int input_size = ftell(file);
    if (input_size == 0) {
        fprintf(stderr, "Input file is empty.\n");
        return false;
    }
    if (fseek(file, 0, SEEK_SET) != 0) {
        fprintf(stderr, "Failed to rewind input file to the beginning.\n");
        return false;
    }
    content->resize(input_size);
    size_t read_pos = 0;
    while (read_pos < content->size()) {
        const size_t bytes_read =
                fread(&content->at(read_pos), 1, content->size() - read_pos, file);
        if (bytes_read == 0) {
            fprintf(stderr, "Failed to read input file\n");
            return false;
        }
        read_pos += bytes_read;
    }
    return true;
}

/// reading to file
bool FileAccessor::writeFile(const std::string &file_name, std::string& content) {
    FILE* file = fopen(file_name.c_str(), "wb");
    if (file == nullptr) {
        fprintf(stderr, "Failed to open file for writing.\n");
        return false;
    }
    bool ok = FileAccessor::writeFileInternal(file, content);
    if (fclose(file) != 0) {
        if (ok) {
            fprintf(stderr, "Failed to close output file.\n");
        }
        return false;
    }
    return ok;
}

bool FileAccessor::writeFileInternal(FILE *file, std::string& content) {
    size_t write_pos = 0;
    while (write_pos < content.size()) {
        const size_t bytes_written =
                fwrite(&content[write_pos], 1, content.size() - write_pos, file);
        if (bytes_written == 0) {
            fprintf(stderr, "Failed to write output.\n");
            return false;
        }
        write_pos += bytes_written;
    }
    return true;
}

size_t FileAccessor::StringWriter(void* data, const uint8_t* buf, size_t count) {
    auto* output = reinterpret_cast<std::string*>(data);
    output->append(reinterpret_cast<const char*>(buf), count);
    return count;
}