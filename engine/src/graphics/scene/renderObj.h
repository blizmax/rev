//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
// Render scene primitive containing geometry and transform
#pragma once

#include <math/algebra/affineTransform.h>
#include <memory>
#include <vector>

namespace rev { namespace graphics {

	class RenderMesh;

	class RenderObj
	{
	public:
		RenderObj()
			: transform(math::AffineTransform::identity())
		{}

		RenderObj(const std::shared_ptr<const RenderMesh>& _mesh)
			: RenderObj()
		{
			mesh = _mesh;
		}

		math::AffineTransform	transform;
		std::shared_ptr<const RenderMesh> mesh;
	};

}}	// namespace rev::graphics
