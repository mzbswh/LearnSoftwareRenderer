#pragma once

#include <vector>
#include <memory>
#include "Base/GLMInc.h"

namespace SoftGL
{
    class VertexArrayObject
    {
    public:
        virtual int getId() = 0;
        virtual void updateVertexData(void *data, size_t length) = 0;
    };

    // only support float type attribute
    struct VertexAttrbuteDesc
    {
        size_t size;
        size_t stride;
        size_t offset;
    };

    struct VertexArray
    {
        size_t vertexSize = 0;
        std::vector<VertexAttrbuteDesc> vertexesDesc;
        
        uint8_t *vertexesBuffer = nullptr;
        size_t vertexesBufferLength = 0;
        
        int32_t *indexBuffer = nullptr;
        size_t indexBufferLength = 0;
    };
}