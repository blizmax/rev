////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, types
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on August 22nd, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// types

#ifndef _REV_REVCORE_TYPES_H_
#define _REV_REVCORE_TYPES_H_

// Standard headers
#if defined(_linux) || defined(ANDROID) || defined (WIN32)
#include <string>
#endif // _linux || ANDROID || WIN32

namespace rev
{

#if defined(_linux) || defined(ANDROID) || defined (WIN32)

using std::string;
typedef		float		TReal;

#endif // _linux || ANDROID || WIN32

}	// namespace rev

#endif // _REV_REVCORE_TYPES_H_
