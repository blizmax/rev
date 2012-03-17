////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fern�ndez-Ag�era Tortosa (a.k.a. Technik)
// Created on March 15th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Rigid Body

#include "rigidBody.h"

#include <revCore/codeTools/assert/assert.h>

namespace rev { namespace physics
{
	// Static data definitin
	CPhysicsWorld * CRigidBody::defaultWorld = 0;

	//-----------------------------------------------------------------------------------------------------------------
	CRigidBody::CRigidBody(float _mass)
		:mInvMass(0.f)
		,mLinearVelocity(CVec3::zero)
		,mAngularVelocity(CVec3::zero)
		,mForce(CVec3::zero)
		,mTorque(CVec3::zero)
	{
		setMass(_mass);
	}

	//-----------------------------------------------------------------------------------------------------------------
	CRigidBody::~CRigidBody()
	{
	}

	//-----------------------------------------------------------------------------------------------------------------
	void CRigidBody::integrate( float _time )
	{
		// Apply forces
		applyImpulse(mForce * _time);
		// Apply friction
		applyFriction();
		// Integrate motion
		setPosition(position() + mLinearVelocity * _time);
		if(!(mAngularVelocity == CVec3::zero))
		{
			setRotation(CQuat(mAngularVelocity * _time) * rotation());
		}
	}

	//-----------------------------------------------------------------------------------------------------------------
	void CRigidBody::applyForce(const CVec3& _f, const CVec3& _pos)
	{
		mForce = mForce + _f;
		CVec3 distanceToG = _pos - position();
		mTorque = mTorque + (_f ^ distanceToG);
	}

	//-----------------------------------------------------------------------------------------------------------------
	void CRigidBody::applyForce(const CVec3& _f)
	{
		mForce = mForce + _f;
	}

	//-----------------------------------------------------------------------------------------------------------------
	void CRigidBody::applyTorque(const CVec3& _t)
	{
		mTorque = mTorque + _t;
	}

	//-----------------------------------------------------------------------------------------------------------------
	void CRigidBody::applyImpulse(const CVec3& _i, const CVec3& _pos)
	{
		mLinearVelocity = mLinearVelocity + _i*mInvMass;
		_pos;
		// CVec3 distanceToG = _pos - position();
		// mAngularVelocity = mAngularVelocity + (_i ^ distanceToG).componentProduct(mInvInertia)
	}

	//-----------------------------------------------------------------------------------------------------------------
	void CRigidBody::applyImpulse(const CVec3& _i)
	{
		mLinearVelocity = mLinearVelocity + _i*mInvMass;
	}

	//-----------------------------------------------------------------------------------------------------------------
	void CRigidBody::applyTorqueImpulse(const CVec3& _t)
	{
		_t;
	}

	//-----------------------------------------------------------------------------------------------------------------
	void CRigidBody::setMass(float _mass)
	{
		mInvMass = _mass==0.f? 0.f : (1.f / _mass);
	}

	//-----------------------------------------------------------------------------------------------------------------
	void CRigidBody::applyFriction()
	{
		// TODO
	}

}	// namespace physics
}	// namespace rev