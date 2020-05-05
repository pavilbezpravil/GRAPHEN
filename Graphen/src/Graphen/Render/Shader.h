#pragma once

#include "gnpch.h"

ComPtr<ID3DBlob> CompileShader(
   const char* filename,
   const char* entrypoint,
   const D3D_SHADER_MACRO* defines,
   const char* target);

enum class ShaderType
{
   Vertex,
   Pixel,
   Compute,
};

struct ShaderInfo
{
   std::string filename;
   std::string entrypoint;
   ShaderType type;
   std::vector<D3D_SHADER_MACRO> defines;


   ShaderInfo(const char* filename, const char* entrypoint, ShaderType type,
      const D3D_SHADER_MACRO* defines);
};

class ShaderManager;

class Shader
{
   friend class ShaderManager;

public:
   void* GetBytecodeBuffer() { m_blob->GetBufferPointer(); }
   size_t GetBytecodeBufferSize() { m_blob->GetBufferSize(); }

   D3D12_SHADER_BYTECODE GetBytecode() { return CD3DX12_SHADER_BYTECODE(m_blob.Get()); }

   static sptr<Shader> Create(
      const char* filename,
      const char* entrypoint,
      const D3D_SHADER_MACRO* defines = nullptr);

   static sptr<Shader> Create(
      const char* filename,
      const char* entrypoint,
      ShaderType type,
      const D3D_SHADER_MACRO* defines = nullptr);

   static sptr<Shader> Create(const ShaderInfo& sInfo);

   Shader() = default;
   Shader(ComPtr<ID3DBlob>&& blob, ShaderType type);
private:

   ComPtr<ID3DBlob> m_blob;
   ShaderType m_type;
};


// class ShaderManager
// {
// public:
//    static sptr<Shader> GetShader(
//       const char* filename,
//       const char* entrypoint,
//       ShaderType type,
//       const D3D_SHADER_MACRO* defines = nullptr
//    );
//
// private:
//    static ShaderManager s_Instance;
//
//    std::unordered_map<ShaderInfo, sptr<Shader>> m_ShaderMap;
//
//    sptr<Shader> GetShaderImpl(
//       const char* filename,
//       const char* entrypoint,
//       ShaderType type,
//       const D3D_SHADER_MACRO* defines = nullptr
//    );
//
//    void AddShader(ShaderInfo& sInfo);
// };
