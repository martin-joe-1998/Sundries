using System.Collections;
using UnityEngine;
using UnityEngine.VFX;

namespace Queen.Gameplay
{
    public class PortalVFXRespawn : MonoBehaviour {
        [SerializeField]
        private GameObject portalVFXPrefab;

        [SerializeField]
        private float portalDuration = 3.0f;

        [SerializeField]
        private float portalTwirlTime = 1.5f;

        [SerializeField]
        private Vector3 portalScaleOffset = new Vector3(3f, 3f, 3f);

        private GameObject portalVFXInstance;
        private VisualEffect portalVFXCache;
        private ItemRespawn itemRespawn;

        private float vfxTimer = 0f;
        private bool isVFXActive = false;

        void Awake()
        {
            if (!this.TryGetComponent<ItemRespawn>(out this.itemRespawn)) {
                Debug.Log("ItemRespawn component not found on PortalVFXRespawn GameObject.");
            }
        }

        void Start()
        {
            this.itemRespawn.OnPortalRespawn += this.TriggerPortalVFX;
        }

        void Update()
        {
            if (!this.isVFXActive || this.portalVFXCache == null) return;

            // 偙偙偐傜愭偑幚峴偝傟傞偺偼丄VFX 偑傾僋僥傿僽側応崌偺傒
            this.vfxTimer -= Time.deltaTime;

            float fadeHalf = this.portalTwirlTime / 2f;
            float fadeInStartTime = this.portalDuration + this.portalTwirlTime - fadeHalf;

            // FadeIn 0 -> 1
            if (this.vfxTimer >= fadeInStartTime) {
                float t = Mathf.InverseLerp(this.portalDuration + this.portalTwirlTime, fadeInStartTime, this.vfxTimer);
                this.portalVFXCache.SetFloat("PortalFade", t);
            }
            else if (this.vfxTimer <= this.portalTwirlTime)
            {// FadeOut
                float t = Mathf.Clamp01(this.vfxTimer / this.portalTwirlTime);
                this.portalVFXCache.SetFloat("PortalFade", t);
            }
            else
            {
                this.portalVFXCache.SetFloat("PortalFade", 1.0f);
            }

            // Particle 偺 fadeout 傪惂屼偡傞晹暘
            if (this.vfxTimer <= this.portalDuration) {
                this.portalVFXCache.Stop();
            }

            if (this.vfxTimer <= 0f) {
                // 姰慡偵廔椆丄僄僼僃僋僩傪掆巭偟丄僀儞僗僞儞僗傪攋婞
                // this.portalVFXCache.Stop();
                this.isVFXActive = false;
                Destroy(this.portalVFXInstance);
            }
        }

        void OnDestroy()
        {
            // 億乕僞儖 VFX 僀儞僗僞儞僗偑懚嵼偡傞応崌偼攋婞
            if (this.portalVFXInstance != null) {
                Destroy(this.portalVFXInstance);
                this.portalVFXInstance = null;
            }

            // 僀儀儞僩儕僗僫乕傪夝彍
            if (this.itemRespawn != null) {
                this.itemRespawn.OnPortalRespawn -= this.TriggerPortalVFX;
            }
        }

        private void TriggerPortalVFX()
        {
            // 億乕僞儖 VFX 偑婛偵懚嵼偱偁傟偽壗傕偟側偄丄側偄側傜惗惉偟偰弶婜壔
            if (this.portalVFXInstance == null) {
                Debug.Log("Portal position: " + this.transform.position);
                this.portalVFXInstance = Instantiate(this.portalVFXPrefab, this.transform.position, this.portalVFXPrefab.transform.rotation);
                this.portalVFXInstance.transform.localScale = this.portalScaleOffset;
                this.portalVFXCache = this.portalVFXInstance.TryGetComponent<VisualEffect>(out var vfx) ? vfx : null;
            } else {
                Debug.Log("Can't create portal instance");
            }

            // 億乕僞儖 VFX 偑懚嵼偟丄偐偮桳岠偱偁傟偽嵞僗僞乕僩
            if (this.portalVFXCache)
            {
                // VFX 傪儕僙僢僩偟偰嵞僗僞乕僩
                this.portalVFXCache.enabled = true;
                this.portalVFXCache.Stop();
                this.portalVFXCache.SetFloat("PortalFade", 0.0f);
                this.portalVFXCache.Play();

                // 僞僀儅乕弶婜壔
                this.vfxTimer = this.portalDuration + this.portalTwirlTime;
                this.isVFXActive = true;
            }
            else
            {
                return;
            }
        }
    }
}
