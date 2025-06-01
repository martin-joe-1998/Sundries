using UnityEngine;
using UnityEngine.Video;

public class FullScreenRipple : MonoBehaviour
{
    [SerializeField] private Material rippleMat;
    //[Header("Video resource")]
    //[SerializeField] private VideoClip videoClip;

    //private VideoPlayer videoPlayer;
    //private RenderTexture videoRenderTexture;

    private float lastSpawnTime = -1.0f;
    private float totalLifeTime = 4.0f;

    void Start()
    {
        this.rippleMat.SetFloat("_SpawnTime", -1.0f);

        // videoPlayer = gameObject.AddComponent<VideoPlayer>();
        // videoPlayer.playOnAwake = false;
        // videoPlayer.isLooping = false;
        // videoPlayer.renderMode = VideoRenderMode.RenderTexture;
        // videoPlayer.source = VideoSource.VideoClip;
        // videoPlayer.clip = videoClip;
        // videoPlayer.audioOutputMode = VideoAudioOutputMode.None;
        // 
        // videoRenderTexture = new RenderTexture(1920, 1080, 0, RenderTextureFormat.ARGB32);
        // videoRenderTexture.Create();
        // videoPlayer.targetTexture = videoRenderTexture;

        // if (rippleMat != null) {
        //     rippleMat.SetTexture("_RippleTexture", videoRenderTexture);
        // }
    }

    void Update()
    {
        if (Input.GetKeyDown(KeyCode.K)) {
            // Time.time = _Time.x
            lastSpawnTime = Time.time;
            this.rippleMat.SetFloat("_SpawnTime", lastSpawnTime);

            // videoPlayer.frame = 0;
            // videoPlayer.Play();
            // Debug.Log("Ripple + Video Start at: " + lastSpawnTime);
        }
        
        if (lastSpawnTime >= 0 && Time.time - lastSpawnTime > totalLifeTime) 
        {
            rippleMat.SetFloat("_SpawnTime", -1.0f);
            lastSpawnTime = -1.0f;

            // videoPlayer.Stop();
            // Debug.Log("Ripple + Video Stopped");
        }
    }

    void OnDestroy()
    {
        // if (videoRenderTexture != null) {
        //     videoRenderTexture.Release();
        // }
    }
}
