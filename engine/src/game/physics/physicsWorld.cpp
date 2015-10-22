//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Ag�era Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Physics simulation context
#include "physicsWorld.h"

namespace rev {
	namespace game {
		//--------------------------------------------------------------------------------------------------------------
		PhysicsWorld::PhysicsWorld() {
			mCollisionConfig = new btDefaultCollisionConfiguration();
			mDispatcher = new btCollisionDispatcher(mCollisionConfig);
			mOverlappingPairCache = new btDbvtBroadphase();
			mSolver = new btSequentialImpulseConstraintSolver();
			mWorld = new btDiscreteDynamicsWorld(mDispatcher,
				mOverlappingPairCache, mSolver, mCollisionConfig);
			mWorld->setGravity(btVector3(0.f, 0.f, -9.819f));
		}

		//--------------------------------------------------------------------------------------------------------------
		PhysicsWorld::~PhysicsWorld() {
			delete mWorld;
			delete mSolver;
			delete mOverlappingPairCache;
			delete mDispatcher;
			delete mCollisionConfig;
		}
	}
}