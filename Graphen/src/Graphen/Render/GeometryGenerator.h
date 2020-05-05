//***************************************************************************************
// GeometryGenerator.h by Frank Luna (C) 2011 All Rights Reserved.
//
// Defines a static class for procedurally generating the geometry of
// common mathematical objects.
//
// All triangles are generated "outward" facing.  If you want "inward"
// facing triangles (for example, if you want to place the camera inside
// a sphere to simulate a sky), you will need to:
//   1. Change the Direct3D cull mode or manually reverse the winding order.
//   2. Invert the normal.
//   3. Update the texture coordinates and tangent vectors.
//***************************************************************************************

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


   class GeometryGenerator {
   public:
      ///< summary>
      /// Creates a box centered at the origin with the given dimensions, where each
      /// face has m rows and n columns of vertices.
      ///</summary>
      static MeshData CreateBox(float width, float height, float depth,
         uint32 numSubdivisions);

      ///< summary>
      /// Creates a sphere centered at the origin with the given radius.  The
      /// slices and stacks parameters control the degree of tessellation.
      ///</summary>
      static MeshData CreateSphere(float radius, uint32 sliceCount, uint32 stackCount);

      ///< summary>
      /// Creates a geosphere centered at the origin with the given radius.  The
      /// depth controls the level of tessellation.
      ///</summary>
      static MeshData CreateGeosphere(float radius, uint32 numSubdivisions);

      ///< summary>
      /// Creates a cylinder parallel to the y-axis, and centered about the origin.
      /// The bottom and top radius can vary to form various cone shapes rather than
      /// true
      // cylinders.  The slices and stacks parameters control the degree of
      // tessellation.
      ///</summary>
      static MeshData CreateCylinder(float bottomRadius, float topRadius, float height,
         uint32 sliceCount, uint32 stackCount);

      ///< summary>
      /// Creates an mxn grid in the xz-plane with m rows and n columns, centered
      /// at the origin with the specified width and depth.
      ///</summary>
      static MeshData CreateGrid(float width, float depth, uint32 m, uint32 n);

      ///< summary>
      /// Creates a quad aligned with the screen.  This is useful for postprocessing
      /// and screen effects.
      ///</summary>
      static MeshData CreateQuad(float x, float y, float w, float h, float depth);

   private:
      static void Subdivide(MeshData& meshData);
      static Vertex MidPoint(const Vertex& v0, const Vertex& v1);
      static void BuildCylinderTopCap(float bottomRadius, float topRadius, float height,
         uint32 sliceCount, uint32 stackCount,
         MeshData& meshData);
      static void BuildCylinderBottomCap(float bottomRadius, float topRadius, float height,
         uint32 sliceCount, uint32 stackCount,
         MeshData& meshData);
   };

   class Mesh {
      std::string m_Name;

      MeshData m_meshData;

      StructuredBuffer m_VertexBuffer;
      ByteAddressBuffer m_IndexBuffer;

      void CreateGPUBuffers() {
         m_VertexBuffer.Create(L"Vertex", (uint32)m_meshData.Vertices.size(), sizeof(Vertex), m_meshData.Vertices.data());
         m_IndexBuffer.Create(L"Indexes", (uint32)m_meshData.Indices32.size(), sizeof(uint32), m_meshData.Indices32.data());
      }
   public:
      Mesh(const std::vector<Vertex>& vs, const std::vector<uint32>& inds, const char* name = "") {
         m_Name = name;
         m_meshData.Vertices = vs;
         m_meshData.Indices32 = inds;

         CreateGPUBuffers();
      }

      Mesh(std::vector<Vertex>&& vs, std::vector<uint32>&& inds, const char* name = "") {
         m_Name = name;
         m_meshData.Vertices = std::move(vs);
         m_meshData.Indices32 = std::move(inds);

         CreateGPUBuffers();
      }

      Mesh(const MeshData& md, const char* name = "") {
         m_Name = name;
         m_meshData = md;

         CreateGPUBuffers();
      }

      Mesh(MeshData&& md, const char* name = "") {
         m_Name = name;
         m_meshData = std::move(md);

         CreateGPUBuffers();
      }

      void SetGeometry(GraphicsContext& context) {
         context.SetIndexBuffer(m_IndexBuffer.IndexBufferView());
         context.SetVertexBuffer(0, m_VertexBuffer.VertexBufferView());
      }

      UINT IndexesCount() {
         return m_IndexBuffer.GetElementCount();
      }

      static sptr<Mesh> CreateFromVertex(const std::vector<Vertex> vs, const std::vector<uint32> inds, const char* name = "") {
         return std::make_shared<Mesh>(vs, inds, name);
      }

      static sptr<Mesh> CreateFromMeshData(const MeshData& meshData, const char* name = "") {
         return gn::CreateRef<Mesh>(meshData, name);
      }
   };
   using MeshRef = Ref<Mesh>;
}

