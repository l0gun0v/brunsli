#include <brunsli/decoder.h>
#include <ctime>

/// #### BrunsliDecoder
bool BrunsliDecoder::decode(const std::string &in, const std::string &out) {
    const std::clock_t c_start = std::clock();

    std::string input;
    bool ok = FileAccessor::readFile(in, &input);
    if (!ok) return false;
    std::string output;
    unsigned long input_size = input.size();
    {
        brunsli::JPEGData jpg;
        if(!this->decoder.decode(&input, &jpg)){
            return false;
        }
        if(!encodeJEPG(&jpg, &output)){
            return false;
        }
    }
    unsigned long output_size = output.size();
    ok = FileAccessor::writeFile(out, output);
    if(!ok){
        return false;
    }
    const std::clock_t c_end = std::clock();
    if(this->bench){
        fprintf(stdout, "CPU time used: %.3f ms\nJPEG to BRN ratio = %.3f\n", 1000.0 * double(c_end - c_start) / CLOCKS_PER_SEC, double(output_size)/double(input_size));
    }
    return true;
}

bool BrunsliDecoder::encodeJEPG(brunsli::JPEGData* jpg, std::string* output) {
    brunsli::JPEGOutput writer(FileAccessor::StringWriter, output);
    bool ok = brunsli::WriteJpeg(*jpg, writer);
    if (!ok) {
        fprintf(stderr, "Failed to serialize JPEG data.\n");
        return false;
    }
    return true;
}

/// #### DefaultDecoder
bool DefaultDecoder::decode(std::string *input, brunsli::JPEGData* jpg) const {
    const uint8_t* input_data = reinterpret_cast<const uint8_t*>(input->data());
    bool ok = brunsli::BrunsliDecodeJpeg(input_data, input->size(), jpg) == brunsli::BRUNSLI_OK;
    input->clear();
    input->shrink_to_fit();
    if (!ok) {
        fprintf(stderr, "Failed to parse Brunsli input.\n");
        return false;
    }
    return true;
}

/// #### ????
