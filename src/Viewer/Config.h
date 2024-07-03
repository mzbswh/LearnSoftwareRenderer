#pragma once

#include <string>
#include "Base/GLMInc.h"
#include "Render/Renderer.h"

namespace SoftGL
{
    namespace View
    {
        const std::string ASSETS_DIR = "./assets/";
        const std::string Shader_GLSL_DIR = "./shaders/GLSL/";

        enum AAType
        {
            AAType_None,
            AAType_MSAA,
            AAType_FXAA,
        };

        class Config
        {
        public:
            std::string modelName;
            std::string modelPath;
            std::string skyboxName;
            std::string skyboxPath;

            size_t triangleCount_ = 0;

            bool wireframe = false;
            bool worldAxis = true;
            bool showSkybox = false;
            bool showFloor = true;

            bool shadowMap = true;
            bool pbrIbl = false;    // Image Based Lighting
            bool mipmaps = false;

            bool cullFace = true;
            bool depthTest = true;
            bool reverseZ = false;

            glm::vec4 clearColor = {0.f, 0.f, 0.f, 0.f};
            glm::vec3 ambientColor = {0.5f, 0.5f, 0.5f};

            bool showLight = true;
            glm::vec3 pointLightPosition = {0.f, 0.f, 0.f};
            glm::vec3 pointLightColor = {0.5f, 0.5f, 0.5f};

            int aaType = AAType_None;
            int rendererType = SoftGL::Renderer_Soft;
        };
    }
}