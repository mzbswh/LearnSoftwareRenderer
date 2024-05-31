#pragma once

#include "Base/Buffer.h"
#include "FrameBuffer.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "PipelineStates.h"
#include "Vertex.h"

namespace SoftGL
{
    enum RendererType
    {
        Renderer_Soft,
        Renderer_OpenGL,
        Renderer_Vulkan,
    };

    class Renderer
    {
    public:
        virtual RendererType type() = 0;
        virtual bool create() { return true; };
        virtual void destroy() {};

        // frameBuffer
        virtual std::shared_ptr<FrameBuffer> createFrameBuffer(bool offscreen) = 0;

        // texture
        virtual std::shared_ptr<Texture> createTexture(const TextureDesc &desc) = 0;

        // shader program
        virtual std::shared_ptr<ShaderProgram> createShaderProgram() = 0;

        // pipiline states
        virtual std::shared_ptr<PipelineStates> createPipelineStates(const RenderStates &renderStates) = 0;

        // uniform
        virtual std::shared_ptr<UniformBlock> createUnifromBlock(const std::string &name, int size) = 0;
        virtual std::shared_ptr<UniformSampler> createUniformSampler(const std::string &name, const TextureDesc &desc) = 0;

        // pipeline
        virtual void beginRenderPass(std::shared_ptr<FrameBuffer> &frameBuffer, const ClearStates &states) = 0;
        virtual void setViewport(int x, int y, int width, int height) = 0;
        virtual void setVertexArrayObject(std::shared_ptr<VertexArrayObject> &vao) = 0;
        virtual void setShaderProgram(std::shared_ptr<ShaderProgram> &program) = 0;
        virtual void setShaderResources(std::shared_ptr<ShaderResources> &uniforms) = 0;
        virtual void setPipelineStates(std::shared_ptr<PipelineStates> &states) = 0;
        virtual void draw() = 0;
        virtual void endRenderPass() = 0;
        virtual void waitIdle() = 0;
    };
}