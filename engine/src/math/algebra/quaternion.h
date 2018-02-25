//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Ag�era Tortosa a.k.a. (Technik)
// on September 25th, 2011
//----------------------------------------------------------------------------------------------------------------------
// Linear algebra's quaternion classes

#ifndef _REV_MATH_ALGEBRA_QUATERION_H_
#define _REV_MATH_ALGEBRA_QUATERION_H_

// Standard headers
#include <cmath>

// Engine headers
#include "matrix.h"
#include "vector.h"
#include "../numericTraits.h"

namespace rev { namespace math {

	template<typename T>
	class UnitQuaternion
	{
	public:
		// Constructors
		UnitQuaternion	()	{}
		UnitQuaternion	(T _x, T _y, T _z, T _w):
			m({_x, _y, _z, _w})
		{}
		template<typename S_>
		UnitQuaternion	(const MatrixBase<3,3,S_>& _matrix);
		static UnitQuaternion	identity();

		// Accessors
		const T& x() const { return m[0]; }
		const T& y() const { return m[1]; }
		const T& z() const { return m[2]; }
		const T& w() const { return m[3]; }

		// Useful constructors
		/// \param axis is assumed to be normalized
		UnitQuaternion fromAxisAngle(const Vector3<T>& axis, T _radians)
		{
			auto half_sin = sin(_radians); // Using sine(theta/2) instead of cosine preserves the sign.
			m.block<3,1>(0,0) = axis*half_sin;
			w() = std::sqrt(1-half_sin*half_sin);
		}

		UnitQuaternion fromUnitVectors(const Vector3<T>& u, const Vector3<T>& v)
		{
			// TODO: This can be made faster by precomputing sqrt(2)
			auto m = sqrt(2 + 2 * u.dot(v));
			Vector3<T> w = (1 / m) * u.cross(v);
			return UnitQuaternion(w.x(), w.y(), w.z(), m*0.5f);
		}

		UnitQuaternion from2Vectors(const Vector3<T>& u, const Vector3<T>& v)
		{
			Vector3<T> w = u.cross(v);
			Quaternion q = quat(dot(u, v), w.x, w.y, w.z);
			q.w += length(q);
			return q.normalized();
		}

		UnitQuaternion conjugate() const
		{
			return Quaternion(-x(), -y(), -z(), w());
		}

		// Other operations
		operator Mat33f<T>			() const; // Rotation matrix
		UnitQuaternion	operator *	(const UnitQuaternion& _q) const;
		Vector3<T>		rotate		(const Vector3<T>& _v) const;
		T				norm		() const { return m.norm(); }

	private:
		T& x() { return m[0]; }
		T& y() { return m[1]; }
		T& z() { return m[2]; }
		T& w() { return m[3]; }

		UnitQuaternion(const Vector4<T> raw) : m(raw) {}
		UnitQuaternion normalized() const { return UnitQuaternion(m.normalized()); }

		Vector4<T> m;
	};

	//------------------------------------------------------------------------------------------------------------------
	// Useful typedefs
	typedef UnitQuaternion<float>	Quatf;
	typedef UnitQuaternion<double>	Quatd;

