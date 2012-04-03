////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution engine
// Created by Carmelo J. Fern�ndez-Ag�era Tortosa (a.k.a. Technik)
// on March 30th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Script variant

#ifndef _REV_CORE_SCRIPT_CVARIANT_H_
#define _REV_CORE_SCRIPT_CVARIANT_H_

#include <vector.h>

namespace rev
{
	class CVariant
	{
	public:
		enum	EDataType
		{
			eNill = 0,
			eInteger,
			eReal,
			eBool,
			eList,
			eDictionary
		};
	public:
		// ---- Construction, copy and deletion -----
		// Basic constructors
		CVariant			();
		CVariant			(const CVariant&);

		// Typed constructors
		CVariant			(int);
		CVariant			(double);
		CVariant			(bool);

		// Assignment
		CVariant& operator=		(const CVariant&);
		CVariant& operator=		(int);
		CVariant& operator=		(double);
		CVariant& operator=		(bool);
		void		setNill		();
		void		append		(const CVariant& _x);

		// Destruction
		~CVariant			() {}
		
		// Accessors
		int			asInt		() const	{ return mData.i; }
		double		asDouble	() const	{ return mData.d; }
		bool		asBool		() const	{ return mData.b; }
		CVariant&	operator[]	(unsigned _idx);

		EDataType	type		() const	{ return mType;	  }
	public:
		// Comparison
		bool	operator==	(const CVariant&) const;
		bool	operator==	(int) const;
		bool	operator==	(double) const;
		bool	operator==	(bool) const;
		bool	operator!=	(const CVariant&) const;
		bool	operator!=	(int) const;
		bool	operator!=	(double) const;
		bool	operator!=	(bool) const;

	private:
		union UDataT
		{
			int		i;	// Int
			double	d;	// Real
			bool	b;	// Bool
		};

	private:
		EDataType				mType;
		UDataT					mData;
		rtl::vector<CVariant>	mList;
	};
}	// namespace rev

#endif // _REV_CORE_SCRIPT_CVARIANT_H_
