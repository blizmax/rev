//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Ag�era Tortosa (a.k.a. Technik)
// 2014/April/03
//----------------------------------------------------------------------------------------------------------------------
// Generic mathematical vector
#ifndef _REV_MATH_ALGEBRA_VECTOR_INL_
#define _REV_MATH_ALGEBRA_VECTOR_INL_

#include "vector.h"

namespace rev {
	namespace math {
		
		//--------------------------------------------------------------------------------------------------------------
		template<typename T_>
		bool Vector<T_, 2>::operator==(const Vector& _v) const {
			return _v.x == x && _v.y == y;
		}

		//--------------------------------------------------------------------------------------------------------------
		template<typename T_>
		bool Vector<T_, 2>::operator!=(const Vector& _v) const {
			return _v.x != x || _v.y != y;
		}

		//--------------------------------------------------------------------------------------------------------------
		template<typename T_>
		Vector<T_, 3>& Vector<T_, 3>::operator=(const Vector& _v) {
			x = _v.x;
			y = _v.y;
			z = _v.z;
			return *this;
		}

		//------------------------------------------------------------------------------------------------------------------
		template<class T_>
		bool Vector<T_, 3>::operator==(const Vector& _v) const
		{
			return (x == _v.x) && (y == _v.y) && (z == _v.z);
		}

		//------------------------------------------------------------------------------------------------------------------
		template<class T_>
		bool Vector<T_, 3>::operator!=(const Vector& _v) const
		{
			return (x != _v.x) || (y != _v.y) || (z != _v.z);
		}

		//------------------------------------------------------------------------------------------------------------------
		template<class T_>
		Vector<T_, 3> Vector<T_, 3>::operator+ (const Vector& _v) const
		{
			return Vector<T_, 3>(x + _v.x, y + _v.y, z + _v.z);
		}

		//------------------------------------------------------------------------------------------------------------------
		template<class T_>
		Vector<T_, 3>& Vector<T_, 3>::operator+= (const Vector& _v)
		{
			x += _v.x;
			y += _v.y;
			z += _v.z;
			return *this;
		}

		//------------------------------------------------------------------------------------------------------------------
		template<class T_>
		Vector<T_, 3>	Vector<T_, 3>::operator- (const Vector<T_, 3>& _v) const
		{
			return Vector<T_, 3>(x - _v.x, y - _v.y, z - _v.z);
		}

		//------------------------------------------------------------------------------------------------------------------
		template<class T_>
		Vector<T_, 3>& Vector<T_, 3>::operator-= (const Vector<T_, 3>& _v)
		{
			x -= _v.x;
			y -= _v.y;
			z -= _v.z;
			return *this;
		}

		//------------------------------------------------------------------------------------------------------------------
		template<class T_>
		Vector<T_, 3> Vector<T_, 3>::operator- () const
		{
			return Vector<T_, 3>(-x, -y, -z);
		}

		//----------------------------------------------------------------------------------------------------------------
		template<class T_>
		Vector<T_, 3> Vector<T_, 3>::operator*(const T_ _k) const
		{
			return Vector<T_, 3>(x * _k, y * _k, z * _k);
		}

		//----------------------------------------------------------------------------------------------------------------
		template<class T_>
		Vector<T_, 3>& Vector<T_, 3>::operator*=(const T_ _k)
		{
			x *= _k;
			y *= _k;
			z *= _k;
			return *this;
		}

		//----------------------------------------------------------------------------------------------------------------
		template<class T_>
		Vector<T_, 3> Vector<T_, 3>::operator/(const T_ _k) const
		{
			T_ inv = 1.f / _k;
			return Vector<T_, 3>(x * inv, y * inv, z * inv);
		}

		//----------------------------------------------------------------------------------------------------------------
		template<class T_>
		Vector<T_, 3>& Vector<T_, 3>::operator/=(const T_ _k)
		{
			T_ inv = 1.f / _k;
			x *= inv;
			y *= inv;
			z *= inv;
			return *this;
		}

		//----------------------------------------------------------------------------------------------------------------
		template<class T_>
		T_ Vector<T_, 3>::operator*(const Vector<T_, 3>& _b) const
		{
			return x * _b.x + y * _b.y + z * _b.z;
		}

		//----------------------------------------------------------------------------------------------------------------
		template<class T_>
		Vector<T_, 3> Vector<T_, 3>::operator^(const Vector<T_, 3>& _b) const
		{
			return Vector<T_, 3>(y*_b.z - z*_b.y,
				z*_b.x - x*_b.z,
				x*_b.y - y*_b.x);
		}

		//----------------------------------------------------------------------------------------------------------------
		template<class T_>
		T_ Vector<T_, 3>::norm() const
		{
			return sqrt(x*x + y*y + z*z);
		}

		//----------------------------------------------------------------------------------------------------------------
		template<class T_>
		T_ Vector<T_, 3>::sqNorm() const
		{
			return x*x + y*y + z*z;
		}

		//----------------------------------------------------------------------------------------------------------------
		template<class T_>
		Vector<T_, 3> Vector<T_, 3>::normalized() const
		{
			T_ factor = 1.f / norm();
			return Vector<T_, 3>(x*factor, y*factor, z*factor);
		}

		//----------------------------------------------------------------------------------------------------------------
		template<class T_>
		Vector<T_, 3>& Vector<T_, 3>::normalize()
		{
			T_ factor = 1.f / norm();
			x *= factor;
			y *= factor;
			z *= factor;
			return *this;
		}

	}	// namespace math
}	// namespace rev

#endif // _REV_MATH_ALGEBRA_VECTOR_INL_