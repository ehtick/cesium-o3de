#include "GltfModelComponent.h"
#include "BitangentAndTangentGenerator.h"
#include <CesiumUtility/Math.h>
#include <Atom/RPI.Reflect/Buffer/BufferAssetCreator.h>
#include <Atom/RPI.Reflect/Model/ModelLodAsset.h>
#include <Atom/RPI.Reflect/Model/ModelLodAssetCreator.h>
#include <Atom/RPI.Reflect/Model/ModelAssetCreator.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Cesium
{
    struct GltfModelComponent::GltfLoadContext
    {
    };

    struct GltfModelComponent::GltfUVConverter
    {
        template<typename T>
        bool operator()([[maybe_unused]] const CesiumGltf::AccessorView<T>& colorAccessorView)
        {
            return false;
        }

        template<typename T>
        bool operator()(const CesiumGltf::AccessorView<CesiumGltf::AccessorTypes::VEC2<T>>& uvAccessorView)
        {
            if (uvAccessorView.status() != CesiumGltf::AccessorViewStatus::Valid)
            {
                return false;
            }

            if (m_generateUnIndexedMesh && m_indicesAccessorView.size() > 0)
            {
                m_uvs.resize(static_cast<std::size_t>(m_indicesAccessorView.size()));
                for (std::int64_t i = 0; i < m_indicesAccessorView.size(); ++i)
                {
                    std::int64_t index = m_indicesAccessorView[i];
                    const auto& uv = uvAccessorView[index];
                    float u = decodeUV(uv.value[0]);
                    float v = decodeUV(uv.value[1]);
                    m_uvs[static_cast<std::size_t>(i)] = glm::vec2(u, v);
                }
            }
            else
            {
                m_uvs.resize(uvAccessorView.size());
                for (std::int64_t i = 0; i < uvAccessorView.size(); ++i)
                {
                    const auto& uv = uvAccessorView[i];
                    float u = decodeUV(uv.value[0]);
                    float v = decodeUV(uv.value[1]);
                    m_uvs[static_cast<std::size_t>(i)] = glm::vec2(u, v);
                }
            }

            return true;
        }

        template<typename T>
        float decodeUV([[maybe_unused]] T c)
        {
            return 0.0f;
        }

        float decodeUV(float c)
        {
            return c;
        }

        float decodeUV(std::uint8_t c)
        {
            return c / 256.0f;
        }

        float decodeUV(std::uint16_t c)
        {
            return c / 65536.0f;
        }

        CesiumGltf::AccessorView<std::uint32_t> m_indicesAccessorView;
        AZStd::vector<glm::vec2> m_uvs;
        bool m_generateUnIndexedMesh;
    };

    struct GltfModelComponent::GltfColorConverter
    {
        template<typename T>
        bool operator()([[maybe_unused]] const CesiumGltf::AccessorView<T>& colorAccessorView)
        {
            return false;
        }

        template<typename T>
        bool operator()(const CesiumGltf::AccessorView<CesiumGltf::AccessorTypes::VEC3<T>>& colorAccessorView)
        {
            if (colorAccessorView.status() != CesiumGltf::AccessorViewStatus::Valid)
            {
                return false;
            }

            if (m_generateUnIndexedMesh && m_indicesAccessorView.size() > 0)
            {
                m_colors.resize(static_cast<std::size_t>(m_indicesAccessorView.size()));
                for (std::int64_t i = 0; i < m_indicesAccessorView.size(); ++i)
                {
                    std::int64_t index = m_indicesAccessorView[i];
                    const auto& color = colorAccessorView[index];
                    float red = decodeColor(color.value[0]);
                    float blue = decodeColor(color.value[1]);
                    float green = decodeColor(color.value[2]);
                    m_colors[static_cast<std::size_t>(i)] = glm::vec4(red, blue, green, 1.0f);
                }
            }
            else
            {
                m_colors.resize(colorAccessorView.size());
                for (std::int64_t i = 0; i < colorAccessorView.size(); ++i)
                {
                    const auto& color = colorAccessorView[i];
                    float red = decodeColor(color.value[0]);
                    float blue = decodeColor(color.value[1]);
                    float green = decodeColor(color.value[2]);
                    m_colors[static_cast<std::size_t>(i)] = glm::vec4(red, blue, green, 1.0f);
                }
            }

            return true;
        }

        template<typename T>
        bool operator()(const CesiumGltf::AccessorView<CesiumGltf::AccessorTypes::VEC4<T>>& colorAccessorView)
        {
            if (colorAccessorView.status() != CesiumGltf::AccessorViewStatus::Valid)
            {
                return false;
            }

            if (m_generateUnIndexedMesh && m_indicesAccessorView.size() > 0)
            {
                m_colors.resize(static_cast<std::size_t>(m_indicesAccessorView.size()));
                for (std::int64_t i = 0; i < m_indicesAccessorView.size(); ++i)
                {
                    std::int64_t index = m_indicesAccessorView[i];
                    const auto& color = colorAccessorView[index];
                    float red = decodeColor(color.value[0]);
                    float blue = decodeColor(color.value[1]);
                    float green = decodeColor(color.value[2]);
                    float alpha = decodeColor(color.value[3]);
                    m_colors[static_cast<std::size_t>(i)] = glm::vec4(red, blue, green, alpha);
                }
            }
            else
            {
                m_colors.resize(colorAccessorView.size());
                for (std::int64_t i = 0; i < colorAccessorView.size(); ++i)
                {
                    const auto& color = colorAccessorView[i];
                    float red = decodeColor(color.value[0]);
                    float blue = decodeColor(color.value[1]);
                    float green = decodeColor(color.value[2]);
                    float alpha = decodeColor(color.value[3]);
                    m_colors[static_cast<std::size_t>(i)] = glm::vec4(red, blue, green, alpha);
                }
            }

            return true;
        }

        template<typename T>
        float decodeColor([[maybe_unused]] T c)
        {
            return 0.0f;
        }

        float decodeColor(float c)
        {
            return c;
        }

        float decodeColor(std::uint8_t c)
        {
            return c / 256.0f;
        }

        float decodeColor(std::uint16_t c)
        {
            return c / 65536.0f;
        }

        CesiumGltf::AccessorView<std::uint32_t> m_indicesAccessorView;
        AZStd::vector<glm::vec4> m_colors;
        bool m_generateUnIndexedMesh;
    };

    void GltfModelComponent::LoadModel(const CesiumGltf::Model& model)
    {
        GltfLoadContext loadContext{};

        if (model.scene >= 0 && model.scene < model.scenes.size())
        {
            // display default scene
            LoadScene(model, model.scenes[model.scene], loadContext);
        }
        else if (model.scenes.size() > 0)
        {
            // no default scene, display the first one
            LoadScene(model, model.scenes.front(), loadContext);
        }
        else
        {
            // load all meshes in the gltf
            for (const auto& mesh : model.meshes)
            {
                LoadMesh(model, mesh, glm::dmat4(1.0), loadContext);
            }
        }
    }

    void GltfModelComponent::LoadScene(const CesiumGltf::Model& model, const CesiumGltf::Scene& scene, GltfLoadContext& loadContext)
    {
        glm::dmat4 parentTransform(1.0);
        for (std::int32_t rootIndex : scene.nodes)
        {
            if (rootIndex >= 0 && rootIndex <= model.nodes.size())
            {
                LoadNode(model, model.nodes[static_cast<std::size_t>(rootIndex)], parentTransform, loadContext);
            }
        }
    }

    void GltfModelComponent::LoadNode(
        const CesiumGltf::Model& model, const CesiumGltf::Node& node, const glm::dmat4& parentTransform, GltfLoadContext& loadContext)
    {
        glm::dmat4 currentTransform = parentTransform;
        if (node.matrix.size() == 16 && !IsIdentityMatrix(node.matrix))
        {
            currentTransform *= glm::dmat4(
                glm::dvec4(node.matrix[0], node.matrix[1], node.matrix[2], node.matrix[3]),
                glm::dvec4(node.matrix[4], node.matrix[5], node.matrix[6], node.matrix[7]),
                glm::dvec4(node.matrix[8], node.matrix[9], node.matrix[10], node.matrix[11]),
                glm::dvec4(node.matrix[12], node.matrix[13], node.matrix[14], node.matrix[15]));
        }
        else
        {
            if (node.translation.size() == 3)
            {
                currentTransform =
                    glm::translate(currentTransform, glm::dvec3(node.translation[0], node.translation[1], node.translation[2]));
            }

            if (node.rotation.size() == 4)
            {
                currentTransform *= glm::dmat4(glm::dquat(node.rotation[0], node.rotation[1], node.rotation[2], node.rotation[3]));
            }

            if (node.scale.size() == 3)
            {
                currentTransform = glm::scale(currentTransform, glm::dvec3(node.scale[0], node.scale[1], node.scale[2]));
            }
        }

        if (node.mesh >= 0 && node.mesh <= model.meshes.size())
        {
            LoadMesh(model, model.meshes[static_cast<std::size_t>(node.mesh)], currentTransform, loadContext);
        }

        for (std::int32_t child : node.children)
        {
            if (child >= 0 && child < node.children.size())
            {
                LoadNode(model, model.nodes[static_cast<std::size_t>(child)], currentTransform, loadContext);
            }
        }
    }

    void GltfModelComponent::LoadMesh(
        const CesiumGltf::Model& model, const CesiumGltf::Mesh& mesh, const glm::dmat4& transform, GltfLoadContext& loadContext)
    {
        for (const CesiumGltf::MeshPrimitive& primitive : mesh.primitives)
        {
            LoadPrimitive(model, primitive, transform, loadContext);
        }
    }

    void GltfModelComponent::LoadPrimitive(
        const CesiumGltf::Model& model,
        const CesiumGltf::MeshPrimitive& primitive,
        const glm::dmat4& transform,
        GltfLoadContext& loadContext)
    {
        // create primitive handle
        auto modelAsset = CreateModelAsset(model, primitive, loadContext);
        if (!modelAsset)
        {
            // Cannot create asset, skip rendering it
            return;
        }

        auto materialInstance = CreateMaterialInstance(model, primitive.material, loadContext);
        auto primitiveHandle = m_meshFeatureProcessor->AcquireMesh(AZ::Render::MeshHandleDescriptor{ modelAsset }, materialInstance);

        // set transformation
        AZ::Matrix3x4 o3deMatrix = AZ::Matrix3x4::CreateFromColumns(
            AZ::Vector3(transform[0][0], transform[0][1], transform[0][2]), AZ::Vector3(transform[1][0], transform[1][1], transform[1][2]),
            AZ::Vector3(transform[2][0], transform[2][1], transform[2][2]), AZ::Vector3(transform[3][0], transform[3][1], transform[3][2]));
        AZ::Transform o3deTransform = AZ::Transform::CreateFromMatrix3x4(o3deMatrix);
        m_meshFeatureProcessor->SetTransform(primitiveHandle, o3deTransform);

        // save the handle
        m_primitives.emplace_back(std::move(primitiveHandle));
    }

    AZ::Data::Instance<AZ::RPI::Material> GltfModelComponent::CreateMaterialInstance(
        [[maybe_unused]] const CesiumGltf::Model& model,
        [[maybe_unused]] std::int32_t primitiveMaterial,
        [[maybe_unused]] GltfLoadContext& loadContext)
    {
        return AZ::Data::Instance<AZ::RPI::Material>();
    }

    AZ::Data::Asset<AZ::RPI::MaterialAsset> GltfModelComponent::CreateMaterialAsset(
        [[maybe_unused]] const CesiumGltf::Model& model,
        [[maybe_unused]] const CesiumGltf::Material& material,
        [[maybe_unused]] GltfLoadContext& loadContext)
    {
        return AZ::Data::Asset<AZ::RPI::MaterialAsset>();
    }

    AZ::Data::Asset<AZ::RPI::ImageAsset> GltfModelComponent::CreateImageAsset(
        [[maybe_unused]] const CesiumGltf::Model& model,
        [[maybe_unused]] const CesiumGltf::Image& image,
        [[maybe_unused]] GltfLoadContext& loadContext)
    {
        return AZ::Data::Asset<AZ::RPI::ImageAsset>();
    }

    AZ::Data::Asset<AZ::RPI::ModelAsset> GltfModelComponent::CreateModelAsset(
        const CesiumGltf::Model& model, const CesiumGltf::MeshPrimitive& primitive, [[maybe_unused]] GltfLoadContext& loadContext)
    {
        // retrieve required positions first
        auto positionAttribute = primitive.attributes.find("POSITION");
        if (positionAttribute == primitive.attributes.end())
        {
            return AZ::Data::Asset<AZ::RPI::ModelAsset>();
        }

        auto positionAccessor = model.getSafe<CesiumGltf::Accessor>(&model.accessors, positionAttribute->second);
        if (!positionAccessor)
        {
            return AZ::Data::Asset<AZ::RPI::ModelAsset>();
        }

        auto positionAccessorView = CesiumGltf::AccessorView<glm::vec3>(model, *positionAccessor);
        if (positionAccessorView.status() != CesiumGltf::AccessorViewStatus::Valid)
        {
            return AZ::Data::Asset<AZ::RPI::ModelAsset>();
        }

        // construct bounding volume
        AZ::Aabb aabb = AZ::Aabb::CreateNull();
        if (positionAccessor->min.size() == 3 && positionAccessor->max.size() == 3)
        {
            aabb = AZ::Aabb::CreateFromMinMaxValues(
                positionAccessor->min[0], positionAccessor->min[1], positionAccessor->min[2], positionAccessor->max[0],
                positionAccessor->max[1], positionAccessor->max[2]);
        }
        else
        {
            aabb = CreateAabbFromPositions(positionAccessorView);
        }

        // get index view
        CesiumGltf::AccessorView<std::uint32_t> indicesAccessorView{ CesiumGltf::AccessorViewStatus::Valid };
        auto indicesAccessor = model.getSafe<CesiumGltf::Accessor>(&model.accessors, primitive.indices);
        if (indicesAccessor)
        {
            indicesAccessorView = CesiumGltf::AccessorView<std::uint32_t>(model, *indicesAccessor);
            if (indicesAccessorView.status() != CesiumGltf::AccessorViewStatus::Valid)
            {
                // Gltf primitive says it has indices, but turn out it's not valid. We terminate the parsing right here
                return AZ::Data::Asset<AZ::RPI::ModelAsset>();
            }
        }

        // check if we should generate normal
        bool generateFlatNormal = false;
        CesiumGltf::AccessorView<glm::vec3> normalsAccessorView;
        auto normalAttribute = primitive.attributes.find("NORMAL");
        if (normalAttribute == primitive.attributes.end())
        {
            generateFlatNormal = true;
        }
        else
        {
            normalsAccessorView = CesiumGltf::AccessorView<glm::vec3>(model, normalAttribute->second);
            bool isNormalAccessorValid = normalsAccessorView.status() == CesiumGltf::AccessorViewStatus::Valid;
            bool hasEnoughNormalVertices = normalsAccessorView.size() == positionAccessorView.size();
            generateFlatNormal = !isNormalAccessorValid || !hasEnoughNormalVertices;
        }

        // check if we should generate tangent
        bool generateTangent = false;
        CesiumGltf::AccessorView<glm::vec4> tangentsAccessorView;
        auto tangentAttribute = primitive.attributes.find("TANGENT");
        if (tangentAttribute == primitive.attributes.end())
        {
            generateTangent = true;
        }
        else
        {
            tangentsAccessorView = CesiumGltf::AccessorView<glm::vec4>(model, tangentAttribute->second);
            bool isTangentAccessorValid = tangentsAccessorView.status() == CesiumGltf::AccessorViewStatus::Valid;
            bool hasEnoughTangentVertices = tangentsAccessorView.size() == positionAccessorView.size();
            generateTangent = !isTangentAccessorValid || !hasEnoughTangentVertices;
        }

        // check if we should generate unindexed mesh
        bool hasNoIndices = indicesAccessorView.size() == 0;
        bool generateUnIndexedMesh = generateFlatNormal || generateTangent || hasNoIndices;

        // copy position accessor to vector
        AZStd::vector<glm::vec3> positions;
        if (generateUnIndexedMesh)
        {
            CreateUnIndexedAttribute(indicesAccessorView, positionAccessorView, positions);
        }
        else
        {
            positions.resize(static_cast<std::size_t>(positionAccessorView.size()), glm::vec3(0.0f));
            for (std::int64_t i = 0; i < positionAccessorView.size(); ++i)
            {
                positions[static_cast<std::size_t>(i)] = positionAccessorView[i];
            }
        }

        // copy uv accessor to vector
        AZStd::vector<glm::vec2> uv_0 = CreateUVAttribute(model, primitive, indicesAccessorView, generateUnIndexedMesh, 0);
        AZStd::vector<glm::vec2> uv_1 = CreateUVAttribute(model, primitive, indicesAccessorView, generateUnIndexedMesh, 1);

        // copy normal accessor to vector
        AZStd::vector<glm::vec3> normals;
        if (generateFlatNormal)
        {
            // if we are at this point, positions is already un-indexed
            assert(generateUnIndexedMesh);
            CreateFlatNormal(positions, normals);
        }
        else if (generateUnIndexedMesh)
        {
            CreateUnIndexedAttribute(indicesAccessorView, normalsAccessorView, normals);
        }
        else
        {
            normals.resize(static_cast<std::size_t>(normalsAccessorView.size()), glm::vec3(0.0f));
            for (std::int64_t i = 0; i < normalsAccessorView.size(); ++i)
            {
                normals[static_cast<std::size_t>(i)] = normalsAccessorView[i];
            }
        }

        // copy tangent and bitangent accessor to vector
        AZStd::vector<glm::vec4> tangents;
        AZStd::vector<glm::vec3> bitangents;
        if (generateTangent)
        {
            // positions, normals, and uvs should be unindexed at this point
            assert(generateUnIndexedMesh);

            // Try first uvs. Generator can deal with empty UVs
            bool tangentSuccess = BitangentAndTangentGenerator::Generate(positions, normals, uv_0, tangents, bitangents);

            // try again with the second uvs
            if (!tangentSuccess && !uv_1.empty())
            {
                tangentSuccess = BitangentAndTangentGenerator::Generate(positions, normals, uv_1, tangents, bitangents);
            }

            // if we still cannot generate MikkTSpace, then we generate dummy
            if (!tangentSuccess)
            {
                tangents.resize(positions.size(), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
                bitangents.resize(positions.size(), glm::vec3(0.0f, 0.0f, 0.0f));
            }
        }
        else if (generateUnIndexedMesh)
        {
            // copy tangents to vector
            CreateUnIndexedAttribute(indicesAccessorView, tangentsAccessorView, tangents);

            // create bitangents
            bitangents.resize(tangents.size(), glm::vec3(0.0f));
            for (std::size_t i = 0; i < tangents.size(); ++i)
            {
                bitangents[i] = glm::cross(normals[i], glm::vec3(tangents[i])) * tangents[i].w;
            }
        }
        else
        {
            // populate tangent and bitangent vector
            tangents.resize(static_cast<std::size_t>(tangentsAccessorView.size()), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
            bitangents.resize(tangents.size(), glm::vec3(0.0f));
            for (std::int64_t i = 0; i < tangentsAccessorView.size(); ++i)
            {
                tangents[static_cast<std::size_t>(i)] = tangentsAccessorView[i];
                bitangents[i] = glm::cross(normals[i], glm::vec3(tangents[i])) * tangents[i].w;
            }
        }

        // copy color to vectors
        AZStd::vector<glm::vec4> colors = CreateColorAttribute(model, primitive, indicesAccessorView, generateUnIndexedMesh);

        // create buffer assets
        auto indicesBuffer = CreateIndicesBufferAsset(model, *indicesAccessor, indicesAccessorView);
        auto positionBuffer = CreateBufferAsset(positions.data(), positions.size(), AZ::RHI::Format::R32G32B32_FLOAT);
        auto normalBuffer = CreateBufferAsset(normals.data(), normals.size(), AZ::RHI::Format::R32G32B32_FLOAT);
        auto tangentBuffer = CreateBufferAsset(tangents.data(), tangents.size(), AZ::RHI::Format::R32G32B32A32_FLOAT);
        auto bitangentBuffer = CreateBufferAsset(bitangents.data(), bitangents.size(), AZ::RHI::Format::R32G32B32_FLOAT);
        auto uvBuffer_0 = CreateBufferAsset(uv_0.data(), uv_0.size(), AZ::RHI::Format::R32G32_FLOAT);
        auto uvBuffer_1 = CreateBufferAsset(uv_1.data(), uv_1.size(), AZ::RHI::Format::R32G32_FLOAT);
        AZ::Data::Asset<AZ::RPI::BufferAsset> colorBuffer;
        if (!colors.empty())
        {
            colorBuffer = CreateBufferAsset(colors.data(), colors.size(), AZ::RHI::Format::R32G32B32A32_FLOAT);
        }

        // create LOD asset
        AZ::RPI::ModelLodAssetCreator lodCreator;
        lodCreator.Begin(AZ::Uuid::CreateRandom());
        if (indicesBuffer)
        {
            lodCreator.AddLodStreamBuffer(indicesBuffer);
        }

        if (colorBuffer)
        {
            lodCreator.AddLodStreamBuffer(colorBuffer);
        }

        lodCreator.AddLodStreamBuffer(positionBuffer);
        lodCreator.AddLodStreamBuffer(normalBuffer);
        lodCreator.AddLodStreamBuffer(tangentBuffer);
        lodCreator.AddLodStreamBuffer(bitangentBuffer);
        lodCreator.AddLodStreamBuffer(uvBuffer_0);
        lodCreator.AddLodStreamBuffer(uvBuffer_1);

        // create mesh
        lodCreator.BeginMesh();
        if (indicesBuffer)
        {
            lodCreator.SetMeshIndexBuffer(AZ::RPI::BufferAssetView(indicesBuffer, indicesBuffer->GetBufferViewDescriptor()));
        }

        if (colorBuffer)
        {
            lodCreator.AddMeshStreamBuffer(
                AZ::RHI::ShaderSemantic("COLOR"), AZ::Name(),
                AZ::RPI::BufferAssetView(colorBuffer, colorBuffer->GetBufferViewDescriptor()));
        }

        lodCreator.AddMeshStreamBuffer(
            AZ::RHI::ShaderSemantic("POSITION"), AZ::Name(),
            AZ::RPI::BufferAssetView(positionBuffer, positionBuffer->GetBufferViewDescriptor()));
        lodCreator.AddMeshStreamBuffer(
            AZ::RHI::ShaderSemantic("NORMAL"), AZ::Name(), AZ::RPI::BufferAssetView(normalBuffer, normalBuffer->GetBufferViewDescriptor()));
        lodCreator.AddMeshStreamBuffer(
            AZ::RHI::ShaderSemantic("TANGENT"), AZ::Name(),
            AZ::RPI::BufferAssetView(tangentBuffer, tangentBuffer->GetBufferViewDescriptor()));
        lodCreator.AddMeshStreamBuffer(
            AZ::RHI::ShaderSemantic("BITANGENT"), AZ::Name(),
            AZ::RPI::BufferAssetView(bitangentBuffer, bitangentBuffer->GetBufferViewDescriptor()));
        lodCreator.AddMeshStreamBuffer(
            AZ::RHI::ShaderSemantic("UV", 0), AZ::Name(), AZ::RPI::BufferAssetView(uvBuffer_0, uvBuffer_0->GetBufferViewDescriptor()));
        lodCreator.AddMeshStreamBuffer(
            AZ::RHI::ShaderSemantic("UV", 1), AZ::Name(), AZ::RPI::BufferAssetView(uvBuffer_1, uvBuffer_1->GetBufferViewDescriptor()));

        lodCreator.SetMeshAabb(std::move(aabb));
        lodCreator.EndMesh();

        AZ::Data::Asset<AZ::RPI::ModelLodAsset> lodAsset;
        lodCreator.End(lodAsset);

        // create model asset
        AZ::RPI::ModelAssetCreator modelCreator;
        modelCreator.Begin(AZ::Uuid::CreateRandom());
        modelCreator.AddLodAsset(std::move(lodAsset));

        AZ::Data::Asset<AZ::RPI::ModelAsset> modelAsset;
        modelCreator.End(modelAsset);

        return modelAsset;
    }

    AZ::Data::Asset<AZ::RPI::BufferAsset> GltfModelComponent::CreateIndicesBufferAsset(
        const CesiumGltf::Model& model,
        const CesiumGltf::Accessor& indicesAccessor,
        const CesiumGltf::AccessorView<std::uint32_t>& indicesAccessorView)
    {
        assert(indicesAccessorView.status() == CesiumGltf::AccessorViewStatus::Valid);
        if (indicesAccessorView.size() == 0)
        {
            return AZ::Data::Asset<AZ::RPI::BufferAsset>();
        }

        const CesiumGltf::BufferView* bufferView = model.getSafe<CesiumGltf::BufferView>(&model.bufferViews, indicesAccessor.bufferView);
        assert(bufferView != nullptr); // should not happen because when creating accessor view, it checks for validity

        // data is packed together, we pass them directly to o3de
        const CesiumGltf::Buffer* buffer = model.getSafe<CesiumGltf::Buffer>(&model.buffers, bufferView->buffer);
        assert(buffer != nullptr); // should not happen because when creating accessor view, it checks for validity

        const void* data = buffer->cesium.data.data() + bufferView->byteOffset + indicesAccessor.byteOffset;
        return CreateBufferAsset(data, static_cast<std::size_t>(indicesAccessor.count), AZ::RHI::Format::R32_UINT);
    }

    AZ::Data::Asset<AZ::RPI::BufferAsset> GltfModelComponent::CreateBufferAsset(
        const void* data, const std::size_t elementCount, AZ::RHI::Format format)
    {
        AZ::RHI::BufferViewDescriptor bufferViewDescriptor = AZ::RHI::BufferViewDescriptor::CreateTyped(0, elementCount, format);
        AZ::RHI::BufferDescriptor bufferDescriptor;
        bufferDescriptor.m_bindFlags = AZ::RHI::BufferBindFlags::InputAssembly | AZ::RHI::BufferBindFlags::ShaderRead;
        bufferDescriptor.m_byteCount = bufferViewDescriptor.m_elementCount * bufferViewDescriptor.m_elementSize;

        AZ::RPI::BufferAssetCreator creator;
        creator.Begin(AZ::Uuid::CreateRandom());
        creator.SetBuffer(data, bufferDescriptor.m_byteCount, bufferDescriptor);
        creator.SetBufferViewDescriptor(bufferViewDescriptor);
        creator.SetUseCommonPool(AZ::RPI::CommonBufferPoolType::StaticInputAssembly);

        AZ::Data::Asset<AZ::RPI::BufferAsset> bufferAsset;
        creator.End(bufferAsset);

        return bufferAsset;
    }

    AZStd::vector<glm::vec2> GltfModelComponent::CreateUVAttribute(
        const CesiumGltf::Model& model,
        const CesiumGltf::MeshPrimitive& primitive,
        const CesiumGltf::AccessorView<std::uint32_t> indicesAccessorView,
        bool generateUnIndexedMesh,
        std::int32_t uvIndex)
    {
        std::string uvName = "UV_" + std::to_string(uvIndex);
        auto uvAttribute = primitive.attributes.find(uvName);
        if (uvAttribute == primitive.attributes.end())
        {
            return {};
        }

        GltfUVConverter uvConverter{ indicesAccessorView, {}, generateUnIndexedMesh };
        if (CesiumGltf::createAccessorView(model, uvAttribute->second, uvConverter))
        {
            return uvConverter.m_uvs;
        }

        return {};
    }

    AZStd::vector<glm::vec4> GltfModelComponent::CreateColorAttribute(
        const CesiumGltf::Model& model,
        const CesiumGltf::MeshPrimitive& primitive,
        const CesiumGltf::AccessorView<std::uint32_t> indicesAccessorView,
        bool generateUnIndexedMesh)
    {
        auto colorAttribute = primitive.attributes.find("COLOR_0");
        if (colorAttribute == primitive.attributes.end())
        {
            return {};
        }

        GltfColorConverter colorConverter{ indicesAccessorView, {}, generateUnIndexedMesh };
        if (CesiumGltf::createAccessorView(model, colorAttribute->second, colorConverter))
        {
            return colorConverter.m_colors;
        }

        return {};
    }

    void GltfModelComponent::CreateFlatNormal(const AZStd::vector<glm::vec3>& positions, AZStd::vector<glm::vec3>& normals)
    {
        normals.resize(positions.size());
        for (std::size_t i = 0; i < positions.size(); i += 3)
        {
            const glm::vec3& p0 = positions[i];
            const glm::vec3& p1 = positions[i + 1];
            const glm::vec3& p2 = positions[i + 2];
            glm::vec3 normal = glm::cross(p1 - p0, p2 - p0);
            if (CesiumUtility::Math::equalsEpsilon(glm::dot(normal, normal), 0.0, CesiumUtility::Math::EPSILON5))
            {
                normal = glm::vec3(0.0f, 1.0f, 0.0f);
            }
            else
            {
                normal = glm::normalize(normal);
            }

            normals[i] = normal;
            normals[i + 1] = normal;
            normals[i + 2] = normal;
        }
    }

    AZ::Aabb GltfModelComponent::CreateAabbFromPositions(const CesiumGltf::AccessorView<glm::vec3>& positionAccessorView)
    {
        AZ::Aabb aabb = AZ::Aabb::CreateNull();
        for (std::int64_t i = 0; i < positionAccessorView.size(); ++i)
        {
            const glm::vec3& position = positionAccessorView[i];
            aabb.AddPoint(AZ::Vector3(position.x, position.y, position.z));
        }

        return aabb;
    }

    bool GltfModelComponent::IsIdentityMatrix(const std::vector<double>& matrix)
    {
        static constexpr double identity[] = { 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0 };
        return std::equal(matrix.begin(), matrix.end(), identity);
    }
} // namespace Cesium
