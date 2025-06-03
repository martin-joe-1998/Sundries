using System;
using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEngine;
using UnityEngine.Rendering;
using UnityEngine.Rendering.RenderGraphModule;
using UnityEngine.Rendering.Universal;

// ?SRF包含 3 个主要 pass：
// BlitStartRenderPass：从?像机的?色?冲拷?数据出来（准? Blit ?入?理）
// BlitRenderPass：按?序?多个材??行全屏 Blit 操作
// BlitEndRenderPass：将最??理后的?像再 Blit 回?像机
public class BlitRendererFeature : ScriptableRendererFeature
{
    // 一个存?在 ContextContainer 中的自定?上下文?，??管理?理（Unity 的 per-frame 数据容器）
    public class BlitData : ContextItem, IDisposable
    {
        // 在多次 Blit 操作中?流作??入?出，避免中??果被覆盖。（Ping-Pong 双?冲）
        // RTHandle 是 Unity 的 RenderTexture 封装?（跨分辨率?配）
        RTHandle m_TextureFront;
        RTHandle m_TextureBack;
        // TextureHandle 是 RenderGraph 用于管理?像依?的句柄（?????）
        TextureHandle m_TextureHandleFront;
        TextureHandle m_TextureHandleBack;

        // scaleBias 是屏幕?配用的
        static Vector4 scaleBias = new Vector4(1f, 1f, 0f, 0f);

        // 控制当前激活的是?一??理（ping-pong）
        bool m_IsFront = true;

        // 表示当前“有效?果”的?像句柄
        public TextureHandle texture;

        // 使用?像机当前的 RenderTextureDescriptor 初始化 RTHandle
        // 然后通? renderGraph.ImportTexture ?成 TextureHandle，以供 RenderGraph 使用
        // ??在?用?一?的 class 之前被?用
        public void Init(RenderGraph renderGraph, RenderTextureDescriptor targetDescriptor, string textureName = null)
        {
            var texName = String.IsNullOrEmpty(textureName) ? "_BlitTexture" : textureName;
            // 会自?重分配尺寸?化的 RTHandle，ReAllocateHandleIfNeeded会自?把 texName 注册?全局?理
            RenderingUtils.ReAllocateHandleIfNeeded(ref m_TextureFront, targetDescriptor, FilterMode.Bilinear, TextureWrapMode.Clamp, name: texName + "Front");
            RenderingUtils.ReAllocateHandleIfNeeded(ref m_TextureBack, targetDescriptor, FilterMode.Bilinear, TextureWrapMode.Clamp, name: texName + "Back");

            m_TextureHandleFront = renderGraph.ImportTexture(m_TextureFront);
            m_TextureHandleBack = renderGraph.ImportTexture(m_TextureBack);
            // ?置初始有效?像? m_TextureHandleFront
            texture = m_TextureHandleFront;
        }

        // RenderGraph 只保存当前??源，所以需要??重置
        public override void Reset()
        {
            // 清空 TextureHandle
            m_TextureHandleFront = TextureHandle.nullHandle;
            m_TextureHandleBack = TextureHandle.nullHandle;
            texture = TextureHandle.nullHandle;
            // 恢? m_IsFront ? true，重新从 Front ?始
            m_IsFront = true;
        }

        // 用于?一个 Blit Pass 的?入?出??体
        // ?一个 blit pass 都需要知道：来源?理（通常是上一次的?果），?出?理（双?冲中的?一个），?理使用的材?（材?中可能会?入特定 shader）
        class PassData
        {
            public TextureHandle source;
            public TextureHandle destination;
            public Material material;
        }

