#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include "Base/Geometry.h"
#include "Render/Vertex.h"
#include "Material.h"

namespace SoftGL
{
    namespace View
    {
        struct Vertex
        {
            glm::vec3 a_position;
            glm::vec2 a_texcoord;
            glm::vec3 a_normal;
            glm::vec3 a_tangent;
        };

        struct ModelVertexes : VertexArray
        {
            PrimitiveType primitiveType;
            size_t primitiveCnt = 0;
            std::vector<Vertex> vertexes;
            std::vector<int32_t> indices;
            std::shared_ptr<VertexArrayObject> vao = nullptr;

            void UpdateVertexes() const
            {
                if (vao)
                {
                    vao->updateVertexData(vertexesBuffer, vertexesBufferLength);
                }
            }

            void InitVertexes()
            {
                vertexSize = sizeof(Vertex);

                vertexesDesc.resize(4);
                vertexesDesc[0] = {3, vertexSize, offsetof(Vertex, a_position)};
                vertexesDesc[1] = {2, vertexSize, offsetof(Vertex, a_texcoord)};
                vertexesDesc[2] = {3, vertexSize, offsetof(Vertex, a_normal)};
                vertexesDesc[3] = {3, vertexSize, offsetof(Vertex, a_tangent)};
                
                vertexesBuffer = vertexes.empty() ? nullptr : (uint8_t*) &vertexes[0];
                vertexesBufferLength = vertexes.size() * vertexSize;

                indexBuffer = indices.empty() ? nullptr : &indices[0];
                indexBufferLength = indices.size() * sizeof(int32_t);
            }
        };

        struct ModelBase : ModelVertexes
        {
            BoundingBox aabb{};
            std::shared_ptr<Material> material = nullptr;

            virtual void resetStates()
            {
                vao = nullptr;
                if (material)
                {
                    material->resetStates();
                }
            }
        };

        struct ModelPoints : ModelBase {};
        struct ModelLines : ModelBase {};
        struct ModelMesh : ModelBase {};

        struct ModelNode
        {
            glm::mat4 transform = glm::mat4(1.0f);
            std::vector<ModelMesh> meshes;
            std::vector<ModelNode> children;
        };

        struct Model
        {
            std::string resourcePath;
            ModelNode rootNode;
            BoundingBox rootAABB;

            size_t meshCnt = 0;
            size_t primitiveCnt = 0;
            size_t vertexCnt = 0;

            glm::mat4 centeredTransform;

            void resetStates()
            {
                resetNodeStates(rootNode);   
            }

            void resetNodeStates(ModelNode &node)
            {
                for (auto &mesh : node.meshes)
                {
                    mesh.resetStates();
                }
                for (auto &child : node.children)
                {
                    resetNodeStates(child);
                }
            }
        };

        struct DemoScene
        {
            std::shared_ptr<Model> model;
            ModelLines worldAxis;
            ModelPoints pointLight;
            ModelMesh floor;
            ModelMesh skybox;

            void resetStates()
            {
                if (model)
                {
                    model->resetStates();
                }
                worldAxis.resetStates();
                pointLight.resetStates();
                floor.resetStates();
                skybox.resetStates();
            }
        };
    }
}