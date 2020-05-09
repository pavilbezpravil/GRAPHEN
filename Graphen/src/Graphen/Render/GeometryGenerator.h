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
#include "BaseMesh.h"


namespace gn {
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
}

