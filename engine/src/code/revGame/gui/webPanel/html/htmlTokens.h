////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fern�ndez-Ag�era Tortosa (a.k.a. Technik)
// Created on May 14th, 2012
////////////////////////////////////////////////////////////////////////////////
// html lexer

#ifndef _REV_GAME_GUI_WEBPANEL_HTML_HTMLTOKENS_H_
#define _REV_GAME_GUI_WEBPANEL_HTML_HTMLTOKENS_H_

namespace rev { namespace game
{
	enum EHtmlToken
	{
		eOpenEndTag,
		eOpenTag,
		eCloseTag,
		eAssignement,
		ePercentage,
		eSpace,
		eInt,
		eFloat,
		eId,
		eColor,
		eWord,
		eString,
	};
	/*struct CHtmlToken
	{
		enum EType
		{
			eSpace,
			eWord,
			eOpenTag,
			eOpenEndTag,
			eCloseTag,
			eAssignment,
			eString,
			eSignedInt,
			eFloat,
			ePercentage,
			eColor
		};

		EType type;
		unsigned line;
		unsigned pos;
		const char * text;
		
		void clean() { if (0 != text) {delete[] text; text = 0; } }
	};*/
}	// namespace game
}	// namespace rev

#endif // _REV_GAME_GUI_WEBPANEL_HTML_HTMLTOKENS_H_
