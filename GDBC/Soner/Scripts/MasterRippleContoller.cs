using UnityEngine;

public class MasterRippleContoller : MonoBehaviour
{
    private const int MAXRIPPLES = 10;
    [SerializeField] private Camera mainCamera;
    private Material rippleMaterial;

    private Vector4[] flatRippleSpawnData = new Vector4[MAXRIPPLES]; // .xyz is FlatRippleSpawnPos, .w is spawnTime
    private Color[] flatRippleColors = new Color[MAXRIPPLES]; // RGBA
    private Vector4[] vertexRippleSpawnData = new Vector4[MAXRIPPLES];
    private Color[] vertexRippleColors = new Color[MAXRIPPLES];

    private int flatRippleIndex = 0;
    private int vertexRippleIndex = 0;

    private void Awake()
    {
        this.rippleMaterial = this.GetComponent<Renderer>().material;
    }

    private void Start()
    {
        for (int i = 0; i < MAXRIPPLES; i++) {
            // Setup flat ripple data
            // w < 0 means uninitialized
            this.flatRippleSpawnData[i] = new Vector4(9999, 9999, 9999, -9999);
            this.flatRippleColors[i] = new Vector4(1.0f, 1.0f, 1.0f, 0.0f);
            // Setup vertex ripple data
            this.vertexRippleSpawnData[i] = new Vector4(9999, 9999, 9999, -9999);
            this.vertexRippleColors[i] = new Vector4(1.0f, 1.0f, 1.0f, 0.0f);
        }

        this.rippleMaterial.SetVectorArray("_FlatRippleSpawnWorldPos", this.flatRippleSpawnData);
        this.rippleMaterial.SetColorArray("_FlatRippleColor", this.flatRippleColors);
        this.rippleMaterial.SetVectorArray("_VertexRippleSpawnWorldPos", this.vertexRippleSpawnData);
        this.rippleMaterial.SetColorArray("_VertexRippleColor", this.vertexRippleColors);
    }

    void Update()
    {
        // Process flat ripple
        if (Input.GetMouseButtonDown(0)) {
            Ray ray = this.mainCamera.ScreenPointToRay(Input.mousePosition);
            if (Physics.Raycast(ray, out RaycastHit hit)) {
                // Calc spawn position
                Vector3 spawnPos = hit.point;

                float spawnTime = Time.time * this.rippleMaterial.GetFloat("_TimeScale");

                // Set flat ripple color
                Vector4 rippleColor = Color.red;

                this.flatRippleSpawnData[this.flatRippleIndex] = new Vector4(spawnPos.x, spawnPos.y, spawnPos.z, spawnTime);
                this.flatRippleColors[this.flatRippleIndex] = rippleColor;

                this.flatRippleIndex = (this.flatRippleIndex + 1) % MAXRIPPLES;

                this.rippleMaterial.SetColorArray("_FlatRippleColor", this.flatRippleColors);
                this.rippleMaterial.SetVectorArray("_FlatRippleSpawnWorldPos", this.flatRippleSpawnData);
                Debug.Log("Flat Ripple Spawn Pos: " + hit.point);
            }
        }

        // Process vertex ripple
        if (Input.GetMouseButtonDown(1)) {
            Ray ray = this.mainCamera.ScreenPointToRay(Input.mousePosition);
            if (Physics.Raycast(ray, out RaycastHit hit)) {
                // Calc spawn position
                Vector3 spawnPos = hit.point;

                float spawnTime = Time.time * this.rippleMaterial.GetFloat("_TimeScale");

                // Set vertex ripple color
                Vector4 rippleColor = Color.blue;

                this.vertexRippleSpawnData[this.vertexRippleIndex] = new Vector4(spawnPos.x, spawnPos.y, spawnPos.z, spawnTime);
                this.vertexRippleColors[this.vertexRippleIndex] = rippleColor;

                this.vertexRippleIndex = (this.vertexRippleIndex + 1) % MAXRIPPLES;

                this.rippleMaterial.SetColorArray("_VertexRippleColor", this.vertexRippleColors);
                this.rippleMaterial.SetVectorArray("_VertexRippleSpawnWorldPos", this.vertexRippleSpawnData);
                Debug.Log("Vertex Ripple Spawn Pos: " + hit.point);
            }
        }
    }
}