        // ?像机?像 到 texture
        public void RecordBlitColor(RenderGraph renderGraph, ContextContainer frameData)
        {
            // 使用 !IsValid() 来判断 “?一?的句柄是否已??置?”。没?置?：初始化?理和句柄。?置?：?明?一?已?准?好?理?源，可以直接使用。
            if (!texture.IsValid())
            {
                // 拿到当前?像机的目??理格式（尺寸、?色格式等）
                var cameraData = frameData.Get<UniversalCameraData>();
                var descriptor = cameraData.cameraTargetDescriptor;
                // 禁用 MSAA 和深度（blit 只?理?色）
                descriptor.msaaSamples = 1;
                descriptor.depthStencilFormat = UnityEngine.Experimental.Rendering.GraphicsFormat.None;

                Init(renderGraph, descriptor);
            }

            // 根据 pass 的名称?始??render graph pass
            // 并?出用于将数据????染函数?行的数据。
            // "BlitColorPass" 是?次 pass 的名字
            // 此?的 using 是 C# 的一??法糖，是 C# ? IDisposable ?象的明??源管理方式，用于自??放?源。
            // 当?在 using (...) {} ?法??束?，它会自??用 builder.Dispose()，完成底?注册??、??、清理等任?。
            using (var builder = renderGraph.AddRasterRenderPass<PassData>("BlitColorPass", out var passData))
            {
                // ?取了当前?像机的?源上下文，包括?像机的 Color RT 和 Depth RT
                var resourceData = frameData.Get<UniversalResourceData>();

                // 由于 material 包含了上一?的信息，所以需要?行重置
                passData.material = null;
                // ?取当前相机?色?冲作? source
                passData.source = resourceData.activeColorTexture;
                // ?置 destination：当前自定?的 Blit ?理?冲区（通常是?用来?行后?理的???理）
                passData.destination = texture;

                // 注册 source ?理作??入依?。?是告知 Render Graph：?个 pass 会?取??理，因此不能在其它地方被修改，?保?染流程一致性。
                builder.UseTexture(passData.source);
                // ?置 ?出?染目?：将?染?果?出到 destination。参数 0 是指 Render Target 0（也就是常??色?出口）。
                builder.SetRenderAttachment(passData.destination, 0);

                // ?置?染函数 SetRenderFunc
                // ?置? pass 的真正?行??函数：ExecutePass(...)，ExecutePass 通常包含 Blitter.BlitTexture(rgContext.cmd, data.source, ..., data.material, ...)
                // => 是 C# 中的Lambda 表?式?法，也叫“箭?函数”。
                // SetRenderFunc：注册 将来?行?个 Pass 的函数（RenderGraph 会在?行?段真正?用它）。
                // ExecutePass：?指定的???行??，比如?用 Blitter.BlitTexture() 等
                builder.SetRenderFunc((PassData passData, RasterGraphContext rgContext) => ExecutePass(passData, rgContext));
            }
        }

        // 最??果 到 ?像机 color buffer
        public void RecordBlitBackToColor(RenderGraph renderGraph, ContextContainer frameData)
        {
            // 如果 BlitData's texture 是 invalid ?明它没有被初始化 或者 是 error ?生了
            if (!texture.IsValid()) return;

            // 根据 pass 的名称?始??render graph pass
            // 并?出用于将数据????染函数?行的数据。
            using (var builder = renderGraph.AddRasterRenderPass<PassData>($"BlitBackToColorPass", out var passData))
            {
                // ?取了当前?像机的?源上下文，包括?像机的 Color RT 和 Depth RT
                var resourceData = frameData.Get<UniversalResourceData>();

                passData.material = null;
                passData.source = texture;
                passData.destination = resourceData.activeColorTexture;

                // 注册 source ?理作??入依?。
                builder.UseTexture(passData.source);
                // ?置 ?出?染目?：将?染?果?出到 destination。参数 0 是指 Render Target 0（也就是常??色?出口）。
                builder.SetRenderAttachment(passData.destination, 0);

                // ?置?染函数 SetRenderFunc
                builder.SetRenderFunc((PassData passData, RasterGraphContext rgContext) => ExecutePass(passData, rgContext));
            }
        }

        // BlitData ?中的一个方法，用于向当前?的 RenderGraph 中 添加一个全屏?理的 pass，使用提供的 Material ?画面?行?理。
        public void RecordFullScreenPass(RenderGraph renderGraph, string passName, Material material)
        {
            // ??是否已有上一?的?出?理 (texture) 以及材?是否有效
            // 如果没有就直接 return，?防止??非法的 GPU ?源?致??。
            if (!texture.IsValid() || material == null)
            {
                Debug.LogWarning("Invalid input texture handle, will skip fullscreen pass.");
                return;
            }

            // ?始注册 Raster Pass。
            // ?建一个 Raster ?型的 RenderPass。?个 Pass 会在 GPU 上?行?制操作（通常就是全屏四?形）。
            using (var builder = renderGraph.AddRasterRenderPass<PassData>(passName, out var passData))
            {
                // 交?前后?冲区(Ping-Pong Buffering)
                m_IsFront = !m_IsFront;

                // ?置?入数据。使用之前??的 texture 作??入。使用提供的 material 来?行?理。
                passData.material = material;
                passData.source = texture;

                // 交??个 active texture。通? m_IsFront 判断当前?写入?一个 RT Handle。
                if (m_IsFront)
                    passData.destination = m_TextureHandleFront;
                else
                    passData.destination = m_TextureHandleBack;

                // .UseTexture() 告? RenderGraph：我?一 pass ?取了 source
                builder.UseTexture(passData.source);
                // .SetRenderAttachment() 告?它：我的 color output 是 destination（Blit的?出目?）
                builder.SetRenderAttachment(passData.destination, 0);

                // 更新当前的 active ?出?理。
                // ?次?行完一个 pass，我?都把 texture 指向最新的 output。??下一次?用?，?取的就会是?一?中最近更新?的内容。
                texture = passData.destination;

                builder.SetRenderFunc((PassData passData, RasterGraphContext rgContext) => ExecutePass(passData, rgContext));
            }
        }

