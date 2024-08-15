#pragma once

#include "RendererInternal.h"
#include "Base/Geometry.h"
#include "Base/ThreadPool.h"
#include "Render/Renderer.h"
#include "Render/Software/VertexSoft.h"
#include "Render/Software/FrameBufferSoft.h"

namespace SoftGL
{
    class RendererSoft : public Renderer
    {
    public:
        RendererType type() override { return Renderer_Soft; }
        // frameBuffer
        std::shared_ptr<FrameBuffer> createFrameBuffer(bool offscreen) override;
        // texture
        std::shared_ptr<Texture> createTexture(const TextureDesc &desc) override;
        // vertex
        std::shared_ptr<VertexArrayObject> createVertexArrayObject(const VertexArray &vertexArray) override;
        // shader program
        std::shared_ptr<ShaderProgram> createShaderProgram() override;
        // pipiline states
        std::shared_ptr<PipelineStates> createPipelineStates(const RenderStates &renderStates) override;
        // uniform
        std::shared_ptr<UniformBlock> createUniformBlock(const std::string &name, int size) override;
        std::shared_ptr<UniformSampler> createUniformSampler(const std::string &name, const TextureDesc &desc) override;
        // pipeline
        void beginRenderPass(std::shared_ptr<FrameBuffer> &frameBuffer, const ClearStates &states) override;
        void setViewport(int x, int y, int width, int height) override;
        void setVertexArrayObject(std::shared_ptr<VertexArrayObject> &vao) override;
        void setShaderProgram(std::shared_ptr<ShaderProgram> &program) override;
        void setShaderResources(std::shared_ptr<ShaderResources> &uniforms) override;
        void setPipelineStates(std::shared_ptr<PipelineStates> &states) override;
        void draw() override;
        void endRenderPass() override;
        void waitIdle() override;
    
    public:
        inline void setEnableEarlyZ(bool enable) { earlyZ_ = enable; } 
    
    private:
        void processVertexShader();
        void processPrimitiveAssembly();
        void processClipping();
        void processPerspectiveDivide();
        void procesViewportTransform();
        void processFaceCulling();
        void processRasterization();
        void processFragmentShader(glm::vec4 &screenPos, bool frontFacing, void *varyings, ShaderProgram *shader);
        void processPerSampleOperations(int x, int y, float depth, const glm::vec4 &color, int sample);
        bool processColorBlending(int x, int y, glm::vec4 &color, int sample);

        void processPointAssembly();
        void processLineAssembly();
        void processPolygonAssembly();

        void clippingPoint(PrimitiveHolder &point);
        void clippingLine(PrimitiveHolder &line, bool postVertexProcess = false);
        void clippingTriangle(PrimitiveHolder &triangle, std::vector<PrimitiveHolder> &appendPrimitives);

        void interpolateVertex(VertexHolder &out, VertexHolder &v0, VertexHolder &v1, float t);
        void interpolateLinear(float *varsOut, const float *varsIn[2], size_t elemCnt, float t);
        void interpolateBarycentric(float *varsOut, const float *varsIn[3], size_t elemCnt, glm::aligned_vec4 &bc);
        void interpolateBarycentricSIMD(float *varsOut, const float *varsIn[3], size_t elemCnt, glm::aligned_vec4 &bc);

        void rasterizationPoint(VertexHolder *v, float pointSize);
        void rasterizationLine(VertexHolder *v0, VertexHolder *v1, float lineWidth);
        void rasterizationTriangle(VertexHolder *v0, VertexHolder *v1, VertexHolder *v2, bool frontFacing);
        void rasterizationPolygons(std::vector<PrimitiveHolder> &primitives);
        void rasterizationPolygonsPoint(std::vector<PrimitiveHolder> &primitives);
        void rasterizationPolygonsLine(std::vector<PrimitiveHolder> &primitives);
        void rasterizationPolygonsTriangle(std::vector<PrimitiveHolder> &primitives);
        void rasterizationPixelQuad(PixelQuadContext &quad);

        bool earlyZTest(PixelQuadContext &quad);
        void multiSampleResolve();

    private:
        inline RGBA *getFrameColor(int x, int y, int sample);
        inline float *getFrameDepth(int x, int y, int sample);
        inline void setFrameColor(int x, int y, const RGBA &color, int sample);

        size_t clippingNewVertex(size_t idx0, size_t idx1, float t, bool postVertexProcess = false);
        void vertexShaderImpl(VertexHolder &vertex);
        void perspectiveDivideImpl(VertexHolder &vertex);
        void viewportTransformImpl(VertexHolder &vertex);
        int countFrustumClipMask(glm::vec4 &clipPos);
        BoundingBox triangleBoundingBox(glm::vec4 *vert, float width, float height);
        bool barycentric(glm::aligned_vec4 *vert, glm::aligned_vec4 &v0, glm::aligned_vec4 &p, glm::aligned_vec4 &bc);

    private:
        Viewport viewport_{};
        PrimitiveType primitiveType_ = Primitive_TRIANGLE;
        FrameBufferSoft *fbo_ = nullptr;
        const RenderStates *renderStates_ = nullptr;
        VertexArrayObjectSoft *vao_ = nullptr;
        ShaderProgramSoft *shaderProgram_ = nullptr;
        std::shared_ptr<ImageBufferSoft<RGBA>> fboColor_ = nullptr;
        std::shared_ptr<ImageBufferSoft<float>> fboDepth_ = nullptr;
        std::vector<VertexHolder> vertex_;
        std::vector<PrimitiveHolder> primitives_;
        std::shared_ptr<float> varyings_ = nullptr;
        size_t varyingsCnt = 0;
        size_t varyingsAlignedCnt_ = 0;
        size_t varyingsAlgnedSize_ = 0;
        float pointSize_ = 1.0f;
        bool earlyZ_ = true;
        int rasterSamples_ = 1;
        int rasterBlockSize_ = 1;
        ThreadPool threadPool_;
        std::vector<PixelQuadContext> threadQuadCtx_;
    };
}