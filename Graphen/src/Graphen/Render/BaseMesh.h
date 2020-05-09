#pragma once
#include "CommandContext.h"


namespace gn {

   struct Vertex {
      Vertex() = default;

      Vertex(const DirectX::XMFLOAT3& p, const DirectX::XMFLOAT3& n,
         const DirectX::XMFLOAT3& t, const DirectX::XMFLOAT2& uv)
         : Position(p), Normal(n), TangentU(t), TexC(uv) {
      }

      Vertex(float px, float py, float pz, float nx, float ny, float nz, float tx,
         float ty, float tz, float u, float v)
         : Position(px, py, pz),
         Normal(nx, ny, nz),
         TangentU(tx, ty, tz),
         TexC(u, v) {
      }

      Vector3 Position;
      Vector3 Normal;
      Vector3 TangentU;
      Vector2 TexC;
   };


   struct MeshData {
      std::vector<Vertex> Vertices;
      std::vector<uint32> Indices32;

      std::vector<uint16>& GetIndices16() {
         if (mIndices16.empty()) {
            mIndices16.resize(Indices32.size());
            for (size_t i = 0; i < Indices32.size(); ++i)
               mIndices16[i] = static_cast<uint16>(Indices32[i]);
         }

         return mIndices16;
      }

   private:
      std::vector<uint16> mIndices16;
   };

   class BaseMesh {
   private:
      std::string m_name;

   public:

      virtual const void SetDrawBuffers(GraphicsContext& context) const = 0;
      virtual const uint GetDrawIndexCount() const = 0;

      std::string& GetName() { return m_name; }

      BaseMesh(std::string&& name = "") : m_name(std::move(name)) { }
      BaseMesh(const std::string& name = "") : m_name(name) { }
      BaseMesh(const char* name = "") : BaseMesh(std::move(std::string(name))) { }

      virtual ~BaseMesh() = default;
   };
   using BaseMeshRef = Ref<BaseMesh>;


   namespace MeshUtils {
      void BuildSeparateBuffersForVertex(const std::vector<Vertex>& vs,
         StructuredBuffer* posBuffer, StructuredBuffer* normalBuffer, StructuredBuffer* tangentBuffer, StructuredBuffer* texBuffer);
   }

   class Mesh : public BaseMesh {
      MeshData m_meshData;

      StructuredBuffer m_posBuffer;
      StructuredBuffer m_normalsBuffer;
      StructuredBuffer m_tangentBuffer;
      StructuredBuffer m_texBuffer;

      ByteAddressBuffer m_indexBuffer;

      void CreateGPUBuffers() {
         MeshUtils::BuildSeparateBuffersForVertex(m_meshData.Vertices, &m_posBuffer, &m_normalsBuffer, &m_tangentBuffer, &m_texBuffer);
         m_indexBuffer.Create(L"Indexes", (uint32)m_meshData.Indices32.size(), sizeof(uint32), m_meshData.Indices32.data());
      }
   public:
      Mesh(const std::vector<Vertex>& vs, const std::vector<uint32>& inds, const std::string& name = "") : BaseMesh(name) {
         m_meshData.Vertices = vs;
         m_meshData.Indices32 = inds;

         CreateGPUBuffers();
      }

      Mesh(std::vector<Vertex>&& vs, std::vector<uint32>&& inds, const std::string& name = "") : BaseMesh(name) {
         m_meshData.Vertices = std::move(vs);
         m_meshData.Indices32 = std::move(inds);

         CreateGPUBuffers();
      }

      Mesh(const MeshData& md, const std::string& name = "") : BaseMesh(name) {
         m_meshData = md;

         CreateGPUBuffers();
      }

      Mesh(MeshData&& md, const std::string& name = "") : BaseMesh(name) {
         m_meshData = std::move(md);

         CreateGPUBuffers();
      }

      const void SetDrawBuffers(GraphicsContext& context) const override;
      const uint GetDrawIndexCount() const override;

      static sptr<Mesh> CreateFromVertex(const std::vector<Vertex> vs, const std::vector<uint32> inds, const char* name = "") {
         return std::make_shared<Mesh>(vs, inds, name);
      }

      static sptr<Mesh> CreateFromMeshData(const MeshData& meshData, const char* name = "") {
         return gn::CreateRef<Mesh>(meshData, name);
      }
   };
   using MeshRef = Ref<Mesh>;

}

