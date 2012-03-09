////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, video driver interface
// by Carmelo J. Fern�ndez-Ag�era Tortosa (a.k.a. Technik)
// Created on August 21st, 2011
////////////////////////////////////////////////////////////////////////////////
// vectors

#ifndef _REV_REVCORE_MATH_VECTOR_H_
#define _REV_REVCORE_MATH_VECTOR_H_

#include "revCore/types.h"

namespace rev
{
	//--------------------------------------------------------------------------
	// 2 components vector
	class CVec2
	{
	public:
		CVec2()	{}
		CVec2(TReal _x, TReal _y): x(_x), y(_y)	{}
		TReal	x;
		TReal	y;

		CVec2	operator * (TReal _k) const { return CVec2(x*_k, y*_k);}

		// Useful vectors
		static const CVec2 zero;
	};

	//--------------------------------------------------------------------------
	// 3 components vector
	class CVec3
	{
	public:
		// Constructors
		CVec3()	{}
		CVec3(TReal _x, TReal _y, TReal _z): x(_x), y(_y), z(_z) {}

		// Mathematical operators
		CVec3 operator + (const CVec3& _v) const
		{
			return CVec3(x+_v.x,y+_v.y,z+_v.z);
		}
		CVec3 operator - (const CVec3& _v) const
		{
			return CVec3(x-_v.x,y-_v.y,z-_v.z);
		}
		CVec3 operator * (const TReal k) const { return CVec3(x*k, y*k, z*k); }
		CVec3 operator ^ (const CVec3& b) const 
		{
			return CVec3(
				y*b.z - z*b.y,
				z*b.x - x*b.z,
				x*b.y - y*b.x);
		}

		// Common vectors
		static const CVec3 zero;

		// Components
		TReal x;
		TReal y;
		TReal z;
	};

}	//namespace rev

#endif // _REV_REVCORE_MATH_VECTOR_H_
