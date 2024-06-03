#pragma once

#include <glad/glad.h>
#include "Base/ImageUtils.h"
#include "Render/Texture.h"
#include "Render/OpenGL/EnumsOpenGL.h"
#include "Render/OpenGL/OpenGLUtils.h"

namespace SoftGL
{
    struct TextureOpenGLDesc
    {
        GLint internalformat;
        GLenum format;
        GLenum type;
    };
}