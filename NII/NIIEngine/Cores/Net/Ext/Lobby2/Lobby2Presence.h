/*
-----------------------------------------------------------------------------
A
     __      _   _   _   ______
    |   \   | | | | | | |  ____)                    _
    | |\ \  | | | | | | | |         ___      ___   (_)   ___
    | | \ \ | | | | | | | |____    / _ \   / ___ \  _   / _ \   ___
    | |  \ \| | | | | | |  ____)  | / \ | | |  | | | | | / \ | / _ )
    | |   \ | | | | | | | |_____  | | | | | |__| | | | | | | | | __/
    |_|    \ _| |_| |_| |_______) |_| |_|  \___| | |_| |_| |_| |___|
                                             __/ |                 
                                            \___/   
                                                
                                                
                                                                 F i l e


Copyright: NIIEngine Team Group

Home page: www.niiengine.com 

Email: niiengine@gmail.com OR niiengine@163.com

Licence: commerce(www.niiengine.com/license)(Three kinds)
------------------------------------------------------------------------------
*/
#ifndef __LOBBY_2_PRESENCE_H
#define __LOBBY_2_PRESENCE_H

#include "RakString.h"

namespace NII
{
namespace NII_NET
{
	/// Lobby2Presence is information about your online status. It is only held in memory, so is lost when you go offline.
	/// Set by calling Client_SetPresence and retrieve with Client_GetPresence
	struct Lobby2Presence
	{
		Lobby2Presence();
		Lobby2Presence(const Lobby2Presence & input);
		Lobby2Presence & operator = (const Lobby2Presence & input);
		~Lobby2Presence();
        void write(NetSerializer * in);
		void read(NetSerializer * out);

		enum Status
		{
			/// Set by the constructor, meaning it was never set. This is the default if a user is online, but SetPresence() was never called.
			UNDEFINED,
			/// Returned by Client_GetPresence() if you query for a user that is not online, whether or not SetPresence was ever called().
			NOT_ONLINE,
			/// Set by the user (you)
			AWAY,
			/// Set by the user (you)
			DO_NOT_DISTURB,
			/// Set by the user (you)
			MINIMIZED,
			/// Set by the user (you)
			TYPING,
			/// Set by the user (you)
			VIEWING_PROFILE,
			/// Set by the user (you)
			EDITING_PROFILE,
			/// Set by the user (you)
			IN_LOBBY,
			/// Set by the user (you)
			IN_ROOM,
			/// Set by the user (you)
			IN_GAME
		} status;

		/// Visibility flag. This is not enforced by the server, so if you want a user's presence to be not visible, just don't display it on the client
		bool isVisible;

		/// Although game name is also present in the titleNameOrID member of Client_Login, this is the visible name returned by presence queries
		/// That is because Client_Login::titleNameOrID member is optional, for example for lobbies that support multiple titles.
		/// Set by the user (you) or leave blank if desired.
		RakString titleNameOrID;

		/// Anything you want
		RakString statusString;
	};
}
}
#endif