        static void ExecutePass(PassData data, RasterGraphContext rgContext)
        {
            // Blitter.BlitTexture() 是 Unity 内部的高效全屏?制封装
            if (data.material == null)
                // 直接?制 source 到目??理，无任何?理
                Blitter.BlitTexture(rgContext.cmd, data.source, scaleBias, 0, false);
            else
                // 使用 material ? source ?行?理后?出到目??理
                Blitter.BlitTexture(rgContext.cmd, data.source, scaleBias, data.material, 0);
        }

        // 和 using 作用域?束??用的 Dispose 不是同一个
        public void Dispose()
        {
            m_TextureFront?.Release();
            m_TextureBack?.Release();
        }
    }

    // Initial render pass for the renderer feature which is run to initialize the data in frameData and copying
    // the camera's color attachment to a texture inside BlitData so we can do transformations using blit.
    class BlitStartRenderPass : ScriptableRenderPass
    {
        public override void RecordRenderGraph(RenderGraph renderGraph, ContextContainer frameData)
        {
            // Creating the data BlitData inside frameData.
            var blitTextureData = frameData.Create<BlitData>();
            // Copies the camera's color attachment to a texture inside BlitData.
            blitTextureData.RecordBlitColor(renderGraph, frameData);
        }
    }

    // Render pass which makes a blit for each material given to the renderer feature.
    class BlitRenderPass : ScriptableRenderPass
    {
        List<Material> m_Materials;

        // Setup function used to retrive the materials from the renderer feature.
        public void Setup(List<Material> materials)
        {
            m_Materials = materials;
        }

        public override void RecordRenderGraph(RenderGraph renderGraph, ContextContainer frameData)
        {
            // Retrives the BlitData from the current frame.
            var blitTextureData = frameData.Get<BlitData>();
            foreach(var material in m_Materials)
            {
                // Skip current cycle if the material is null since there is no need to blit if no
                // transformation happens.
                if (material == null) continue;
                // Records the material blit pass.
                blitTextureData.RecordFullScreenPass(renderGraph, $"Blit {material.name} Pass", material);
            }    
        }
    }

    // Final render pass to copying the texture back to the camera's color attachment.
    class BlitEndRenderPass : ScriptableRenderPass
    {
        public override void RecordRenderGraph(RenderGraph renderGraph, ContextContainer frameData)
        {
            // Retrives the BlitData from the current frame and blit it back again to the camera's color attachment.
            var blitTextureData = frameData.Get<BlitData>();
            blitTextureData.RecordBlitBackToColor(renderGraph, frameData);
        }
    }

    [SerializeField]
    [Tooltip("Materials used for blitting. They will be blit in the same order they have in the list starting from index 0. ")]
    List<Material> m_Materials;

    BlitStartRenderPass m_StartPass;
    BlitRenderPass m_BlitPass;
    BlitEndRenderPass m_EndPass;

    // Here you can create passes and do the initialization of them. This is called everytime serialization happens.
    public override void Create()
    {
        m_StartPass = new BlitStartRenderPass();
        m_BlitPass = new BlitRenderPass();
        m_EndPass = new BlitEndRenderPass();

        // Configures where the render pass should be injected.
        m_StartPass.renderPassEvent = RenderPassEvent.AfterRenderingPostProcessing;
        m_BlitPass.renderPassEvent = RenderPassEvent.AfterRenderingPostProcessing;
        m_EndPass.renderPassEvent = RenderPassEvent.AfterRenderingPostProcessing;
    }

    // Here you can inject one or multiple render passes in the renderer.
    // This method is called when setting up the renderer once per-camera.
    public override void AddRenderPasses(ScriptableRenderer renderer, ref RenderingData renderingData)
    {
        // Early return if there is no texture to blit.
        if (m_Materials == null || m_Materials.Count == 0) return;

        // Pass the material to the blit render pass.
        m_BlitPass.Setup(m_Materials);

        // Since they have the same RenderPassEvent the order matters when enqueueing them.
        renderer.EnqueuePass(m_StartPass);
        renderer.EnqueuePass(m_BlitPass);
        renderer.EnqueuePass(m_EndPass);
    }
}


