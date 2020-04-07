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

using LoadCB = void (*)(void* data, int dataSize);
using ProcessCB = void (*)(int size, int16_t* leftPtr, int16_t* rightPtr);

EM_JS(void, wasm_setup, (LoadCB loadCB, ProcessCB processCb), {
    const BUFFERSIZE = 480;
    const SAMPLERATE = 48000;

    let bufferSize = BUFFERSIZE;
    let leftPtr = Module._malloc(BUFFERSIZE * 2);
    let rightPtr = Module._malloc(BUFFERSIZE * 2);

    class LibopenmptProcessor extends AudioWorkletProcessor {
        constructor() {
            super();

            this.port.onmessage = (event) => {
                const data = event.data; // ArrayBuffer
                if (data.byteLength == 0) {
                    Module.dynCall_vii(loadCb, 0, 0);
                } else {
                    const dataPtr = Module._malloc(data.byteLength);
                    HEAPU8.set(data, dataPtr);

                    Module.dynCall_vii(loadCb, dataPtr, data.byteLength);

                    Module._free(dataPtr);
                }
            };
        }

        process(inputs, outputs, parameters) {
            const left = outputs[0][0]; // Float32Array
            const right = outputs[0][1];

            Module.dynCall_viii(processCb, left.length, leftPtr, rightPtr);

            // Also upsample from int16 to float32
            left.set(HEAPU16.subarray(leftPtr >> 1, (leftPtr + left.length) >> 1));
            right.set(HEAPU16.subarray(rightPtr >> 1, (rightPtr + right.length) >> 1));
        }
    }

    globalThis.LibopenmptProcessor = LibopenmptProcessor;

    globalThis.registerProcessor('libopenmpt-processor', LibopenmptProcessor);
    console.log('registered!');
});

void load(void* data, int dataSize) {
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

void process(int size, int16_t* leftPtr, int16_t* rightPtr) {
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

int main(int argc, char* argv[]) {
    printf("main\n");
    wasm_setup(&load, &process);
    printf("setup complete\n");
    emscripten_pause_main_loop();
    printf("shouldn't get here?\n");

#if 0

    int result = 0;
    FILE* file = 0;
    openmpt_module* mod = 0;
    int mod_err = OPENMPT_ERROR_OK;
    const char* mod_err_str = nullptr;
    size_t count = 0;
    // PaError pa_error = paNoError;
    // int pa_initialized = 0;
    // PaStream* stream = 0;

    if (argc != 2) {
        fprintf(stderr, "Error: %s\n",
                "Wrong invocation. Use 'libopenmpt_example_c SOMEMODULE'.");
        goto fail;
    }

    if (strlen(argv[1]) == 0) {
        fprintf(stderr, "Error: %s\n",
                "Wrong invocation. Use 'libopenmpt_example_c SOMEMODULE'.");
        goto fail;
    }
    file = fopen(argv[1], "rb");

    if (!file) {
        fprintf(stderr, "Error: %s\n", "fopen() failed.");
        goto fail;
    }

    mod = openmpt_module_create2(openmpt_stream_get_file_callbacks(), file,
                                 &logfunc, nullptr, &errfunc, nullptr, &mod_err,
                                 &mod_err_str, nullptr);
    if (!mod) {
        printError("openmpt_module_create2()", mod_err, mod_err_str);
        openmpt_free_string(mod_err_str);
        mod_err_str = nullptr;
        goto fail;
    }
    openmpt_module_set_error_func(mod, nullptr, nullptr);

    // pa_error = Pa_Initialize();
    // if (pa_error != paNoError) {
    //     fprintf(stderr, "Error: %s\n", "Pa_Initialize() failed.");
    //     goto fail;
    // }
    // pa_initialized = 1;

    // pa_error = Pa_OpenDefaultStream(&stream, 0, 2, paInt16 | paNonInterleaved,
    //                                 SAMPLERATE, paFramesPerBufferUnspecified,
    //                                 nullptr, nullptr);
    // if (pa_error != paNoError) {
    //     fprintf(stderr, "Error: %s\n", "Pa_OpenStream() failed.");
    //     goto fail;
    // }
    // if (!stream) {
    //     fprintf(stderr, "Error: %s\n", "Pa_OpenStream() failed.");
    //     goto fail;
    // }

    // pa_error = Pa_StartStream(stream);
    // if (pa_error != paNoError) {
    //     fprintf(stderr, "Error: %s\n", "Pa_StartStream() failed.");
    //     goto fail;
    // }

    while (1) {

        openmpt_module_error_clear(mod);
        count = openmpt_module_read_stereo(mod, SAMPLERATE, BUFFERSIZE, left,
                                           right);
        mod_err = openmpt_module_error_get_last(mod);
        mod_err_str = openmpt_module_error_get_last_message(mod);
        if (mod_err != OPENMPT_ERROR_OK) {
            printError("openmpt_module_read_stereo()", mod_err, mod_err_str);
            openmpt_free_string(mod_err_str);
            mod_err_str = nullptr;
        }
        if (count == 0) {
            break;
        }

        // pa_error = Pa_WriteStream(stream, buffers, (unsigned long)count);
        // if (pa_error == paOutputUnderflowed) {
        //     pa_error = paNoError;
        // }
        // if (pa_error != paNoError) {
        //     fprintf(stderr, "Error: %s\n", "Pa_WriteStream() failed.");
        //     goto fail;
        // }
    }

    result = 0;

    goto cleanup;

fail:

    result = 1;

cleanup:

    // if (stream) {
    //     if (Pa_IsStreamActive(stream) == 1) {
    //         Pa_StopStream(stream);
    //     }
    //     Pa_CloseStream(stream);
    //     stream = 0;
    // }

    // if (pa_initialized) {
    //     Pa_Terminate();
    //     pa_initialized = 0;
    // }

    if (mod) {
        openmpt_module_destroy(mod);
        mod = 0;
    }

    if (file) {
        fclose(file);
        file = 0;
    }

    return result;
#endif
}