	//------------------------------------------------------------------------------------------------------------------
	// Inline implementation
	//------------------------------------------------------------------------------------------------------------------
	template<class N_>
	template<typename S_>
	inline UnitQuaternion<N_>::UnitQuaternion(const MatrixBase<3,3,S_> & _m)
	{
		//static_assert(typename S_::Element == N_, "Inconsistent number type Quaternion-Matrix");
		N_ tr = _m(0,0) + _m(1,1) + _m(2,2);

		if (tr > 0) {
			N_ r = sqrt(1 + tr);
			N_ inv2r = 0.5f / r;
			w() = 0.5f * r;
			x() = (_m(2,1) - _m(1,2)) * inv2r;
			y() = (_m(0,2) - _m(2,0)) * inv2r;
			z() = (_m(1,0) - _m(0,1)) * inv2r;
			return;
		}
		// Find the largest diagonal element of _m
		if(_m(0,0) > _m(1,1)) {
			if(_m(0,0) > _m(2,2)) { // _m00 is the largest diagonal element
				N_ r = sqrt( 1 + _m(0,0) - _m(1,1) - _m(2,2) );
				N_ inv2r = 0.5f / r;
				w() = (_m(2,1) - _m(1,2)) * inv2r;
				x() = 0.5f * r;
				y() = (_m(0,1) + _m(1,0)) * inv2r;
				z() = (_m(2,0) + _m(0,2)) * inv2r;
			} else { // _m22 is the largest diagonal element
				N_ r = sqrt( 1 + _m(2,2) - _m(0,0) - _m(1,1) );
				N_ inv2r = 0.5f / r;
				w() = (_m(1,0) - _m(0,1)) * inv2r;
				z() = 0.5f * r;
				x() = (_m(2,0) + _m(0,2)) * inv2r;
				y() = (_m(1,2) + _m(2,1)) * inv2r;
			}
		} else {
			if(_m(1,1) > _m(2,2)) { // _m11 is the largest diagonal element
				N_ r = sqrt( 1 + _m(1,1) - _m(2,2) - _m(0,0) );
				N_ inv2r = 0.5f / r;
				w() = (_m(0,1) - _m(1,0)) * inv2r;
				y() = 0.5f * r;
				z() = (_m(1,2) + _m(2,1)) * inv2r;
				x() = (_m(0,1) + _m(1,0)) * inv2r;
			} else { // _m22 is the largest diagonal element
				N_ r = sqrt( 1 + _m(2,2) - _m(0,0) - _m(1,1) );
				N_ inv2r = 0.5f / r;
				w() = (_m(1,0) - _m(0,1)) * inv2r;
				z() = 0.5f * r;
				x() = (_m(2,0) + _m(0,2)) * inv2r;
				y() = (_m(1,2) + _m(2,1)) * inv2r;
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class N_>
	inline UnitQuaternion<N_> UnitQuaternion<N_>::operator*(const UnitQuaternion<N_>& _q) const
	{
		return Quaternion<N_>(
			w()*_q.x() + x()*_q.w() + y()*_q.z() - z()*_q.y(),	// x
			w()*_q.y() - x()*_q.z() + y()*_q.w() + z()*_q.x(),	// y
			w()*_q.z() + x()*_q.y() - y()*_q.x() + z()*_q.w(),	// z
			w()*_q.w() - x()*_q.x() - y()*_q.y() - z()*_q.z()	// w
			);
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class N_>
	inline Vector3<N_> UnitQuaternion<N_>::rotate(const Vector3<N_>& _v) const
	{
		auto a2 = w()*w();
		auto b2 = x()*x();
		auto c2 = y()*y();
		auto d2 = z()*z();
		auto ab = w()*x();
		auto ac = w()*y();
		auto ad = w()*z();
		auto bc = x()*y();
		auto bd = x()*z();
		auto cd = y()*z();
		return Vector3<N_>(
			(a2+b2-c2-d2)*_v.x()+2*((bc-ad)*_v.y()+(bd+ac)*_v.z()),	// x
			(a2-b2+c2-d2)*_v.y()+2*((cd-ab)*_v.z()+(bc+ad)*_v.x()),	// y
			(a2-b2-c2+d2)*_v.z()+2*((bd-ac)*_v.x()+(cd+ab)*_v.y())	// z
			);
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class N_>
	inline UnitQuaternion<N_> UnitQuaternion<N_>::identity()
	{
		return Quaternion<N_>(0,0,0,1);
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class N_>
	inline UnitQuaternion<N_>::operator Matrix33<N_>() const
	{
		N_ a2 = w()*w();
		N_ b2 = x()*x();
		N_ c2 = y()*y();
		N_ d2 = z()*z();
		N_ ab = 2 * w()*x();
		N_ ac = 2 * w()*y();
		N_ ad = 2 * w()*z();
		N_ bc = 2 * x()*y();
		N_ bd = 2 * x()*z();
		N_ cd = 2 * y()*z();
		Matrix33<N_> mat;
		mat(0,0) = a2 + b2 - c2 - d2;
		mat(0,1) = bc - ad;
		mat(0,2) = bd + ac;
		mat(1,0) = bc + ad;
		mat(1,1) = a2 - b2 + c2 - d2;
		mat(1,2) = cd - ab;
		mat(2,0) = bd - ac;
		mat(2,1) = cd + ab;
		mat(2,2) = a2 - b2 - c2 + d2;
		return mat;
	}

}	// namespace math
}	// namespace rev

#endif // _REV_REVCORE_MATH_QUATERION_H_
