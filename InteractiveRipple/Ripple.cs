using UnityEngine;

public class Ripple : MonoBehaviour {
    [SerializeField]
    private Camera mainCamera;
    [SerializeField]
    private Shader DrawShader;       // draw shader
    [SerializeField]
    private Shader RippleShader;     // Ripple calc shader
    [SerializeField][Range(0, 1.0f)]
    private float DrawRadius = 0.15f;
    [SerializeField][Range(0, 1024)]
    private int TextureSize = 512;

    private RenderTexture PrevRT;    // Previous frame's RT
    private RenderTexture CurrentRT; // Current frame's RT
    private RenderTexture TempRT;    // Temporary RT
    private Material RippleMat;
    private Material DrawMat;
    private Vector3 lastPos;

    void Start()
    {
        // Get main camera object
        this.mainCamera = Camera.main.GetComponent<Camera>();

        this.CurrentRT = CreateRT();
        this.PrevRT = CreateRT();
        this.TempRT = CreateRT();

        this.DrawMat = new Material(this.DrawShader);
        this.RippleMat = new Material(this.RippleShader);

        this.GetComponent<Renderer>().material.mainTexture = this.CurrentRT;
    }

    // Create a new RenderTexture with the specified size and format
    private RenderTexture CreateRT()
    {
        RenderTexture rt = new RenderTexture(this.TextureSize, this.TextureSize, 0, RenderTextureFormat.RFloat);
        rt.Create();

        return rt;
    }

    private void DrawAt(float x, float y, float radius)
    {
        // original texture
        this.DrawMat.SetTexture("_SourceTex", CurrentRT);
        this.DrawMat.SetVector("_Pos", new Vector4(x, y, radius));
        // pass to tempRT
        Graphics.Blit(null, this.TempRT, this.DrawMat);

        RenderTexture rt = this.TempRT;
        this.TempRT = this.CurrentRT;
        this.CurrentRT = rt;
    }

    void Update()
    {
        // raycast detect
        if (Input.GetMouseButton(0)) {
            Ray ray = this.mainCamera.ScreenPointToRay(Input.mousePosition);
            // raycast hit info
            RaycastHit hit;
            if (Physics.Raycast(ray, out hit)) {
                if ((hit.point - this.lastPos).sqrMagnitude > 0.1f) {
                    // reset lastPos
                    this.lastPos = hit.point;
                    // draw
                    DrawAt(hit.textureCoord.x, hit.textureCoord.y, this.DrawRadius);
                }
            }
        }

        // calc ripple
        this.RippleMat.SetTexture("_PrevRT", this.PrevRT);
        this.RippleMat.SetTexture("_CurrentRT", this.CurrentRT);
        Graphics.Blit(null, this.TempRT, this.RippleMat);

        // pass to curr, ping pong buffer
        Graphics.Blit(this.TempRT, this.PrevRT);
        RenderTexture rt = this.PrevRT;
        this.PrevRT = this.CurrentRT;
        this.CurrentRT = rt;
    }
}
