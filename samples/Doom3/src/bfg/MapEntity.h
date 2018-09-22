//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Ag�era Tortosa
//----------------------------------------------------------------------------------------------------------------------
// A doom 3 map loader
#pragma once

namespace id {

	class Lexer;

	class MapEntity
	{
	public:
		
		static MapEntity *	Parse( Lexer &src );

	protected:
		//idDict					epairs;
		//idList<idMapPrimitive*, TAG_IDLIB_LIST_MAP>	primitives;
	};

}