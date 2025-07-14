#include "ShaderManager.h"

#include "Utility/Logger.h"

namespace sl
{

  /// <summary>
  /// 僔僃乕僟傪儘乕僪偟偰僉儍僢僔儏
  /// </summary>
  /// <param name="name"></param>
  /// <param name="device"></param>
  /// <param name="filename"></param>
  /// <param name="vsEntry"></param>
  /// <param name="psEntry"></param>
  /// <returns></returns>
  Shader* ShaderManager::Load(const std::string& name, ID3D11Device* device, ID3D11DeviceContext* context, const std::wstring& filename, const std::string& vsEntry, const std::string& psEntry)
  {
    auto it = mShaders.find(name);
    if (it != mShaders.end())
    {
      return it->second.get();
    }

    auto shader = std::make_unique<Shader>();
    if (!shader->Load(device, context, filename, vsEntry, psEntry))
    {
			LOG_WARN("僔僃乕僟僼傽僀儖側偄: {}", name);
      return nullptr;
    }

    Shader* ptr = shader.get();
    mShaders[name] = std::move(shader);
    return ptr;
  }

  /// <summary>
	/// 僼傽僀儖柤偐傜僔僃乕僟乕傪庢摼
  /// </summary>
  /// <param name="name"></param>
  /// <returns></returns>
  Shader* ShaderManager::Get(const std::string& name) const
  {
    auto it = mShaders.find(name);
    if (it != mShaders.end())
    {
      return it->second.get();
    }
    return nullptr;
  }

  void ShaderManager::ReleaseAll()
  {
    mShaders.clear();
  }
}