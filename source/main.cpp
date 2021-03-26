#include <GarrysMod/Lua/Interface.h>

#include "basic_server.hpp"

#include <lrdb/command_stream/socket.hpp>

namespace rdb
{
	typedef basic_server<lrdb::command_stream_socket> lrdb_server;

	static int32_t metatype = GarrysMod::Lua::Type::None;
	static const int16_t default_port = 21111;

	LUA_FUNCTION_STATIC( activate )
	{
		lrdb_server **server = LUA->GetUserType<lrdb_server *>( lua_upvalueindex( 1 ), metatype );
		if( *server != nullptr )
		{
			delete *server;
			*server = nullptr;
		}

		if( LUA->IsType( 1, GarrysMod::Lua::Type::Number ) )
			*server = new lrdb_server( static_cast<int16_t>( LUA->GetNumber( 1 ) ) );
		else
			*server = new lrdb_server( default_port );

		( *server )->reset( LUA->GetState( ) );
		return 0;
	}

	LUA_FUNCTION_STATIC( deactivate )
	{
		lrdb_server **server = LUA->GetUserType<lrdb_server *>( lua_upvalueindex( 1 ), metatype );
		if( *server != nullptr )
		{
			delete *server;
			*server = nullptr;
		}

		return 0;
	}

	LUA_FUNCTION_STATIC( destruct )
	{
		lrdb_server **server = LUA->GetUserType<lrdb_server *>( 1, metatype );
		if( *server != nullptr )
		{
			delete *server;
			*server = nullptr;
		}

		return 0;
	}

	static int32_t Initialize( GarrysMod::Lua::ILuaBase *LUA )
	{
		metatype = LUA->CreateMetaTable( "rdb" );

		LUA->PushCFunction( destruct );
		LUA->SetField( -2, "__gc" );

		lrdb_server **server = LUA->NewUserType<lrdb_server *>( metatype );
		*server = nullptr;

		LUA->Push( -2 ); // push metatable to the stack top
		LUA->SetMetaTable( -2 ); // pop reference on stack top and set it as metatable of userdata
		LUA->Remove( -2 ); // remove older metatable reference on stack

		LUA->CreateTable( );

		LUA->PushString( "rdb 1.1.0" );
		LUA->SetField( -2, "Version" );

		// version num follows LuaJIT style, xxyyzz
		LUA->PushNumber( 10100 );
		LUA->SetField( -2, "VersionNum" );

		LUA->Push( -2 ); // push userdata to stack stop
		LUA->PushCClosure( activate, 1 );
		LUA->SetField( -2, "activate" );

		LUA->Push( -2 ); // push userdata to stack stop
		LUA->PushCClosure( deactivate, 1 );
		LUA->SetField( -2, "deactivate" );

		LUA->Push( -1 );
		LUA->SetField( GarrysMod::Lua::INDEX_GLOBAL, "rdb" );

		return 1;
	}

	static int32_t Deinitialize( GarrysMod::Lua::ILuaBase *LUA )
	{
		LUA->PushNil( );
		LUA->SetField( GarrysMod::Lua::INDEX_GLOBAL, "rdb" );
		return 0;
	}
}

GMOD_MODULE_OPEN( )
{
	return rdb::Initialize( LUA );
}

GMOD_MODULE_CLOSE( )
{
	return rdb::Deinitialize( LUA );
}
