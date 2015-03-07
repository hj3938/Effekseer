
#ifndef	__EFFEKSEERRENDERER_STANDARD_RENDERER_BASE_H__
#define	__EFFEKSEERRENDERER_STANDARD_RENDERER_BASE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <Effekseer.h>

#include "EffekseerRenderer.VertexBufferBase.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

struct StandardRendererState
{
	bool								DepthTest;
	bool								DepthWrite;
	bool								Distortion;

	::Effekseer::AlphaBlendType			AlphaBlend;
	::Effekseer::CullingType			CullingType;
	::Effekseer::TextureFilterType		TextureFilterType;
	::Effekseer::TextureWrapType		TextureWrapType;
	void*								TexturePtr;

	StandardRendererState()
	{
		DepthTest = false;
		DepthWrite = false;
		Distortion = false;
		AlphaBlend = ::Effekseer::AlphaBlendType::Blend;
		CullingType = ::Effekseer::CullingType::Front;
		TextureFilterType = ::Effekseer::TextureFilterType::Nearest;
		TextureWrapType = ::Effekseer::TextureWrapType::Repeat;
		TexturePtr = NULL;
	}

	bool operator != (const StandardRendererState state)
	{
		if (DepthTest != state.DepthTest) return true;
		if (DepthWrite != state.DepthWrite) return true;
		if (Distortion != state.Distortion) return true;
		if (AlphaBlend != state.AlphaBlend) return true;
		if (CullingType != state.CullingType) return true;
		if (TextureFilterType != state.TextureFilterType) return true;
		if (TextureWrapType != state.TextureWrapType) return true;
		if (TexturePtr != state.TexturePtr) return true;
		return false;
	}
};

template<typename RENDERER, typename SHADER, typename TEXTURE, typename VERTEX>
class StandardRenderer
{

private:
	RENDERER*	m_renderer;
	SHADER*		m_shader;
	SHADER*		m_shader_no_texture;

	SHADER*		m_shader_distortion;
	SHADER*		m_shader_no_texture_distortion;

	TEXTURE*	m_texture;

	StandardRendererState		m_state;

	std::vector<uint8_t>		vertexCaches;
	int32_t						vertexCacheMaxSize = 0;

public:

	StandardRenderer(RENDERER* renderer, SHADER* shader, SHADER* shader_no_texture, SHADER* shader_distortion, SHADER* shader_no_texture_distortion)
	{
		m_renderer = renderer;
		m_shader = shader;
		m_shader_no_texture = shader_no_texture;
		m_shader_distortion = shader_distortion;
		m_shader_no_texture_distortion = shader_no_texture_distortion;

		vertexCaches.reserve(m_renderer->GetVertexBuffer()->GetMaxSize());
		vertexCacheMaxSize = m_renderer->GetVertexBuffer()->GetMaxSize();
	}

	void UpdateStateAndRenderingIfRequired(StandardRendererState state)
	{
		if(m_state != state)
		{
			Rendering();
		}

		m_state = state;
	}

	void BeginRenderingAndRenderingIfRequired(int32_t count, int32_t& offset, void*& data)
	{
		if (count * sizeof(VERTEX) + vertexCaches.size() > vertexCacheMaxSize)
		{
			Rendering();
		}

		auto old = vertexCaches.size();
		vertexCaches.resize(count * sizeof(VERTEX) + vertexCaches.size());

		offset = old;
		data = (vertexCaches.data() + old);
	}

	void ResetAndRenderingIfRequired()
	{
		Rendering();

		// 必ず次の描画で初期化される。
		m_state.TexturePtr = (void*)0x1;
	}

	void Rendering(const Effekseer::Matrix44& cvp)
	{
		if (vertexCaches.size() == 0) return;

		int32_t vertexSize = vertexCaches.size();
		int32_t offsetSize = 0;
		{
			VertexBufferBase* vb = m_renderer->GetVertexBuffer();

			void* data = nullptr;

			vb->RingBufferLock(vertexCaches.size(), offsetSize, data);

			memcpy(data, vertexCaches.data(), vertexCaches.size());
			vertexCaches.clear();

			vb->Unlock();
		}

		RenderStateBase::State& state = m_renderer->GetRenderState()->Push();
		state.DepthTest = m_state.DepthTest;
		state.DepthWrite = m_state.DepthWrite;
		state.CullingType = m_state.CullingType;

		SHADER* shader_ = nullptr;

		bool distortion = m_state.Distortion;

		if (distortion)
		{
			if (m_state.TexturePtr != nullptr)
			{
				shader_ = m_shader_distortion;
			}
			else
			{
				shader_ = m_shader_no_texture_distortion;
			}
		}
		else
		{
			if (m_state.TexturePtr != nullptr)
			{
				shader_ = m_shader;
			}
			else
			{
				shader_ = m_shader_no_texture;
			}
		}

		m_renderer->BeginShader(shader_);

		TEXTURE textures[2];

		if (m_state.TexturePtr != nullptr)
		{
			textures[0] = TexturePointerToTexture<TEXTURE>(m_state.TexturePtr);
		}
		else
		{
			textures[0] = 0;
		}

		if (distortion)
		{
			textures[1] = m_renderer->GetBackground();
			m_renderer->SetTextures(shader_, textures, 2);
		}
		else
		{
			m_renderer->SetTextures(shader_, textures, 1);
		}

		((Effekseer::Matrix44*)(shader_->GetVertexConstantBuffer()))[0] =cvp;
		shader_->SetConstantBuffer();

		state.AlphaBlend = m_state.AlphaBlend;
		state.TextureFilterTypes[0] = m_state.TextureFilterType;
		state.TextureWrapTypes[0] = m_state.TextureWrapType;

		m_renderer->GetRenderState()->Update(false);

		m_renderer->SetVertexBuffer(m_renderer->GetVertexBuffer(), sizeof(VERTEX));
		m_renderer->SetIndexBuffer(m_renderer->GetIndexBuffer());
		m_renderer->SetLayout(shader_);
		m_renderer->DrawSprites(vertexSize / sizeof(VERTEX) / 4, offsetSize / sizeof(VERTEX));

		m_renderer->EndShader(shader_);

		m_renderer->GetRenderState()->Pop();
	}

	void Rendering()
	{
		Rendering(m_renderer->GetCameraProjectionMatrix());
	}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEERRENDERER_STANDARD_RENDERER_H__