using UnityEngine;
using UnityEngine.Rendering;
using UnityEngine.Rendering.RenderGraphModule;
using UnityEngine.Rendering.Universal;

[System.Serializable]
public class FullScreenRippleSettings {
    public bool isActive = true;
    public Material fullScreenMat;
}

public class FullScreenPassRipple : ScriptableRendererFeature
{

    public FullScreenRippleSettings settings = new FullScreenRippleSettings();

    class RipplePass : ScriptableRenderPass {
        private Material mat;

        public RipplePass(Material mat)
        {
            this.mat = mat;
            this.renderPassEvent = RenderPassEvent.AfterRenderingPostProcessing;
        }

        [System.Obsolete]
        public void Run(ScriptableRenderContext context, ref RenderingData renderingData)
        {
            if (mat == null) return;

            CommandBuffer cmd = CommandBufferPool.Get("RippleFullScreenPass");
            var source = renderingData.cameraData.renderer.cameraColorTargetHandle;
            cmd.Blit(source, source, mat);
            context.ExecuteCommandBuffer(cmd);
            CommandBufferPool.Release(cmd);
        }
    }

    private RipplePass ripplePass;

    public override void Create()
    {
        ripplePass = new RipplePass(settings.fullScreenMat);
    }

    public override void AddRenderPasses(ScriptableRenderer renderer, ref RenderingData renderingData)
    {
        // Control whether to enable this pass
        if (RippleEffectToggle.Enabled)
            renderer.EnqueuePass(ripplePass);
    }
}
