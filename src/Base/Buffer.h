#pragma once

#include "GLMInc.h"
#include "MemoryUtils.h"

namespace SoftGL
{
    enum BufferLayout
    {
        Layout_Linear,
        Layout_Tiled,
        Layout_Morton,
    };

    template<typename T>
    class Buffer
    {
        public:
            static std::shared_ptr<Buffer<T>> makeDefault(size_t w, size_t h);
            static std::shared_ptr<Buffer<T>> maskeLayout(size_t w, size_t h, BufferLayout layout);

            virtual void initLayout()
            {
                innerWidth_ = width_;
                innerHeight_ = height_;
            }

            virtual inline size_t convertIndex(size_t x, size_t y) const
            {
                return x + y * innerWidth_;
            }

            virtual BufferLayout getLayout() const
            {
                return Layout_Linear;
            }

            void create(size_t w, size_t h, const uint8_t *data = nullptr)
            {
                if (w <= 0 || h <= 0) return;
                if (width_ == w && height_ == h) return;
                width_ = w;
                height_ = h;
                initLayout();
                dataSize_ = innerWidth_ * innerHeight_;
                data_ = MemoryUtils::makeBuffer<T>(dataSize_, data);
            }

        protected:
            size_t width_ = 0;
            size_t height_ = 0;
            size_t innerWidth_ = 0;
            size_t innerHeight_ = 0;
            std::shared_ptr<T> data_ = nullptr;
            size_t dataSize_ = 0;
    };
}
