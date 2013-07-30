//----------------------------------------------------------------------------------------------------------------------
// Graphics 3d module
// Created by Carmelo J. Fdez-Ag�era Tortosa (a.k.a. Technik)
// On 2013/July/2
//----------------------------------------------------------------------------------------------------------------------
// render scene

#ifndef _REV_GRAPHICS3D_RENDERABLE_RENDERSCENE_H_
#define _REV_GRAPHICS3D_RENDERABLE_RENDERSCENE_H_

#include <vector>
#include <functional>

namespace rev { namespace graphics3d {

	class Renderable;

	class RenderScene
	{
	public:
		static RenderScene* get	();

		void	add				(const Renderable*);
		void	remove			(const Renderable*);

		void	traverse		(std::function<void (const Renderable*)>) const;

	private:
		RenderScene() {}

	private:
		static RenderScene*				sInstance;
		std::vector<const Renderable*>	mRenderQueue;
	};

}	// namespace graphics3d
}	// namespace rev

#endif // _REV_GRAPHICS3D_RENDERABLE_RENDERSCENE_H_