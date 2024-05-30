#pragma once

#include "Base/GLMInc.h"

namespace SoftGL
{
    enum DepthFunction
    {
        DepthFunc_Never,
        DepthFunc_Less,
        DepthFunc_Equal,
        DepthFunc_LEqual,
        DepthFunc_Greater,
        DepthFunc_NEqual,
        DepthFunc_GEqual,
        DepthFunc_Always,
    };

    enum BlendFactor
    {
        BlendFactor_Zero,
        BlendFactor_One,
        BlendFactor_Src_Color,
        BlendFactor_Src_Alpha,
        BlendFactor_Dst_Color,
        BlendFactor_Dst_Aplha,
        BlendFactor_One_Minus_Src_Color,
        BlendFactor_One_Minus_Src_Alpha,
        BlendFactor_One_Minus_Dst_Color,
        BlendFactor_One_Minus_Dst_Alpha,
    };

    enum BlendFunction
    {
        BlendFunc_Add,
        BlendFunc_Subtract,
        BlendFunc_Reverse_Subtract,
        BlendFunc_Min,
        BlendFunc_Max,
    };

    enum PolygonMode
    {
        PolygonMode_Point,
        PolygonMode_Line,
        PolygonMode_Fill,
    };

    struct BlendParameters
    {
        BlendFunction blendFuncRgb = BlendFunc_Add;
        BlendFactor blendSrcRgb = BlendFactor_One;
        BlendFactor blendDstRgb = BlendFactor_Zero;
        BlendFunction blendFuncAlphs = BlendFunc_Add;
        BlendFactor blendSrcAlpha = BlendFactor_One;
        BlendFactor blendDstAlpha = BlendFactor_Zero;

        void SetBlendFactor(BlendFactor src, BlendFactor dst)
        {
            blendSrcRgb = src;
            blendSrcAlpha = src;
            blendDstRgb = dst;
            blendDstAlpha = dst;
        }

        void SetBlendFunc(BlendFunction func)
        {
            blendFuncRgb = func;
            blendFuncAlphs = func;
        }
    };

    enum PrimitiveType
    {
        Primitive_Point,
        Primitive_Line,
        Primitive_Triangle,
    };

    struct RenderStates
    {
        bool blend = false;
        BlendParameters blendParams;

        bool depthTest = false;
        bool depthMask = true;
        DepthFunction depthFunc = DepthFunc_Less;

        bool cullFace = false;
        PrimitiveType primitiveType = Primitive_Triangle;
        PolygonMode polygonMode = PolygonMode_Fill;

        float lineWidth = 1.f;
    };

    struct ClearStates
    {
        bool depthFlag = false;
        bool colorFlag = false;
        glm::vec4 clearColor = glm::vec4(0.f);
        float clearDepth = 1.f;
    };
}