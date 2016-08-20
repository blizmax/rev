//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Ag�era Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#include "shadowPass.h"

#include <video/graphics/driver/graphicsDriver.h>
#include <video/graphics/renderer/backend/rendererBackEnd.h>
#include <video/graphics/shader/shader.h>
#include <video/graphics/renderer/types/renderTarget.h>
#include <video/graphics/renderer/debugDrawer.h>

using namespace rev::math;

namespace rev {
	namespace video {

		//--------------------------------------------------------------------------------------------------------------
		ShadowPass::ShadowPass(GraphicsDriver* _driver, RendererBackEnd* _backEnd, size_t _bufferSize)
			: mDriver(_driver)
			, mBackEnd(_backEnd)
		{
			mShadowShader = Shader::manager()->get("shadow");

			mShadowBuffer = mDriver->createRenderTarget({ _bufferSize, _bufferSize}, Texture::EImageFormat::depth, Texture::EByteFormat::eFloat);
		}

		//--------------------------------------------------------------------------------------------------------------
		ShadowPass::~ShadowPass()
		{
			mDriver->destroyRenderTarget(mShadowBuffer);
		}

		//--------------------------------------------------------------------------------------------------------------
		void ShadowPass::config(const math::Vec3f& _lightDir, const math::Mat34f& _viewMat, const math::Frustum& _viewFrustum, float _depth) {
			// ---- Compute shadow transform axes ----
			Mat34f shadowBasis = Mat34f::identity();
			if (abs(_lightDir * _viewMat.col(1)) < 0.71f) { // Only use the view direction if it is not too aligned with the light
				auto shadowX = _viewMat.col(1) ^ _lightDir; // view.y ^ light;
				auto shadowY = _lightDir ^ shadowX;
				shadowBasis.setCol(0, shadowX);
				shadowBasis.setCol(1, shadowY);
				// auto lightUp = _viewMat.col(1) ^ _lightDir;
				// auto lightSide = _lightDir ^ lightUp;
				// shadowBasis.setCol(0, lightSide);
				// shadowBasis.setCol(1, lightUp);
				// shadowBasis.setCol(2, _lightDir);
			}
			else {
				auto shadowY = _lightDir ^ _viewMat.col(0);
				auto shadowX = shadowY ^_lightDir;
				shadowBasis.setCol(0, shadowX);
				shadowBasis.setCol(1, shadowY);
				// auto lightUp = _viewMat.col(2) ^ _lightDir;
				// auto lightSide = _lightDir ^ lightUp;
				// shadowBasis.setCol(0, lightSide);
				// shadowBasis.setCol(1, lightUp);
				// shadowBasis.setCol(2, _lightDir);
			}
			shadowBasis.setCol(2, _lightDir);

			// ---- Project frustum into the basis to compute the bounding box ---
			Vec3f frustumWorld[8];
			for (size_t i = 0; i < 8; ++i) {
				frustumWorld[i] = _viewMat*_viewFrustum.vertices()[i];
			}
			Vec3f centroidWorld = _viewMat * Vec3f(0.f, _viewFrustum.centroid(), 0.f);
			BBox shadowBB(centroidWorld, centroidWorld);
			// Transfrom frustum into shadow space to compute bounding box
			Mat34f invShadow;
			shadowBasis.inverse(invShadow);
			for (size_t i = 0; i < 8; ++i) {
				shadowBB.merge(invShadow * frustumWorld[i]);
			}
			// Recenter shadow basis
			shadowBasis.setCol(3, centroidWorld); // TODO: Match lower surface of the bb with lower side of the frustum
			Mat44f shadowProj = Mat44f::ortho({ shadowBB.size().x,shadowBB.size().y, _depth });
			shadowBasis.inverse(invShadow);
			if (mDebug) {
				mDebug->drawBasis(shadowBasis);
				mDebug->drawLine(centroidWorld-_lightDir*50.f, centroidWorld, Color(1.f,0.f,0.f));
				mDebug->drawLine(centroidWorld, centroidWorld + _lightDir*50.f, Color(0.f, 1.f, 1.f));
			}

			mBackEnd->setCamera(shadowBasis, shadowProj);
			mViewProj = shadowProj * invShadow;
			// Configure renderer
			mDriver->setRenderTarget(mShadowBuffer);
			mDriver->setViewport(math::Vec2i(0, 0), mShadowBuffer->tex->size);
			mDriver->clearZBuffer();
			mDriver->setCulling(GraphicsDriver::ECulling::eFront);
			mBackEnd->setShader(mShadowShader);
		}
	}
}