#pragma once

#include "Base/UUID.h"
#include "Render/Vertex.h"

namespace SoftGL
{
    class VertexArrayObjectSoft : public VertexArrayObject
    {
    public:
        explicit VertexArrayObjectSoft(const VertexArray &vertexArray) 
        {
            // init vertexes
            vertexStride = vertexArray.vertexesDesc[0].stride;
            vertexCnt = vertexArray.vertexesBufferLength / vertexStride;
            vertexes.resize(vertexCnt * vertexStride);
            memcpy(vertexes.data(), vertexArray.vertexesBuffer, vertexArray.vertexesBufferLength);
            // init indices
            indicesCnt = vertexArray.indexBufferLength / sizeof(int32_t);
            indices.resize(indicesCnt);
            memcpy(indices.data(), vertexArray.indexBuffer, vertexArray.indexBufferLength);
        }

        void updateVertexData(void *data, size_t length) override
        {
            memcpy(vertexes.data(), data, std::min(length, vertexes.size()));
        }

        int getId() const override
        {
            return uuid_.get();
        }

    public:
        size_t vertexStride;
        size_t vertexCnt;
        size_t indicesCnt;
        std::vector<uint8_t> vertexes;
        std::vector<int32_t> indices;

    private:
        UUID<VertexArrayObjectSoft> uuid_;
    };
}