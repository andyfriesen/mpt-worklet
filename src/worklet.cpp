#include <memory.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libopenmpt/libopenmpt.hpp"

const int SAMPLERATE = 48000;

struct ModPlayer {
    openmpt::module* mpt = nullptr;
    bool paused = false;
};

// FIXME: If we used the C API instead, we could delete this whole file and do it all on the JS side.

extern "C" ModPlayer* load(void* data, int dataSize) {
    if (!data) {
        return nullptr;
    }

    try {
        if (const auto mpt = new openmpt::module(data, dataSize)) {
            return new ModPlayer { mpt };
        }
    } catch (const openmpt::exception& e) {
        printf("Failed to load module: %s\n", e.what());
    } catch (const std::exception& e) {
        printf("Some other exception! %s\n", e.what());
    } catch (...) {
        printf("DOTDOTDOT D:\n");
    }

    return nullptr;
}

extern "C" void unload(ModPlayer* mod) {
    if (mod) {
        delete mod->mpt;
        delete mod;
    }
}

extern "C" void process1(ModPlayer* mod, int sampleRate, int size, float* outPtr) {
    if (!mod || mod->paused) {
        std::fill(outPtr, outPtr + size, 0);
        return;
    }

    mod->mpt->read(sampleRate, size, outPtr);
}

extern "C" void process2(ModPlayer* mod, int sampleRate, int size, float* leftPtr, float* rightPtr) {
    if (!mod || mod->paused) {
        std::fill(leftPtr, leftPtr + size, 0);
        std::fill(rightPtr, rightPtr + size, 0);
        return;
    }

    mod->mpt->read(sampleRate, size, leftPtr, rightPtr);
}

extern "C" void process4(ModPlayer* mod, int sampleRate, int size, float* leftPtr, float* rightPtr, float* leftBackPtr, float* rightBackPtr) {
    if (!mod || mod->paused) {
        std::fill(leftPtr, leftPtr + size, 0);
        std::fill(rightPtr, rightPtr + size, 0);
        std::fill(leftBackPtr, leftBackPtr + size, 0);
        std::fill(rightBackPtr, rightBackPtr + size, 0);
        return;
    }

    mod->mpt->read(sampleRate, size, leftPtr, rightPtr, leftBackPtr, rightBackPtr);
}

extern "C" void setRepeatCount(ModPlayer* mod, int repeatCount) {
    if (mod) {
        mod->mpt->set_repeat_count(repeatCount);
    }
}

extern "C" void setPosition(ModPlayer* mod, double pos) {
    if (mod) {
        mod->mpt->set_position_seconds(pos);
    }
}

extern "C" double getPosition(ModPlayer* mod) {
    if (mod) {
        return mod->mpt->get_position_seconds();
    } else {
        return 0;
    }
}

extern "C" void setPaused(ModPlayer* mod, bool paused) {
    if (mod) {
        mod->paused = paused;
    }
}

extern "C" bool getPaused(ModPlayer* mod) {
    if (mod) {
        return mod->paused;
    }
}