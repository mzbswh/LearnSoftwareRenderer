#pragma once

#include "Base/MemoryUtils.h"
#include "Render/Software/ShaderProgramSoft.h"

namespace SoftGL
{
    struct Viewport
    {
        int x = 0;
        int y = 0;
        int width = 0;
        int height = 0;
        float minDepth;
        float maxDepth;

        // ref: https://registry.khronos.org/vulkan/specs/1.0/html/chap24.html#vertexpostproc-viewport
        glm::vec4 innerO;
        glm::vec4 innerP;

        float absMinDepth;
        float absMaxDepth;
    };

    struct VertexHolder
    {
        bool discard = false;
        size_t index = 0;
        
        void *vertex = nullptr;
        float *varyings = nullptr;

        int clipMask = 0;
        glm::aligned_vec4 clipPos = glm::vec4(0.0f);    // clip space position
        glm::aligned_vec4 fragPos = glm::vec4(0.0f);    // screen space position

        std::shared_ptr<uint8_t> vertexHolder = nullptr;
        std::shared_ptr<float> varyingsHolder = nullptr;
    };

    struct PrimitiveHolder
    {
        bool discard = false;
        bool frontFacing = true;
        size_t indices[3] = {0, 0, 0};
    };

    class SampleContext
    {
    public:
        bool inside = false;
        glm::ivec2 fboCoord = glm::ivec2(0);
        glm::aligned_vec4 position = glm::aligned_vec4(0.0f);
        glm::aligned_vec4 barycentric = glm::aligned_vec4(0.0f);
    };

    class PixelContext
    {
    public:
        inline static glm::vec2 *GetSampleLocations4X()
        {
            static glm::vec2 location_4x[4] = 
            {
                {0.375f, 0.875f},
                {0.875f, 0.625f},
                {0.125f, 0.375f},
                {0.625f, 0.125f}
            };
            return location_4x;
        }

        void Init(float x, float y, int sampleCnt = 1)
        {
            inside = false;
            sampleCount = sampleCnt;
            coverage = 0;
            if (sampleCount > 1)
            {
                samples.resize(sampleCount + 1);    // store center sample at end
                if (sampleCount == 4)
                {
                    for (int i = 0; i < sampleCount; i++)
                    {
                        samples[i].fboCoord = glm::ivec2(x, y);
                        samples[i].position = glm::vec4(GetSampleLocations4X()[i] + glm::vec2(x, y), 0.0f, 0.0f);
                    }
                    // pixel center
                    samples[4].fboCoord = glm::ivec2(x, y);
                    samples[4].position = glm::vec4(x + 0.5f, y + 0.5f, 0.0f, 0.0f);
                    sampleShading = &samples[4];
                }
                else
                {
                    // not support
                }
            }
            else
            {
                samples.resize(1);
                samples[0].fboCoord = glm::ivec2(x, y);
                samples[0].position = glm::vec4(x + 0.5f, y + 0.5f, 0.0f, 0.0f);
                sampleShading = &samples[0];
            }
        }

        bool InitCoverage()
        {
            if (sampleCount > 1)
            {
                coverage = 0;
                inside = false;
                for (int i = 0; i < samples.size(); i++)
                {
                    if (samples[i].inside)
                    {
                        coverage++;
                    }
                }
                inside = coverage > 0;
            }
            else
            {
                coverage = 1;
                inside = samples[0].inside;
            }
            return inside;
        }

        void InitShadingSample()
        {
            if (sampleShading->inside)
            {
                return;
            }
            for (auto &sample : samples)
            {
                if (sample.inside)
                {
                    sampleShading = &sample;
                    break;
                }
            }
        }

    public:
        bool inside = false;
        float *varyingsFrag = nullptr;
        std::vector<SampleContext> samples;
        SampleContext *sampleShading = nullptr;
        int sampleCount = 0;
        int coverage = 0;
    };

    class PixelQuadContext
    {
    public:
        void SetVrayingsSize(size_t size)
        {
            if (varyingsAlignedCnt_ != size)
            {
                varyingsAlignedCnt_ = size;
                varyingPool_ = MemoryUtils::makeAlignedBuffer<float>(size * 4);
                for (int i = 0; i < 4; i++)
                {
                    pixels[i].varyingsFrag = varyingPool_.get() + i * varyingsAlignedCnt_;
                }
            }
        }

        void Init(float x, float y, int sampleCnt = 1)
        {
            pixels[0].Init(x, y, sampleCnt);
            pixels[1].Init(x + 1, y, sampleCnt);
            pixels[2].Init(x, y + 1, sampleCnt);
            pixels[3].Init(x + 1, y + 1, sampleCnt);
        }

        bool CheckInside()
        {
            return pixels[0].inside || pixels[1].inside || pixels[2].inside || pixels[3].inside;
        }

    public:
          /**
           *   p2--p3
           *   |   |
           *   p0--p1
           */
          PixelContext pixels[4];
          // triangle vertex screen space position
          glm::aligned_vec4 vertPos[3];
          glm::aligned_vec4 vertPosFlat[4];
          // triangle barycentric correct
          const float *vertZ[3] = {nullptr, nullptr, nullptr};
          glm::aligned_vec4 vertW = glm::aligned_vec4(0.f, 0.f, 0.f, 1.f);
          // triangle vertex shader varyings
          const float *vertVaryings[3] = {nullptr, nullptr, nullptr};
          // triangle Facing
          bool frontFacing = true;
          // shader program
          std::shared_ptr<ShaderProgramSoft> shaderProgram = nullptr;

    private:
        size_t varyingsAlignedCnt_ = 0;
        std::shared_ptr<float> varyingPool_ = nullptr;
    };
}