using UnityEngine;

public class BarRingVisualizer : MonoBehaviour
{
    [SerializeField]
    private Material material;

    void OnRenderObject()
    {
        //Debug.Log("OnRenderObject.");
        if (material != null) {
            // 比喩表現：この筆で
            material.SetPass(0);
            // 比喩表現：この絵を描く
            Graphics.DrawProceduralNow(MeshTopology.Triangles, 6 * 256);
            //Debug.Log("Draw call.");
        }
    }
}
