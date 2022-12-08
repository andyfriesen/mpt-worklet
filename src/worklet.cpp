#include <memory.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libopenmpt/libopenmpt.hpp"

const int SAMPLERATE = 48000;

// FIXME: If we used the C API instead, we could delete this whole file and do it all on the JS side.

extern "C" openmpt::module* load(void* data, int dataSize) {
    if (!data) {
        return nullptr;
    }

    openmpt::module* mod = nullptr;
    char* d = (char*)data;

    try {
        mod = new openmpt::module(data, dataSize);
    } catch (const openmpt::exception& e) {
        printf("Failed to load module: %s\n", e.what());
    } catch (const std::exception& e) {
        printf("Some other exception! %s\n", e.what());
    } catch (...) {
        printf("DOTDOTDOT D:\n");
    }

    return mod;
}

extern "C" void unload(openmpt::module* mod) {
    if (mod) {
        delete mod;
    }
}

extern "C" void process1(openmpt::module* mod, int sampleRate, int size, float* outPtr) {
    if (!mod) {
        std::fill(outPtr, outPtr + size, 0);
        return;
    }

    mod->read(sampleRate, size, outPtr);
}

extern "C" void process2(openmpt::module* mod, int sampleRate, int size, float* leftPtr, float* rightPtr) {
    if (!mod) {
        std::fill(leftPtr, leftPtr + size, 0);
        std::fill(rightPtr, rightPtr + size, 0);
        return;
    }

    mod->read(sampleRate, size, leftPtr, rightPtr);
}

extern "C" void process4(openmpt::module* mod, int sampleRate, int size, float* leftPtr, float* rightPtr, float* leftBackPtr, float* rightBackPtr) {
    if (!mod) {
        std::fill(leftPtr, leftPtr + size, 0);
        std::fill(rightPtr, rightPtr + size, 0);
        std::fill(leftBackPtr, leftBackPtr + size, 0);
        std::fill(rightBackPtr, rightBackPtr + size, 0);
        return;
    }

    mod->read(sampleRate, size, leftPtr, rightPtr, leftBackPtr, rightBackPtr);
}

extern "C" void setRepeatCount(openmpt::module* mod, int repeatCount) {
    if (mod) {
        mod->set_repeat_count(repeatCount);
    }
}

extern "C" void setPosition(openmpt::module* mod, double pos) {
    if (mod) {
        mod->set_position_seconds(pos);
    }
}

extern "C" double getPosition(openmpt::module* mod) {
    if (mod) {
        return mod->get_position_seconds();
    } else {
        return 0;
    }
}