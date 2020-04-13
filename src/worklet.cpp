#include <memory.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libopenmpt/libopenmpt.hpp"

const int SAMPLERATE = 48000;

namespace {
openmpt::module* mod = nullptr;
} // namespace

// FIXME: If we used the C API instead, we could delete this whole file and do it all on the JS side.

extern "C" void load(void* data, int dataSize) {
    if (mod) {
        delete mod;
        mod = nullptr;
    }

    if (!data) {
        return;
    }

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
}

extern "C" void process1(int sampleRate, int size, float* outPtr) {
    if (!mod) {
        std::fill(outPtr, outPtr + size, 0);
        return;
    }

    mod->read(sampleRate, size, outPtr);
}

extern "C" void process2(int sampleRate, int size, float* leftPtr, float* rightPtr) {
    if (!mod) {
        std::fill(leftPtr, leftPtr + size, 0);
        std::fill(rightPtr, rightPtr + size, 0);
        return;
    }

    mod->read(sampleRate, size, leftPtr, rightPtr);
}

extern "C" void process4(int sampleRate, int size, float* leftPtr, float* rightPtr, float* leftBackPtr, float* rightBackPtr) {
    if (!mod) {
        std::fill(leftPtr, leftPtr + size, 0);
        std::fill(rightPtr, rightPtr + size, 0);
        std::fill(leftBackPtr, leftBackPtr + size, 0);
        std::fill(rightBackPtr, rightBackPtr + size, 0);
        return;
    }

    mod->read(sampleRate, size, leftPtr, rightPtr, leftBackPtr, rightBackPtr);
}

extern "C" void setRepeatCount(int repeatCount) {
    if (mod) {
        mod->set_repeat_count(repeatCount);
    }
}

extern "C" void setPosition(double pos) {
    if (mod) {
        mod->set_position_seconds(pos);
    }
}

extern "C" double getPosition() {
    if (mod) {
        return mod->get_position_seconds();
    } else {
        return 0;
    }
}