using System.Collections.Generic;
using Unity.Mathematics;
using UnityEngine;

public class DitherStaticStage : MonoBehaviour
{
    [SerializeField] private Camera mainCamera;
    [SerializeField] private GameObject targetCharacter;
    private string layerName = "Ground";

    private int layerMask;
    private RaycastHit[] raycastHits = new RaycastHit[32];
    private HashSet<GameObject> previousHitObjects = new HashSet<GameObject>();
    private Vector2 ditherRange = new Vector2(4.0f, 0.5f);

    void Start()
    {
        // Get dither target's layerMask (In this case, Ground Layer)
        this.layerMask = 1 << LayerMask.NameToLayer(this.layerName);
    }

    void Update()
    {
        if (this.mainCamera == null || this.targetCharacter == null)
        {
            return;
        }

        Vector3 cameraPosition = this.mainCamera.transform.position;
        Vector3 targetPosition = this.targetCharacter.transform.position;
        Vector3 direction = targetPosition - cameraPosition;
        float distance = direction.magnitude;

        // RayCast to get all obj which situated between Camera and Character
        int hitCount = Physics.RaycastNonAlloc(cameraPosition, direction.normalized, this.raycastHits, distance, this.layerMask);

        HashSet<GameObject> currentHitObjects = new HashSet<GameObject>();

        for (int i = 0; i < hitCount; i++)
        {
            RaycastHit hit = this.raycastHits[i];
            GameObject hitObject = hit.collider.gameObject;
            currentHitObjects.Add(hitObject);

            // Get hitObject's renderer component to set dither parameter
            if (hitObject.TryGetComponent<Renderer>(out var renderer))
            {
                MaterialPropertyBlock propertyBlock = new MaterialPropertyBlock();
                renderer.GetPropertyBlock(propertyBlock);

                // Dither value determined by distance between camera and hitObject
                this.Remap(Vector3.Distance(hitObject.transform.position, cameraPosition), this.ditherRange, new float2(0.0f, 1.0f), out float dither);

                propertyBlock.SetFloat("_Dither", dither);
                renderer.SetPropertyBlock(propertyBlock);
            }
        }

        // Reset previousHitObjects's dither params to Default value
        foreach (GameObject obj in this.previousHitObjects)
        {
            if (!currentHitObjects.Contains(obj))
            {
                if (obj.TryGetComponent<Renderer>(out var renderer))
                {
                    MaterialPropertyBlock propertyBlock = new MaterialPropertyBlock();
                    renderer.GetPropertyBlock(propertyBlock);

                    propertyBlock.SetFloat("_Dither", 0f);
                    renderer.SetPropertyBlock(propertyBlock);
                }
            }
        }

        // Update previousHitObjects
        this.previousHitObjects = currentHitObjects;
    }

    void Remap(float input, float2 inMinMax, float2 outMinMax, out float output)
    {
        output = outMinMax.x + ((input - inMinMax.x) * (outMinMax.y - outMinMax.x) / (inMinMax.y - inMinMax.x));
    }
}
