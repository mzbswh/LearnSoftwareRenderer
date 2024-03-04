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

            virtual void destroy()
            {
                width_ = 0;
                height_ = 0;
                innerWidth_ = 0;
                innerHeight_ = 0;
                dataSize_ = 0;
                data_ = nullptr;
            }

            inline T* getRawDataPtr() const
            {
                return data_.get();
            }

            inline size_t getRawDataSize() const
            {
                return dataSize_;
            }

            inline size_t getRawDataByteSize() const
            {
                return dataSize_ * sizeof(T);
            }

            inline bool empty() const
            {
                return data_ == nullptr;
            }

            inline size_t getWidth() const
            {
                return width_;
            }

            inline size_t getHeight() const
            {
                return height_;
            }

            inline T* get(size_t x, size_t y)
            {
                T *ptr = data_.get();
                if (ptr != nullptr && x < width_ && y < height_)
                {
                    return &ptr[convertIndex(x, y)];
                }
                return nullptr;
            }

            inline void set(size_t x, size_t y, const T &pixel)
            {
                T *ptr = data_.get();
                if (ptr != nullptr && x < width_ && y < height_)
                {
                    ptr[convertIndex(x, y)] = pixel;
                }
            }

            void copyRawDataTo(T *out, bool flip_y = false) const
            {
                T *ptr = data_.get();
                if (ptr == nullptr) return;
                if (!flip_y)
                {
                    memcpy(out, ptr, getRawDataByteSize());
                }
                else
                {
                    for (int i = 0; i < innerHeight_; i++)
                    {
                        memcpy(out + innerWidth_ * i, ptr + innerWidth_ * (innerHeight_ - 1 - i), innerWidth_ * sizeof(T));
                    }
                }
            }

            inline void clear() const
            {
                T *ptr = data_.get();
                if (ptr == nullptr) return;
                memset(ptr, 0, getRawDataByteSize());
            }

            inline void setAll(T val) const
            {
                T *ptr = date_.get();
                if (ptr == nullptr) return;
                for (int i = 0; i < dataSize_; i++)
                {
                    ptr[i] = val;
                }
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
