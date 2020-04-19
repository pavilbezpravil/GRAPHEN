#include "pch.h"
#include "Shader.h"
#include <experimental/filesystem>

using namespace std::experimental;

namespace fs = std::experimental::filesystem;

ComPtr<ID3DBlob> CompileShader(const wchar_t* filename, const char* entrypoint,
                               const D3D_SHADER_MACRO* defines, const char* target)
{
   UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
   compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

   HRESULT hr = S_OK;

   ComPtr<ID3DBlob> byteCode = nullptr;
   ComPtr<ID3DBlob> errors;
   hr = D3DCompileFromFile(filename, defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
                           entrypoint, target, compileFlags, 0, &byteCode, &errors);

   if (errors != nullptr)
   {
      // DEBUGPRINT((char*)errors->GetBufferPointer());
      OutputDebugStringA((char*)errors->GetBufferPointer());
   }

   return byteCode;
}

sptr<Shader> Shader::Create(const wchar_t* filename, const char* entrypoint, const D3D_SHADER_MACRO* defines)
{
   ShaderType sType;

   const auto extension = fs::path(filename).extension();
   if (extension == ".vs") {
      sType = ShaderType::Vertex;
   } else if (extension == ".ps") {
      sType = ShaderType::Pixel;
   } else if (extension == ".cs") {
      sType = ShaderType::Compute;
   } else {
      return {};
   }

   return Shader::Create(filename, entrypoint, sType, defines);
}

sptr<Shader> Shader::Create(const wchar_t* filename, const char* entrypoint, ShaderType type,
   const D3D_SHADER_MACRO* defines)
{
   if (!fs::exists(filename))
   {
      auto curWorkDir = fs::current_path();
      DEBUGPRINT(L"file %s is not exist", filename);
      DEBUGPRINT(L"curernt work dir: %s", fs::current_path().c_str());
      return {};
   }

   std::string target;
   switch (type)
   {
   case ShaderType::Vertex: target = "vs_5_0"; break;
   case ShaderType::Pixel: target = "ps_5_0"; break;
   case ShaderType::Compute: target = "cs_5_0"; break;
   default: ASSERT(false) ;
   }

   auto blob = CompileShader(filename, entrypoint, defines, target.c_str());
   if (!blob)
   {
      // todo: log it
      return {};
   }

   return std::make_shared<Shader>(std::move(blob), type);
}

Shader::Shader(ComPtr<ID3DBlob>&& blob, ShaderType type) : m_blob(std::move(blob)), m_type(type)
{
}
