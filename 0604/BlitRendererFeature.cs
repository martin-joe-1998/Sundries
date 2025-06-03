using System;
using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEngine;
using UnityEngine.Rendering;
using UnityEngine.Rendering.RenderGraphModule;
using UnityEngine.Rendering.Universal;

// 该 SRF 包含 3 个主要 pass
// BlitStartRenderPass：从摄像机的颜色缓冲拷贝数据出来（准备 Blit 输入纹理）
// BlitRenderPass：按顺序对多个材质执行全屏 Blit 操作
// BlitEndRenderPass：将最终处理后的图像再 Blit 回摄像机
// 被挂到 URP 的 Renderer Data 资源中后，每帧会自动调用其 Create() 和 AddRenderPasses()。
public class BlitRendererFeature : ScriptableRendererFeature
{
    // 管理 blit 使用的中间纹理。会被放进 ContextContainer（Unity 的 per-frame 数据容器），用于跨多个 RenderPass 共享状态。
    public class BlitData : ContextItem, IDisposable
    {
        // Ping-Pong 双重缓冲
        // RTHandle 是 Unity 的 RenderTexture 封装类（跨分辨率适配）
        RTHandle m_TextureFront;
        RTHandle m_TextureBack;
        // TextureHandle 是 RenderGraph 用于管理图像依赖的句柄（逻辑图结构）
        TextureHandle m_TextureHandleFront;
        TextureHandle m_TextureHandleBack;

        // scaleBias 是屏幕适配用的
        static Vector4 scaleBias = new Vector4(1f, 1f, 0f, 0f);

        // 控制当前激活的是哪一张纹理（ping-pong）
        bool m_IsFront = true;

        // 表示当前“有效结果”的图像句柄
        public TextureHandle texture;

        // 使用摄像机当前的 RenderTextureDescriptor 初始化 RTHandle
        // 然后通过 renderGraph.ImportTexture 转换成 TextureHandle，以供 RenderGraph 使用
        // 每帧需要调用一次的初始化逻辑
        public void Init(RenderGraph renderGraph, RenderTextureDescriptor targetDescriptor, string textureName = null)
        {
            var texName = String.IsNullOrEmpty(textureName) ? "_BlitTexture" : textureName;
            // 会自动重分配尺寸优化的 RTHandle，ReAllocateHandleIfNeeded会自动把 texName 注册到全局 texture
            RenderingUtils.ReAllocateHandleIfNeeded(ref m_TextureFront, targetDescriptor, FilterMode.Bilinear, TextureWrapMode.Clamp, name: texName + "Front");
            RenderingUtils.ReAllocateHandleIfNeeded(ref m_TextureBack, targetDescriptor, FilterMode.Bilinear, TextureWrapMode.Clamp, name: texName + "Back");

            m_TextureHandleFront = renderGraph.ImportTexture(m_TextureFront);
            m_TextureHandleBack = renderGraph.ImportTexture(m_TextureBack);
            // 设置初始有效图像为 m_TextureHandleFront
            texture = m_TextureHandleFront;
        }

        // RenderGraph 只保存当前帧资源，所以需要每帧重置
        public override void Reset()
        {
            // 清空 TextureHandle
            m_TextureHandleFront = TextureHandle.nullHandle;
            m_TextureHandleBack = TextureHandle.nullHandle;
            texture = TextureHandle.nullHandle;
            // 恢复 m_IsFront 为 true，重新从 Front 开始
            m_IsFront = true;
        }

        // 用于每一个 Blit Pass 的输入输出结构体
        class PassData
        {
            public TextureHandle source;
            public TextureHandle destination;
            public Material material;
        }

