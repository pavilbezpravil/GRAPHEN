#include "gnpch.h"
#include "Shader.h"
#include <experimental/filesystem>

using namespace std::experimental;

namespace fs = std::experimental::filesystem;

ComPtr<ID3DBlob> CompileShader(const char* filename, const char* entrypoint,
                               const D3D_SHADER_MACRO* defines, const char* target)
{
   UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
   compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

   ComPtr<ID3DBlob> byteCode = nullptr;
   ComPtr<ID3DBlob> errors;
   HRESULT hr = D3DCompileFromFile(MakeWStr(filename).c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
                           entrypoint, target, compileFlags, 0, &byteCode, &errors);

   if (errors != nullptr) {
      GN_CORE_WARN("[SHADER] Cant compile '{0}' by error '{1}'", filename, (char*)errors->GetBufferPointer());
   }

   return byteCode;
}

ShaderInfo::ShaderInfo(const char* filename, const char* entrypoint, ShaderType type, const D3D_SHADER_MACRO* defines)
      : filename(filename), entrypoint(entrypoint), type(type)
{
   while (defines) {
      this->defines.emplace_back(*defines);
      ++defines;
      if (!defines) {
         this->defines.push_back({});
         break;
      }
   }
}

sptr<Shader> Shader::Create(const char* filename, const char* entrypoint, const D3D_SHADER_MACRO* defines)
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

sptr<Shader> Shader::Create(const char* filename, const char* entrypoint, ShaderType type,
   const D3D_SHADER_MACRO* defines)
{
   if (!fs::exists(filename))
   {
      GN_CORE_WARN("[SHADER] File '{}' is not exist", filename);
      return {};
   }

   std::string target;
   switch (type)
   {
   case ShaderType::Vertex: target = "vs_5_1"; break;
   case ShaderType::Pixel: target = "ps_5_1"; break;
   case ShaderType::Compute: target = "cs_5_1"; break;
   default: ASSERT(false) ;
   }

   auto blob = CompileShader(filename, entrypoint, defines, target.c_str());
   if (!blob)
   {
      GN_CORE_WARN("[SHADER] Cant compile '{0}'", filename);
      return {};
   }

   GN_CORE_INFO("[SHADER] Success compile shader: '{0}'", filename);

   return std::make_shared<Shader>(std::move(blob), type);
}

sptr<Shader> Shader::Create(const ShaderInfo& sInfo)
{
   return Create(sInfo.filename.c_str(), sInfo.entrypoint.c_str(), sInfo.type, sInfo.defines.data());
}

Shader::Shader(ComPtr<ID3DBlob>&& blob, ShaderType type) : m_blob(std::move(blob)), m_type(type)
{
}

// sptr<Shader> ShaderManager::GetShader(const char* filename, const char* entrypoint, ShaderType type,
//    const D3D_SHADER_MACRO* defines)
// {
//    return s_Instance.GetShaderImpl(filename, entrypoint, type, defines);
// }
//
// sptr<Shader> ShaderManager::GetShaderImpl(const char* filename, const char* entrypoint, ShaderType type,
//    const D3D_SHADER_MACRO* defines)
// {
//    ShaderInfo sInfo{ filename, entrypoint, type, defines };
//    auto cShader = Shader::Create(sInfo);
//    if (!cShader)
//    {
//       // GN_CORE_WARN("[SHADER MANAGER] Cant compile shader {}", filename);
//       return {};
//    }
//
//    m_ShaderMap[sInfo] = cShader;
// }
