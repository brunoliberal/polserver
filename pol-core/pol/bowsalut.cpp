/*
History
=======
2005/11/26 Shinigami: changed "strcmp" into "stricmp" to suppress Script Errors
2009/07/27 MuadDib:   Pakcet struct refactoring

Notes
=======

*/

#include "../clib/stl_inc.h"

#include "../clib/cfgelem.h"
#include "../clib/cfgfile.h"
#include "../clib/endian.h"
#include "../clib/fileutil.h"
#include "../clib/strutil.h"

#include "mobile/charactr.h"
#include "network/client.h"
#include "network/packets.h"
#include "extcmd.h"
#include "pktin.h"
#include "pktdef.h"
#include "uobject.h"
#include "uvars.h"
#include "ufunc.h"
#include "sockio.h"

#include "action.h"
namespace Pol {
  namespace Core {
	UACTION mount_action_xlate[ACTION__HIGHEST + 1];

	UACTION str_to_action( Clib::ConfigElem& elem, const string& str )
	{
	  unsigned short tmp = static_cast<unsigned short>( strtoul( str.c_str(), NULL, 0 ) );

	  if ( UACTION_IS_VALID( tmp ) )
	  {
		return static_cast<UACTION>( tmp );
	  }
	  else
	  {
		elem.throw_error( "Animation value of " + str + " is out of range" );
		return ACTION__LOWEST;
	  }
	}

	void load_anim_xlate_cfg( bool reload )
	{
	  memset( mount_action_xlate, 0, sizeof mount_action_xlate );

	  if ( Clib::FileExists( "config/animxlate.cfg" ) )
	  {
		Clib::ConfigFile cf( "config/animxlate.cfg", "OnMount" );
		Clib::ConfigElem elem;

		if ( cf.read( elem ) )
		{
		  string from_str, to_str;
		  while ( elem.remove_first_prop( &from_str, &to_str ) )
		  {
			UACTION from = str_to_action( elem, from_str );
			UACTION to = str_to_action( elem, to_str );
			mount_action_xlate[from] = to;
		  }
		}
	  }
	}

	void send_action_to_inrange( const Mobile::Character* obj, UACTION action,
								 unsigned short framecount /*=0x05*/,
								 unsigned short repeatcount /*=0x01*/,
								 PKTOUT_6E::DIRECTION_FLAG backward /*=PKTOUT_6E::FORWARD*/,
								 PKTOUT_6E::REPEAT_FLAG repeatflag /*=PKTOUT_6E::NOREPEAT*/,
								 unsigned char delay /*=0x01*/ )
	{
	  if ( obj->on_mount() )
	  {
		if ( action < ACTION_RIDINGHORSE1 || action > ACTION_RIDINGHORSE7 )
		{
		  UACTION new_action = mount_action_xlate[action];
		  if ( new_action == 0 )
			return;
		  action = new_action;
		}
	  }
	  Network::PktHelper::PacketOut<Network::PktOut_6E> msg;
	  msg->Write<u32>( obj->serial_ext );
	  msg->WriteFlipped<u16>( static_cast<u16>( action ) );
	  msg->WriteFlipped<u16>( framecount );
	  msg->WriteFlipped<u16>( repeatcount );
	  msg->Write<u8>( static_cast<u8>( backward ) );
	  msg->Write<u8>( static_cast<u8>( repeatflag ) );
	  msg->Write<u8>( delay );
	  transmit_to_inrange( obj, &msg->buffer, msg->offset, false, false );
	}

	void handle_action( Network::Client *client, PKTIN_12 *cmd )
	{
	  if ( stricmp( (const char *)cmd->data, "bow" ) == 0 )
		send_action_to_inrange( client->chr, ACTION_BOW );
	  else if ( stricmp( (const char *)cmd->data, "salute" ) == 0 )
		send_action_to_inrange( client->chr, ACTION_SALUTE );
	}

	ExtendedMessageHandler action_handler( EXTMSGID_ACTION, handle_action );
  }
}