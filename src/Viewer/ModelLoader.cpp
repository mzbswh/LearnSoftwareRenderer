#include "ModelLoader.h"
#include <iostream>
#include <set>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/GltfMaterial.h>
#include "Base/ImageUtils.h"
#include "Base/ThreadPool.h"
#include "Base/StringUtils.h"
#include "Base/Logger.h"
#include "Cube.h"

namespace SoftGL
{
    namespace View
    {
        ModelLoader::ModelLoader(Config &config) : config_(config)
        {
            loadWorldAxis();
            loadLights();
            loadFloor();
        }

        void ModelLoader::loadCubeMesh(ModelVertexes &mesh)
        {
            const float *cubeVertexes = Cube::getCubeVertexes();
            mesh.primitiveType = Primitive_TRIANGLE;
            mesh.primitiveCnt = 12;
            for (int i = 0; i < 12; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    Vertex vertex{};
                    vertex.a_position.x = cubeVertexes[i * 9 + j * 3 + 0];
                    vertex.a_position.y = cubeVertexes[i * 9 + j * 3 + 1];
                    vertex.a_position.z = cubeVertexes[i * 9 + j * 3 + 2];
                    mesh.vertexes.push_back(vertex);
                    mesh.indices.push_back(i * 3 + j);
                }
            }
            mesh.InitVertexes();
        }

        void ModelLoader::loadWorldAxis()
        {
            float axisY = -0.01f;
            int idx = 0;
            for (int i = -16; i <= 16; i++)
            {
                scene_.worldAxis.vertexes.push_back({glm::vec3(-3.2, axisY, 0.2f * (float)i)});
                scene_.worldAxis.vertexes.push_back({glm::vec3(3.2, axisY, 0.2f * (float)i)});
                scene_.worldAxis.indices.push_back(idx++);
                scene_.worldAxis.indices.push_back(idx++);

                scene_.worldAxis.vertexes.push_back({glm::vec3(0.2f * (float)i, axisY, -3.2)});
                scene_.worldAxis.vertexes.push_back({glm::vec3(0.2f * (float)i, axisY, 3.2)});
                scene_.worldAxis.indices.push_back(idx++);
                scene_.worldAxis.indices.push_back(idx++);
            }
            scene_.worldAxis.primitiveType = Primitive_LINE;
            scene_.worldAxis.primitiveCnt = scene_.worldAxis.indices.size() / 2;
            scene_.worldAxis.material = std::make_shared<Material>();
            scene_.worldAxis.material->shadingModel = Shading_BaseColor;
            scene_.worldAxis.material->baseColor = glm::vec4(0.25f, 0.25f, 0.25f, 1.0f);
            scene_.worldAxis.materia->lineWidth = 1.0f;
        }

        void ModelLoader::loadLights()
        {
            scene_.pointLight.primitiveType = Primitive_POINT;
            scene_.pointLight.primitiveCnt = 1;
            scene_.pointLight.vertexes.resize(scene_.pointLight.primitiveCnt);
            scene_.pointLight.indices.resize(scene_.pointLight.primitiveCnt);

            scene_.pointLight.vertexes[0] = {config_.pointLightPosition};
            scene_.pointLight.indices[0] = 0;
            scene_.pointLight.material = std::make_shared<Material>();
            scene_.pointLight.material->shadingModel = Shading_BaseColor;
            scene_.pointLight.material->baseColor = glm::vec4(config_.pointLightColor, 1.0f);
            scene_.pointLight.material->pointSize = 10.0f;
            scene_.pointLight.InitVertexes();
        }

        void ModelLoader::loadFloor()
        {
            float floorY = 0.01f;
            float floorSize = 2.0f;
            scene_.floor.vertexes.push_back({glm::vec3(-floorSize, floorY, floorSize), 
                                             glm::vec2(0.0f, 1.0f),
                                             glm::vec3(0.0f, 1.0f, 0.0f)});
            scene_.floor.vertexes.push_back({glm::vec3(-floorSize, floorY, -floorSize),
                                             glm::vec2(0.0f, 0.0f),
                                             glm::vec3(0.0f, 1.0f, 0.0f)}); 
            scene_.floor.vertexes.push_back({glm::vec3(floorSize, floorY, -floorSize),
                                             glm::vec2(1.0f, 0.0f),
                                             glm::vec3(0.0f, 1.0f, 0.0f)}); 
            scene_.floor.vertexes.push_back({glm::vec3(floorSize, floorY, floorSize),
                                             glm::vec2(1.0f, 1.0f),
                                             glm::vec3(0.0f, 1.0f, 0.0f)});
            scene_.floor.indices.push_back(0);
            scene_.floor.indices.push_back(2);
            scene_.floor.indices.push_back(1);
            scene_.floor.indices.push_back(0);
            scene_.floor.indices.push_back(3);
            scene_.floor.indices.push_back(2);

            scene_.floor.primitiveType = Primitive_TRIANGLE;
            scene_.floor.primitiveCnt = 2;

            scene_.floor.material = std::make_shared<Material>();
            scene_.floor.material->shadingModel = Shading_BlinnPhong
            scene_.floor.material->baseColor = glm::vec4(1.0f);
            scene_.floor.material->doubleSided = true;

            scene_.floor.aabb = BoundingBox(glm::vec3(-2, 0, -2), glm::vec3(2, 0, 2));
            scene_.floor.InitVertexes();
        }

