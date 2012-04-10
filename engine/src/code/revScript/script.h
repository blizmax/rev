////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution engine
// Created by Carmelo J. Fern�ndez-Ag�era Tortosa (a.k.a. Technik)
// on March 30th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Script machine

#ifndef _REV_CORE_SCRIPT_CSCRIPT_H_
#define _REV_CORE_SCRIPT_CSCRIPT_H_

#include "syntax.h"
#include "variant.h"

namespace rev { namespace script
{
	class CScript
	{
	public:
		CScript(const char * _filename);
		~CScript() {}

		void run(CVariant& _res);

	private:
		CStatementList	mCode;
	};

}	// namespace script
}	// namespace rev

#endif // _REV_CORE_SCRIPT_CSCRIPT_H_