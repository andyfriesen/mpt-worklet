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

extern "C" void process1(int size, float* outPtr) {
    if (!mod) {
        return;
    }

    mod->read(SAMPLERATE, size, outPtr);
}

extern "C" void process2(int size, float* leftPtr, float* rightPtr) {
    if (!mod) {
        return;
    }

    mod->read(SAMPLERATE, size, leftPtr, rightPtr);
}

extern "C" void process4(int size, float* leftPtr, float* rightPtr, float* leftBackPtr, float* rightBackPtr) {
    if (!mod) {
        return;
    }

    mod->read(SAMPLERATE, size, leftPtr, rightPtr, leftBackPtr, rightBackPtr);
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