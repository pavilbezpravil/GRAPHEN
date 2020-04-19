#pragma once

#include "pch.h"

ComPtr<ID3DBlob> CompileShader(
   const wchar_t* filename,
   const char* entrypoint,
   const D3D_SHADER_MACRO* defines,
   const char* target);

enum class ShaderType
{
   Vertex,
   Pixel,
   Compute,
};

class Shader
{
public:
   void* GetBytecodeBuffer() { m_blob->GetBufferPointer(); }
   size_t GetBytecodeBufferSize() { m_blob->GetBufferSize(); }

   D3D12_SHADER_BYTECODE GetBytecode() { return CD3DX12_SHADER_BYTECODE(m_blob.Get()); }

   static sptr<Shader> Create(
      const wchar_t* filename,
      const char* entrypoint,
      const D3D_SHADER_MACRO* defines = nullptr);

   static sptr<Shader> Create(
      const wchar_t* filename,
      const char* entrypoint,
      ShaderType type,
      const D3D_SHADER_MACRO* defines = nullptr);

   Shader(ComPtr<ID3DBlob>&& blob, ShaderType type);
private:

   ComPtr<ID3DBlob> m_blob;
   ShaderType m_type;
};

