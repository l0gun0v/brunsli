#ifndef BRUNSLI_DECODER_H
#define BRUNSLI_DECODER_H

#include <string>
#include <memory>
#include <cstdio>
#include <cstdlib>
#include <brunsli/fileAccessor.h>
#include <brunsli/brunsli_decode.h>
#include <brunsli/jpeg_data_writer.h>
#include <brunsli/jpeg_data.h>
#include <brunsli/types.h>
#include <brunsli/status.h>
#include "c/dec/state.h"


class IDecoder {
public:
    /// accept jpg and output array to write decoced data
    virtual bool decode(std::string *input, brunsli::JPEGData* jpg) const { return false; }
    virtual ~IDecoder() = default;
};

class DefaultDecoder : public IDecoder {
    bool decode(std::string *input, brunsli::JPEGData* jpg) const override;

};

class BrunsliDecoder {
private:
    const IDecoder& decoder;
    bool bench;
public:
    /// constructor accept encoder implementation specified in IEncoder
    explicit BrunsliDecoder(const IDecoder& decoder, bool bench) : decoder(decoder), bench(bench) {}
    /// function to decode brn to jpeg, decode(input file name, output file name)
    bool decode(const std::string& in, const std::string& out);
    /// function to read jpg pass data read with fileAccessor
    static bool encodeJEPG(brunsli::JPEGData*, std::string*);
};

#endif //BRUNSLI_DECODER_H
