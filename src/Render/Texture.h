#pragma once

#include <string>
#include <memory>
#include <vector>
#include "Base/Buffer.h"
#include "Base/GLMInc.h"

namespace SoftGL
{
    enum WrapMode
    {
        Wrap_Repeat,
        Wrap_Mirrored_Repeat,
        Wrap_Clamp_To_Edge,
        Wrap_Clamp_To_Border,
    };

    enum FilterMode
    {
        Filter_Nearest,
        Filter_Linear,
        Filter_Nearest_Mipmap_Nearest,
        Filter_Linear_Mipmap_Nearest,
        Filter_Nearest_Mipmap_Linear,
        Filter_Linear_Mipmap_Linear,
    };

    enum CubeMapFace
    {
        Texture_Cube_Map_Positive_X = 0,
        Texture_Cube_Map_Negative_X = 1,
        Texture_Cube_Map_Positive_Y = 2,
        Texture_Cube_Map_Negative_Y = 3,
        Texture_Cube_Map_Positive_Z = 4,
        Texture_Cube_Map_Negative_Z = 5,
    };

    enum BorderColor
    {
        Border_Black = 0,
        Border_White,
    };

    struct SamplerDesc
    {
        FilterMode filterMin = Filter_Nearest;
        FilterMode filterMag = Filter_Nearest;
        WrapMode wrapS = Wrap_Clamp_To_Edge;
        WrapMode wrapT = Wrap_Clamp_To_Edge;
        WrapMode wrapR = Wrap_Clamp_To_Edge;
        BorderColor borderColor = Border_Black;
    };

    enum TextureType
    {
        TextureType_2D,
        TextureType_Cube,
    };

    enum TextureFormat
    {
        TextureFormat_RGBA8 = 0,
        TextureFormat_Float32 = 1,
    };

    enum TextureUsage
    {
        TextureUsage_Sampler = 1 << 0,
        TextureUsage_UploadData = 1 << 1,
        TextureUsage_AttachmentColor = 1 << 2,
        TextureUsage_AttachmentDepth = 1 << 3,
        TextureUsage_RendererOutput = 1 << 4,
    };

    struct TextureDesc
    {
        int width = 0;
        int height = 0;
        TextureType type = TextureType_2D;
        TextureFormat format = TextureFormat_RGBA8;
        uint32_t usage = TextureUsage_Sampler;
        bool ussMipmaps = false;
        bool multiSample = false;
        std::string tag;
    };

    class Texture : public TextureDesc
    {
    public:
        virtual ~Texture() = default;

        inline uint32_t getLevelWidth(uint32_t level)
        {
            return std::max(1, width >> level);
        }

        inline uint32_t getLevelHeight(uint32_t level)
        {
            return std::max(1, height >> level);
        }

        virtual int getId() const = 0;
        virtual void setSamplerDesc(SamplerDesc &sampler) {};
        virtual void initImageData() {};
        virtual void setImageData(const std::vector<std::shared_ptr<Buffer<RGBA>>> &buffers) {};
        virtual void setImageData(const std::vector<std::shared_ptr<Buffer<float>>> &buffers) {};
        virtual void dumpImage(const char *path, uint32_t layer, uint32_t level) = 0;
    };
}