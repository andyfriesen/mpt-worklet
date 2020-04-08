#include <memory.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <emscripten.h>

#include "libopenmpt/libopenmpt.h"
#include "libopenmpt/libopenmpt_stream_callbacks_buffer.h"

const int BUFFERSIZE = 480;
const int SAMPLERATE = 48000;

namespace {
int16_t left[BUFFERSIZE];
int16_t right[BUFFERSIZE];
int16_t* const buffers[2] = {left, right};
openmpt_module* mod = nullptr;

void logfunc(const char* message, void* userdata) {
    (void)userdata;
    if (message) {
        fprintf(stderr, "openmpt: %s\n", message);
    }
}

int errfunc(int error, void* userdata) {
    (void)userdata;
    (void)error;
    return OPENMPT_ERROR_FUNC_RESULT_DEFAULT & ~OPENMPT_ERROR_FUNC_RESULT_LOG;
}

void printError(const char* func_name, int mod_err, const char* mod_err_str) {
    if (!func_name) {
        func_name = "unknown function";
    }
    if (mod_err == OPENMPT_ERROR_OUT_OF_MEMORY) {
        mod_err_str = openmpt_error_string(mod_err);
        if (!mod_err_str) {
            fprintf(stderr, "Error: %s\n", "OPENMPT_ERROR_OUT_OF_MEMORY");
        } else {
            fprintf(stderr, "Error: %s\n", mod_err_str);
            openmpt_free_string(mod_err_str);
            mod_err_str = nullptr;
        }
    } else {
        if (!mod_err_str) {
            mod_err_str = openmpt_error_string(mod_err);
            if (!mod_err_str) {
                fprintf(stderr, "Error: %s failed.\n", func_name);
            } else {
                fprintf(stderr, "Error: %s failed: %s\n", func_name, mod_err_str);
            }
            openmpt_free_string(mod_err_str);
            mod_err_str = nullptr;
        }
        fprintf(stderr, "Error: %s failed: %s\n", func_name, mod_err_str);
    }
}

} // namespace

extern "C" void load(void* data, int dataSize) {
    if (mod) {
        openmpt_module_destroy(mod);
        mod = nullptr;
    }

    if (!data) {
        return;
    }

    openmpt_stream_buffer buffer;
    openmpt_stream_buffer_init(&buffer, data, dataSize);

    int mod_err = OPENMPT_ERROR_OK;
    const char* mod_err_str = nullptr;

    mod = openmpt_module_create2(openmpt_stream_get_buffer_callbacks(), &buffer, &logfunc, nullptr, &errfunc, nullptr,
                                 &mod_err, &mod_err_str, nullptr);

    if (!mod) {
        printError("openmpt_module_create2()", mod_err, mod_err_str);
        openmpt_free_string(mod_err_str);
    }
}

extern "C" void process(int size, int16_t* leftPtr, int16_t* rightPtr) {
    if (!mod) {
        return;
    }

    openmpt_module_error_clear(mod);
    const auto count = openmpt_module_read_stereo(mod, SAMPLERATE, size, leftPtr, rightPtr);
    const auto mod_err = openmpt_module_error_get_last(mod);
    const auto mod_err_str = openmpt_module_error_get_last_message(mod);
    if (mod_err != OPENMPT_ERROR_OK) {
        printError("openmpt_module_read_stereo()", mod_err, mod_err_str);
        openmpt_free_string(mod_err_str);
    }
}
