#include <brunsli/encoder.h>

// #### BrunsliEncoder

bool BrunsliEncoder::encode(const std::string& in, const std::string& out) {
    const std::clock_t c_start = std::clock();

    std::string input;
    bool ok = FileAccessor::readFile(in, &input);
    if (!ok) return false;

    unsigned long input_size = input.size();
    std::string output;
    {
        brunsli::JPEGData *jpg = readJEPG(input);
        if(jpg == nullptr){
            return false;
        }
        size_t output_size = brunsli::GetMaximumBrunsliEncodedSize(*jpg);
        output.resize(output_size);
        auto* output_data = reinterpret_cast<uint8_t*>(&output[0]);
        ok = this->encoder.encode(*jpg, output_data, &output_size);
        if (!ok) {
            fprintf(stderr, "Failed to transform JPEG to Brunsli\n");
            return false;
        }
        output.resize(output_size);
    }
    unsigned long output_size = output.size();
    ok = FileAccessor::writeFile(out, output);
    if(!ok){
        return false;
    }
    const std::clock_t c_end = std::clock();
    if(this->bench){
        fprintf(stdout, "CPU time used: %.3f ms\nJPEG to BRN ratio = %.3f\n", 1000.0 * double(c_end - c_start) / CLOCKS_PER_SEC, double(input_size)/double(output_size));    }
    return true;
}

brunsli::JPEGData* BrunsliEncoder::readJEPG(std::string &data) {
    const auto* input_data = reinterpret_cast<const uint8_t*>(data.data());
    auto *jpg = new brunsli::JPEGData();
    bool ok = brunsli::ReadJpeg(input_data, data.size(), brunsli::JPEG_READ_ALL, &*jpg);
    if (!ok) {
        fprintf(stderr, "Failed to parse JPEG input.\n");
        return nullptr;
    }
    data.clear();
    data.shrink_to_fit();
    return jpg;
}

// #### DefaultEncoder


bool DefaultEncoder::encode(const brunsli::JPEGData &jpg, uint8_t *data, size_t *len) const{
    return brunsli::BrunsliEncodeJpeg(jpg, data, len);
}

// #### SplitEncoder

bool SplitEncoder::encode(const brunsli::JPEGData &jpg, uint8_t *data, size_t *len) const {
    brunsli::internal::enc::State state;
    size_t num_components = jpg.components.size();

    if(!SplitEncoder::prepare(&state, jpg, num_components)){
        return false;
    }

    if(!SplitEncoder::DCPredict(&state, num_components)){
        return false;
    }


    std::vector<std::vector<uint8_t>> block_state(num_components);
    for (size_t i = 0; i < num_components; ++i) {
        block_state[i].resize(state.meta[i].width_in_blocks * state.meta[i].height_in_blocks);
        state.meta[i].block_state = block_state[i].data();
    }

    EncodeDC(&state);
    EncodeAC(&state);
    std::unique_ptr<brunsli::internal::enc::EntropyCodes> entropy_codes = PrepareEntropyCodes(&state);
    state.entropy_codes = entropy_codes.get();

    return BrunsliSerialize(&state, jpg, 0, data, len);
}


bool SplitEncoder::prepare(brunsli::internal::enc::State* state,
                           const brunsli::JPEGData &jpg, 
                           size_t num_components
                           ) {
    state->use_legacy_context_model = !(jpg.version & 2);

    if (!CalculateMeta(jpg, state)) return false;

    for (size_t i = 0; i < num_components; ++i) {
        state->meta[i].approx_total_nonzeros = SampleNumNonZeros(&(state->meta[i]));
    }
    for (size_t i = 0; i < num_components; ++i) {
        state->meta[i].context_bits = brunsli::internal::enc::SelectContextBits(  state->meta[i].approx_total_nonzeros + 1);
    }
    size_t num_contexts = num_components;
    for (size_t i = 0; i < num_components; ++i) {
        state->meta[i].context_offset = num_contexts;
        num_contexts += brunsli::kNumNonzeroContextSkip[state->meta[i].context_bits];
    }
    state->num_contexts = num_contexts;

    return true;
}

bool SplitEncoder::DCPredict(brunsli::internal::enc::State* state,
                             size_t num_components
                             ) {

    auto *dc_prediction_errors = new std::vector<std::vector<brunsli::coeff_t>>(num_components);
    for (size_t i = 0; i < num_components; ++i) {
        (*dc_prediction_errors)[i].resize(  state->meta[i].width_in_blocks *
                                         state->meta[i].height_in_blocks);
        state->meta[i].dc_prediction_errors = (*dc_prediction_errors)[i].data();
    }
    if (!PredictDCCoeffs(state)) return false;

    return true;
}