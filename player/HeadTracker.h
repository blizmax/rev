//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Ag�era Tortosa
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include <core/components/component.h>

namespace rev {

	class HeadTracker : public core::Component {
	public:
		HeadTracker(const std::string& _datasetName);

		bool update(float _dt);

		math::Mat34f headTransform() const;
	};

}
