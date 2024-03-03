#pragma once

#include <cmath>
#include <memory>
#include "Logger.h"

#define SOFTGL_ALIGNMENT 32

namespace SoftGL
{
    class MemoryUtils
    {
        public:
            static void* alignedMalloc(size_t size, size_t alignment = SOFTGL_ALIGNMENT)
            {
                if ((alignment & (alignment - 1)) != 0) // alignment需要是2的n次方
                {
                    LOGE("failed to malloc, invalid alignment: %d", alignment);
                    return nullptr;
                }
                size_t extra = alignment + sizeof(void *);
                void *data = malloc(size + extra);
                if (!data)
                {
                    LOGE("Failed to malloc with size: %d", size);
                    return nullptr;
                }
                size_t addr = (size_t) data + extra;
                void *alignedPtr = (void *) (addr - (addr % alignment));
                *((void **) alignedPtr - 1) = data;
                return alignedPtr;
            }
    };
}
