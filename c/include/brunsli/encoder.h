#ifndef BRUNSLI_ENCODE_H
#define BRUNSLI_ENCODE_H

#include <string>
#include <memory>
#include <cstdio>
#include <brunsli/fileAccessor.h>
#include <brunsli/jpeg_data_reader.h>
#include <brunsli/jpeg_data.h>
#include <brunsli/types.h>
#include <brunsli/brunsli_encode.h>
#include "c/enc/state.h"
#include "../common/context.h"

class IEncoder {
public:
    /// accept jpg and output array to write encoded data
    virtual bool encode(const brunsli::JPEGData &jpg, uint8_t *data, size_t *len) const { return false; }
    virtual ~IEncoder() = default;
};

/// Default brunsli encoder
class DefaultEncoder : public IEncoder {
public:
    DefaultEncoder() = default;
    bool encode(const brunsli::JPEGData &jpg, uint8_t *data, size_t *len) const override;
};

/// Split brunsli encoder with 2 function
class SplitEncoder : public IEncoder {
public:
    SplitEncoder() = default;
    static bool DCPredict(brunsli::internal::enc::State*, size_t);
    bool encode(const brunsli::JPEGData &jpg, uint8_t *data, size_t *len) const override;
    static bool prepare(brunsli::internal::enc::State *state,
                 const brunsli::JPEGData &jpg, size_t num_components) ;
};


#endif //BRUNSLI_ENCODE_H

class BrunsliEncoder {
private:
    const IEncoder& encoder;
    bool bench;
public:
    /// constructor accept encoder implementation specified in IEncoder
    explicit BrunsliEncoder(const IEncoder& encoder, bool bench) : encoder(encoder), bench(bench) {}
    /// function to encode jpg to brn, encode(input file name, output file name)
    bool encode(const std::string& in, const std::string& out);
    /// function to read jpg pass data read with fileAccessor
    static brunsli::JPEGData* readJEPG(std::string& data);
};
