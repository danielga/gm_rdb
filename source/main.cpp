#include <GarrysMod/Lua/Interface.h>

#include <lrdb/server.hpp>

namespace global
{
	static int32_t metatype = GarrysMod::Lua::Type::None;
	static const int16_t default_port = 21110;

	LUA_FUNCTION_STATIC( lrdb_activate )
	{
		lrdb::server **server = LUA->GetUserType<lrdb::server *>( lua_upvalueindex( 1 ), metatype );
		if( *server != nullptr )
		{
			delete *server;
			*server = nullptr;
		}

		if( LUA->IsType( 1, GarrysMod::Lua::Type::Number ) )
			*server = new lrdb::server( static_cast<int16_t>( LUA->GetNumber( 1 ) ) );
		else
			*server = new lrdb::server( default_port );

		( *server )->reset( LUA->GetState( ) );
		return 0;
	}

	LUA_FUNCTION_STATIC( lrdb_deactivate )
	{
		lrdb::server **server = LUA->GetUserType<lrdb::server *>( lua_upvalueindex( 1 ), metatype );
		if( *server != nullptr )
		{
			delete *server;
			*server = nullptr;
		}

		return 0;
	}

	LUA_FUNCTION_STATIC( lrdb_destruct )
	{
		lrdb::server **server = LUA->GetUserType<lrdb::server *>( 1, metatype );
		if( *server != nullptr )
		{
			delete *server;
			*server = nullptr;
		}

		return 0;
	}

	static int32_t Initialize( GarrysMod::Lua::ILuaBase *LUA )
	{
		metatype = LUA->CreateMetaTable( "lrdb" );

		LUA->PushCFunction( lrdb_destruct );
		LUA->SetField( -2, "__gc" );

		lrdb::server **server = LUA->NewUserType<lrdb::server *>( metatype );
		*server = nullptr;

		LUA->Push( -2 ); // push metatable to the stack top
		LUA->SetMetaTable( -2 ); // pop reference on stack top and set it as metatable of userdata
		LUA->Remove( -2 ); // remove older metatable reference on stack

		LUA->CreateTable( );

		LUA->PushString( "lrdb 1.0.0" );
		LUA->SetField( -2, "Version" );

		// version num follows LuaJIT style, xxyyzz
		LUA->PushNumber( 10000 );
		LUA->SetField( -2, "VersionNum" );

		LUA->Push( -2 ); // push userdata to stack stop
		LUA->PushCClosure( lrdb_activate, 1 );
		LUA->SetField( -2, "activate" );

		LUA->Push( -2 ); // push userdata to stack stop
		LUA->PushCClosure( lrdb_deactivate, 1 );
		LUA->SetField( -2, "deactivate" );

		LUA->Push( -1 );
		LUA->SetField( GarrysMod::Lua::INDEX_GLOBAL, "lrdb" );

		return 1;
	}

	static int32_t Deinitialize( GarrysMod::Lua::ILuaBase *LUA )
	{
		LUA->PushNil( );
		LUA->SetField( GarrysMod::Lua::INDEX_GLOBAL, "lrdb" );
		return 0;
	}
}

GMOD_MODULE_OPEN( )
{
	return global::Initialize( LUA );
}

GMOD_MODULE_CLOSE( )
{
	return global::Deinitialize( LUA );
}
