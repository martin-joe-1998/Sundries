using UnityEngine;

public class RippleController : MonoBehaviour
{
    public Camera mainCamera;
    private Material rippleMaterial;

    private const int MAX_RIPPLES = 10;
    private Vector4[] spawnData = new Vector4[MAX_RIPPLES];
    private int rippleIndex = 0;

    private void Awake()
    {
        rippleMaterial = GetComponent<Renderer>().material;
    }

    private void Start()
    {
        for (int i = 0; i < MAX_RIPPLES; i++) {
            // w < 0 means uninitialized
            spawnData[i] = new Vector4(9999, 9999, 9999, -9999);
        }

        rippleMaterial.SetVectorArray("_SpawnWorldPos", spawnData);
    }

    void Update()
    {
        if (Input.GetMouseButtonDown(0)) {
            Ray ray = mainCamera.ScreenPointToRay(Input.mousePosition);
            if (Physics.Raycast(ray, out RaycastHit hit)) {
                Vector3 spawnPos = hit.point;
                float spawnTime = Time.time * rippleMaterial.GetFloat("_TimeScale");

                spawnData[rippleIndex] = new Vector4(spawnPos.x, spawnPos.y, spawnPos.z, spawnTime);
                rippleIndex = (rippleIndex + 1) % MAX_RIPPLES;

                rippleMaterial.SetVectorArray("_SpawnWorldPos", spawnData);
                Debug.Log("Clicked: " + hit.point);
            }
        }
    }
}
