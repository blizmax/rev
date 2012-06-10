////////////////////////////////////////////////////////////////////////////////
// Revolution Engine game client
// by Carmelo J. Fern�ndez-Ag�era Tortosa (a.k.a. Technik)
// Created on June 10th, 2012
////////////////////////////////////////////////////////////////////////////////
// Revolution Engine Game client

#include <revGame/gameClient/gameClient.h>

class CRevGameClient : public rev::game::SGameClient
{
public:
	CRevGameClient();
	~CRevGameClient();

	void init	();
	bool update	();	///< Update the game
};