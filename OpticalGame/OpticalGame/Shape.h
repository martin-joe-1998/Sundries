#pragma once  
#include "Renderer.h"  
#include "Mesh.h"  
#include "Transform.h"  
#include <memory>  
#include <unordered_map>  
#include <vector>

class Shape  
{  
public:  
    struct KeyState {  
        int key;  
        bool isPressed = false;  
        float timeSinceLastMove = 0.0f;  
    };

    virtual ~Shape();  
    virtual void Draw(Renderer& renderer) = 0;  
    virtual void Update(float deltaTime) = 0;  
    virtual void ProcessInput(float deltaTime, std::vector<KeyState>& keyStates) {}  


protected:  
    Shape() {}  
    virtual void CreateMesh(Renderer& renderer) = 0;  
    virtual void CreateShaders(Renderer& renderer) = 0;  

    std::unique_ptr<Transform> m_transform = nullptr;  

    ID3D11Buffer* m_vertexBuffer = nullptr;  
    ID3D11Buffer* m_indexBuffer = nullptr;  
    ID3D11Buffer* m_cBufferPermanent = nullptr;  
    ID3D11Buffer* m_cBufferEveryFrame = nullptr;  
    ID3D11VertexShader* m_vertexShader = nullptr;  
    ID3D11PixelShader* m_pixelShader = nullptr;  
    ID3D11InputLayout* m_inputLayout = nullptr;  
    ID3D11ShaderResourceView* m_shaderResourceView = nullptr;  
    ID3D11SamplerState* m_samplerLinear = nullptr;  
};
