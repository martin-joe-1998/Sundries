#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <d3d11.h>

#include "Shader.h"

namespace sl
{
  /// <summary>
  /// 僔僃乕僟乕偺堦尦娗棟丒僉儍僢僔儏
  /// </summary>
  class ShaderManager
  {
  public:
    static ShaderManager& GetInstance()
    {
      static ShaderManager instance;
      return instance;
    }

    // 僔僃乕僟乕偺儘乕僪乮僉儍僢僔儏晅偒乯
    Shader* Load(
      const std::string& name,
      ID3D11Device* device,
      ID3D11DeviceContext* context,
      const std::wstring& filename,
      const std::string& vsEntry,
      const std::string& psEntry);
    
    Shader* Get(const std::string& name) const;
    
    void ReleaseAll();

  private:
    std::unordered_map<std::string, std::unique_ptr<Shader>> mShaders;
  };
}
