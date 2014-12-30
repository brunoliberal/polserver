/*
History
=======
2005/01/24 Shinigami: added packet 0xd9 (Spy on Client 2) to exceptions
2005/08/29 Shinigami: SPY_ON_CLIENT renamed to CLIENT_INFO
2007/07/08 Shinigami: added PKTIN_E4_ID to login_exceptions (used by UO:KR)
2009/09/06 Turley:    Added 0xE1 packet
2011/11/11 Tomi:	  Added 0xF8 packet

Notes
=======

*/

#include "msgfiltr.h"

#include "../pktboth.h"
#include "../pktin.h"
#include "../globals/uvars.h"

#include <cstring>

namespace Pol {
  namespace Core {
	MessageTypeFilter::MessageTypeFilter( MessageDefault def, int* exceptions )
	{
	  unsigned char normal;
	  unsigned char abnormal;
	  if ( def == NormallyDisabled )
		normal = 0;
	  else
		normal = 1;
	  abnormal = !normal;

	  memset( msgtype_allowed, normal, sizeof msgtype_allowed );

	  while ( *exceptions != -1 )
	  {
		msgtype_allowed[*exceptions & 0xFF] = abnormal;
		++exceptions;
	  }
	}

	void MessageTypeFilter::createMessageFilter()
	{
	  int login_exceptions[] =
	  {
		PKTIN_00_ID,
		PKTIN_5D_ID,
		PKTBI_73_ID,
		PKTIN_80_ID,
		PKTIN_83_ID,
		PKTIN_91_ID,
		PKTIN_A0_ID,
		PKTIN_A4_ID,
		PKTIN_D9_ID,
		PKTIN_E1_ID, // Used by UO:KR & SA
		PKTIN_E4_ID, // Used by UO:KR
		PKTIN_8D_ID,
		PKTIN_F8_ID,
		-1
	  };
	  gamestate.login_filter = std::unique_ptr<MessageTypeFilter>(new MessageTypeFilter( MessageTypeFilter::NormallyDisabled, login_exceptions ));

	  int game_exceptions[] =
	  {
		PKTIN_00_ID,
		PKTIN_5D_ID,
		PKTIN_80_ID,
		PKTIN_83_ID,
		PKTIN_91_ID,
		PKTIN_A0_ID,
		PKTIN_A4_ID,
		PKTIN_D9_ID,
		PKTIN_8D_ID,
		PKTIN_F8_ID,
		-1
	  };
	  gamestate.game_filter = std::unique_ptr<MessageTypeFilter>(new MessageTypeFilter( MessageTypeFilter::NormallyEnabled, game_exceptions ));

	  int no_exceptions[] =
	  {
		-1
	  };
	  gamestate.disconnected_filter = std::unique_ptr<MessageTypeFilter>(new MessageTypeFilter( MessageTypeFilter::NormallyDisabled, no_exceptions ));
	}

	
  }
}