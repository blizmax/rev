//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Ag�era Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Base component in the component system
#pragma once

#include <iostream>

namespace rev { namespace game {

	class SceneNode;

	class Component
	{
	public:
		// -- Constructor & destructor --
		Component() = default;
		virtual	~Component();

		virtual void init	() {}
		virtual void update(float _dt) {}

		// -- attach and dettach --
		void	attachTo(SceneNode * _node);
		void	dettach();

		SceneNode * node() const { return mNode; }

	private:
		SceneNode * mNode = nullptr;
	};

}}	// namespace rev::game