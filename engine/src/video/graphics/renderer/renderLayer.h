//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Ag�era Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include <core/world/worldLayer.h>
#include <vector>

namespace rev {
	namespace video {

		class ForwardRenderer;
		class RenderObj;

		class RenderLayer : public core::WorldLayer {
		public:
			RenderLayer(ForwardRenderer* _r) : mRenderer (_r) {}
			void update(float _dt) override;

		private:
			std::vector<RenderObj*>	mObjects;
			ForwardRenderer*		mRenderer;
		};

} }	// namespace rev::video