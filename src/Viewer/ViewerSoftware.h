#pragma once

#include "Viewer.h"
#include "Render/OpenGL/OpenGLUtils.h"
#include "Render/Software/RendererSoft.h"
#include "Render/Software/TextureSoft.h"
#include "Shader/Software/ShaderSoft.h"

namespace SoftGL
{
    namespace View
    {
        #define CASE_CREATE_SHADER_SOFT(shading, source) case shading: \
            return programSoft->SetShaders(std::make_shared<source::VS>(), std::make_shared<source::FS>())
    }
}