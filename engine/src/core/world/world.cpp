//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Ag�era Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#include "world.h"
#include "worldLayer.h"

namespace rev {
	namespace core {

		//--------------------------------------------------------------------------------------------------------------
		void World::addLayer(WorldLayer* _l) {
			mLayers.push_back(_l);
		}

		//--------------------------------------------------------------------------------------------------------------
		void World::init() {
			for(auto layer : mLayers)
				layer->init();
		}

		//--------------------------------------------------------------------------------------------------------------
		bool World::update(float _dt) {
			for(auto layer : mLayers)
				if(!layer->update(_dt))
					return false;
			return true;
		}

} }