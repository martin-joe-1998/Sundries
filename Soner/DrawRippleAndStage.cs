using Unity.Mathematics;
using UnityEngine;

public class DrawRippleAndStage : MonoBehaviour
{
    [SerializeField, Range(0.0f, 1.0f)] private float edgeWidth = 0.05f;
    [SerializeField] private Color edgeColor = Color.white;
    [SerializeField] private Vector3 SpawnWorldPos = new Vector3(0.0f, 0.0f, 0.0f);

    private RenderTexture rippleRT;
    private RenderTexture stageRT;

    private Material rippleMat;
    private Material stageMat;
    private Material drawMat;

    void Awake()
    {
        int width = 512;
        int height = 512;

        rippleRT = new RenderTexture(width, height, 0);
        stageRT = new RenderTexture(width, height, 0);

        // Put drawShader on this object!
        drawMat = GetComponent<Renderer>().material;

        rippleMat = new Material(Shader.Find("Ripple/RippleBWShader_uv"));
        stageMat = new Material(Shader.Find("Unlit/EdgeUnlitShader"));
    }

    void Update()
    {
        rippleMat.SetVector("_ObjectScale", transform.lossyScale);
        rippleMat.SetVector("_SpawnWorldPos", SpawnWorldPos);
        stageMat.SetFloat("_EdgeWidth", 1.0f - edgeWidth);
        stageMat.SetColor("_EdgeColor", edgeColor);

        Graphics.Blit(null, rippleRT, rippleMat);
        Graphics.Blit(null, stageRT, stageMat);

        //_propertyBlock.SetVector("_ObjectScale", transform.lossyScale);
        drawMat.SetTexture("_RippleTex", rippleRT);
        drawMat.SetTexture("_StageTex", stageRT);

        Graphics.Blit(null, null as RenderTexture, drawMat);
    }

    void OnDestroy()
    {
        if (rippleRT != null) rippleRT.Release();
        if (stageRT != null) stageRT.Release();
    }
}
