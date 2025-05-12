using UnityEngine;

public class CombineShaderTest : MonoBehaviour
{
    private RenderTexture tempRT_1;
    private RenderTexture tempRT_2;

    private Material material_1;
    private Material material_2;
    private Material drawMaterial;

    void Awake()
    {
        int width = 512;
        int height = 512;

        tempRT_1 = new RenderTexture(width, height, 0);
        tempRT_2 = new RenderTexture(width, height, 0);

        material_1 = new Material(Shader.Find("Unlit/CombineShader1"));
        material_2 = new Material(Shader.Find("Unlit/CombineShader2"));
        drawMaterial = GetComponent<Renderer>().material;
    }

    void Update()
    {
        Graphics.Blit(null, tempRT_1, material_1);
        Graphics.Blit(null, tempRT_2, material_2);

        drawMaterial.SetTexture("_CombineTex_1", tempRT_1);
        drawMaterial.SetTexture("_CombineTex_2", tempRT_2);

        Graphics.Blit(null, null as RenderTexture, drawMaterial);
    }

    void OnDestroy()
    {
        if (tempRT_1 != null) tempRT_1.Release();
        if (tempRT_2 != null) tempRT_2.Release();
    }
}