        bool ModelLoader::loadSkybox(const std::string &filepath)
        {
            if (filepath.empty())
            {
                return false;
            }
            if (scene_.skybox.primitiveCnt <= 0)
            {
                loadCubeMesh(scene_.skybox);
            }

            auto it = skyboxMaterialCache_.find(filepath);
            if (it != skyboxMaterialCache_.end())
            {
                scene_.skybox.material = it->second;
                return true;
            }

            LOGD("load skybox, path: %s", filepath.c_str());
            auto material = std::make_shared<Material>();
            material->shadingModel = Shading_Skybox;

            std::vector<std::shared_ptr<Buffer<RGBA>>> skyboxTex;
            if (StringUtils::endsWith(filepath, "/"))
            {
                skyboxTex.resize(6);
                ThreadPool pool(6);
                pool.pushTask([&](int thread_id) { skyboxTex[0] = loadTextureFile(filepath + "right.jpg"); });
                pool.pushTask([&](int thread_id) { skyboxTex[1] = loadTextureFile(filepath + "left.jpg"); });
                pool.pushTask([&](int thread_id) { skyboxTex[2] = loadTextureFile(filepath + "top.jpg"); });
                pool.pushTask([&](int thread_id) { skyboxTex[3] = loadTextureFile(filepath + "bottom.jpg"); });
                pool.pushTask([&](int thread_id) { skyboxTex[4] = loadTextureFile(filepath + "front.jpg"); });
                pool.pushTask([&](int thread_id) { skyboxTex[5] = loadTextureFile(filepath + "back.jpg"); });
                pool.waitTasksFinish();

                auto &texData = material->textureData[MaterialTexType_CUBE];
                texData.width = skyboxTex[0]->width();
                texData.height = skyboxTex[0]->height();
                texData.data = std::move(skyboxTex);
                texData.wrapModeU = Wrap_CLAMP_TO_EDGE;
                texData.wrapModeV = Wrap_CLAMP_TO_EDGE;
                texData.wrapModeW = Wrap_CLAMP_TO_EDGE;
            }
            else
            {
                skyboxTex.resize(1);
                skyboxTex[0] = loadTextureFile(filepath);
                auto &texData = material->textureData[MaterialTexType_EQUIRECTANGULAR];
                texData.tag = filepath;
                texData.width = skyboxTex[0]->width();
                texData.height = skyboxTex[0]->height();
                texData.data = std::move(skyboxTex);
                texData.wrapModeU = Wrap_CLAMP_TO_EDGE;
                texData.wrapModeV = Wrap_CLAMP_TO_EDGE;
                texData.wrapModeW = Wrap_CLAMP_TO_EDGE;
            }
            skyboxMaterialCache_[filepath] = material;
            scene_.skybox.material = material;
            return true;
        }

        bool ModelLoader::loadModel(const std::string &filepath)
        {
            std::lock_guard<std::mutex> lk(modelLoadMutex_);
            if (filepath.empty())
            {
                return false;
            }
            auto it = modelCache_.find(filepath);
            if (it != modelCache_.end())
            {
                scene_.model = it->second;
                return true;
            }
            modelCache_[filepath] = std::make_shared<Model>();
            scene_.model = modelCache_[filepath];
            LOGD("load model, path: %s", filepath.c_str());
            // load model
            Assimp::Importer importer;
            if (filepath.empty())
            {
                LOGE("ModelLoader::loadModel, empty model file path.");
                return false;
            }
            const aiScene *scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_FlipUVs | aiProcess_GenBoundingBoxes);
            if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
            {
                LOGE("ModelLoader::loadModel, failed to load model, desc: %s", importer.GetErrorString());
                return false;
            }
            scene_.model->resourcePath = filepath.substr(0, filepath.find_last_of('/'));
            // preload textures
            preloadTextureFiles(scene, scene_.model->resourcePath);
            // processNode
            auto currTransform = glm::mat4(1.0f);
            if (!processNode(scene->mRootNode, scene, scene_.model->rootNode, currTransform))
            {
                LOGE("ModelLoader::loadModel, failed to process model node.");
                return false;
            }
            // model center transform
            scene_.model->centeredTransform = adjustModelCenter(scene_.model->rootAABB);
            return true;
        }

        bool ModelLoader::processNode(const aiNode *ai_node, const aiScene *ai_scene, ModelNode &outNode, glm::mat4 &transform)
        {
            if (!ai_node)
            {
                return false;
            }
            outNode.transform = convertMatrix(ai_node->mTransformation);
            auto currTransform = transform * outNode.transform;
            for (size_t i = 0; i < ai_node->mNumMeshes; i++)
            {
                const aiMesh *meshPtr = ai_scene->mMeshes[ai_node->mMeshes[i]];
                if (meshPtr)
                {
                    ModelMesh mesh;
                    if (processMesh(meshPtr, ai_scene, mesh))
                    {
                        scene_.model->meshCnt++;
                        scene_.model->primitiveCnt += mesh.primitiveCnt;
                        scene_.model->vertexCnt += mesh.vertexes.size();

                        // bounding box
                        auto bounds = mesh.aabb.transform(currTransform);
                        scene_.model->rootAABB.merge(bounds);
                        outNode.meshes.push_back(std::move(mesh));
                    }
                }
            }
            for (size_t i = 0; i < aiNode->mNumChildren; i++)
            {
                ModelNode childNode;
                if (processNode(ai_node->mChildren[i], ai_scene, childNode, currTransform))
                {
                    outNode.children.push_back(std::move(childNode));
                }
            }
            return true;
        }
    }
}