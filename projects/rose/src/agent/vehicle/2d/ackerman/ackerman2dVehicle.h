//----------------------------------------------------------------------------------------------------------------------
// Rose: Robot oriented simulation environment
// Created by Carmelo J. Fdez-Ag�era Tortosa a.k.a. (Technik)
// On August 8th, 2012
//----------------------------------------------------------------------------------------------------------------------
// 2d ackerman vehicle agent

#ifndef _ROSE_AGENT_VEHICLE_2D_ACKERMAN_ACKERMAN2DVEHICLE_H_
#define _ROSE_AGENT_VEHICLE_2D_ACKERMAN_ACKERMAN2DVEHICLE_H_

#include <agent/agent.h>
#include <math/algebra/vector.h>

namespace rose { namespace agent
{
	class Ackerman2dVehicle : Agent
	{
	public:
		// Construction and destruction
		Ackerman2dVehicle();
		~Ackerman2dVehicle() {}

		void simulate(unsigned _milliseconds);

	private:
		rev::math::Vec2f	mPosition;
		rev::math::Vec2f	mVelocity;
	};

}	// namespace agent
}	// namespace rose

#endif // _ROSE_AGENT_VEHICLE_2D_ACKERMAN_ACKERMAN2DVEHICLE_H_