        // 摄像机图像 到 texture
        public void RecordBlitColor(RenderGraph renderGraph, ContextContainer frameData)
        {
            // 使用 !IsValid() 来判断 “这一帧的句柄是否已经设置过”：没设置过：初始化纹理和句柄。设置过：说明这一帧已经准备好纹理资源，可以直接使用。
            if (!texture.IsValid())
            {
                // 拿到当前摄像机的目标纹理格式（尺寸、颜色格式等）
                var cameraData = frameData.Get<UniversalCameraData>();
                var descriptor = cameraData.cameraTargetDescriptor;
                // 禁用 MSAA 和深度（blit 只处理颜色）
                descriptor.msaaSamples = 1;
                descriptor.depthStencilFormat = UnityEngine.Experimental.Rendering.GraphicsFormat.None;

                Init(renderGraph, descriptor);
            }

            // 根据 pass 的名称初始化render graph pass
            // 并输出用于将数据输送到渲染函数执行的数据。
            // using 是 C# 的一种语法糖，用于自动释放资源，是 C# 对 IDisposable 对象的明确资源管理方式
            // 当你在 using (...) {} 语法块结束时，它会自动调用 .Dispose()，完成底层注册逻辑、验证、清理等任务。
            using (var builder = renderGraph.AddRasterRenderPass<PassData>("BlitColorPass", out var passData))
            {
                // 从上下文 frameData 中获取当前相机帧的资源数据（activeColorTexture 和 activeDepthTexture）
                var resourceData = frameData.Get<UniversalResourceData>();

                // 由于 material 包含了上一帧的信息，所以需要进行重置
                passData.material = null;
                // 从相机的颜色缓冲区获得的原始画面图像
                passData.source = resourceData.activeColorTexture;
                // 设置 destination：当前自定义的 Blit 纹理缓冲区（通常是用来进行后处理的临时纹理）
                passData.destination = texture;

                // 注册 source 纹理作为输入依赖。告知 Render Graph：这个 pass 会读取该纹理，因此不能在其它地方被修改，确保渲染流程一致性。
                builder.UseTexture(passData.source);
                // 设置 输出渲染目标：将渲染结果输出到 destination。参数 0 是指 Render Target 0（也就是常规颜色输出口）。
                builder.SetRenderAttachment(passData.destination, 0);

                // 设置渲染函数 SetRenderFunc
                // 设置该 pass 的真正执行渲染函数：ExecutePass(...)，ExecutePass 包含 Blitter.BlitTexture(rgContext.cmd, data.source, ..., data.material, ...)
                // => 是 C# 中的Lambda 表达式语法，也叫“箭头函数”
                // SetRenderFunc 注册 将来执行这个 Pass 的函数（RenderGraph 会在执行阶段真正调用它）
                builder.SetRenderFunc((PassData passData, RasterGraphContext rgContext) => ExecutePass(passData, rgContext));
            }
        }

        // 最终结果 到 摄像机 color buffer
        public void RecordBlitBackToColor(RenderGraph renderGraph, ContextContainer frameData)
        {
            // 如果 BlitData's texture 是 invalid 说明它没有被初始化 或者 是 error 发生了
            if (!texture.IsValid()) return;

            using (var builder = renderGraph.AddRasterRenderPass<PassData>($"BlitBackToColorPass", out var passData))
            {
                var resourceData = frameData.Get<UniversalResourceData>();

                passData.material = null;
                passData.source = texture;
                passData.destination = resourceData.activeColorTexture;

                builder.UseTexture(passData.source);
                builder.SetRenderAttachment(passData.destination, 0);
                builder.SetRenderFunc((PassData passData, RasterGraphContext rgContext) => ExecutePass(passData, rgContext));
            }
        }

        // BlitData 类中的一个方法，用于向当前帧的 RenderGraph 中 添加一个全屏处理的 pass，使用提供的 Material 对画面进行处理。
        public void RecordFullScreenPass(RenderGraph renderGraph, string passName, Material material)
        {
            // 检查是否已有上一步的输出纹理 (texture) 以及材质是否有效
            // 如果没有就直接 return，防止访问非法的 GPU 资源导致错误
            if (!texture.IsValid() || material == null)
            {
                Debug.LogWarning("Invalid input texture handle, will skip fullscreen pass.");
                return;
            }

            // 开始注册 Raster Pass
            // 创建一个 Raster 类型的 RenderPass。这个 Pass 会在 GPU 上执行绘制操作（通常就是全屏四边形）。
            using (var builder = renderGraph.AddRasterRenderPass<PassData>(passName, out var passData))
            {
                // 交换前后缓冲区(Ping-Pong Buffering)
                m_IsFront = !m_IsFront;

                // 使用之前记录的 texture 作为输入，并使用提供的 material 来进行处理。
                passData.material = material;
                passData.source = texture;

                // 通过 m_IsFront 判断当前该写入哪一个 RT Handle。
                if (m_IsFront)
                    passData.destination = m_TextureHandleFront;
                else
                    passData.destination = m_TextureHandleBack;

                // .UseTexture() 告诉 RenderGraph：我这一 pass 读取了 source。
                builder.UseTexture(passData.source);
                // .SetRenderAttachment() 告诉它：我的 color output 是 destination。
                builder.SetRenderAttachment(passData.destination, 0);

                // 更新当前的 active 输出纹理。
                // 每次执行完一个 pass，都把 texture 指向最新的 output。这样下一次调用时，读取的就会是这一帧中最近更新过的内容。
                texture = passData.destination;

                builder.SetRenderFunc((PassData passData, RasterGraphContext rgContext) => ExecutePass(passData, rgContext));
            }
        }

        static void ExecutePass(PassData data, RasterGraphContext rgContext)
        {
            // Blitter.BlitTexture() 是 Unity 内部的高效全屏复制封装
            if (data.material == null)
                // 直接复制 source 到目标纹理，无任何处理
                Blitter.BlitTexture(rgContext.cmd, data.source, scaleBias, 0, false);
            else
                // 使用 material 对 source 进行处理后输出到目标纹理
                Blitter.BlitTexture(rgContext.cmd, data.source, scaleBias, data.material, 0);
        }

        // 和 using 作用域结束时调用的 Dispose 不是同一个
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


