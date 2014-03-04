//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Ag�era Tortosa a.k.a. (Technik)
// On 2013/Aug/15
//----------------------------------------------------------------------------------------------------------------------
// Angles

namespace rev { namespace math {
	static const float PI = 3.1415927410125732421875f;
	static const double dPI = 3.141592653589793115997963468544185161590576171875;

	template<class T_>
	T_ deg2Rad(T_ _degrees) {
		return T_(_degrees * dPI / 180.0);
	}
	
	template<class T_>
	T_ rad2Deg(T_ _radians) {
		return T_(_radians * 180 / dPI);
	}
}	// namespace math
}	// namespace rev