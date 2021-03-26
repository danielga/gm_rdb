#include "console_adapter_spew.hpp"

#if !__has_include(<tier0/logging.h>)

#include <GarrysMod/InterfacePointers.hpp>

#include <cdll_int.h>
#include <eiface.h>

SpewOutputFunc_t console_adapter::s_original_spew = nullptr;
std::mutex console_adapter::s_mutex;
std::condition_variable console_adapter::s_condition;
std::vector<json::value> console_adapter::s_messages;

console_adapter::console_adapter( ) :
	m_thread_stop( true )
{

#if IS_SERVERSIDE

	m_engine_server = InterfacePointers::VEngineServer( );

#else

	m_engine_client = InterfacePointers::VEngineClient( );

#endif

}

console_adapter::~console_adapter( )
{
	set_callback( nullptr );
}

void console_adapter::set_callback( const std::function<void( const json::value &message )> &callback )
{
	std::unique_lock<std::mutex> lock( s_mutex );

	m_callback = callback;

	if( callback )
		start( lock );
	else
		stop( lock );
}

void console_adapter::run_command( const std::string &command )
{

#if IS_SERVERSIDE

	m_engine_server->ServerCommand( command.c_str( ) );

#else

	m_engine_client->ClientCmd_Unrestricted( command.c_str( ) );

#endif

}

void console_adapter::start( [[maybe_unused]] std::unique_lock<std::mutex> &lock )
{
	if( !m_thread_stop )
		return;

	s_original_spew = GetSpewOutputFunc( );
	SpewOutputFunc( &console_adapter::Log );

	m_thread_stop = false;
	m_thread = std::thread( &console_adapter::queue_dispatcher, this );
}

void console_adapter::stop( std::unique_lock<std::mutex> &lock )
{
	if( m_thread_stop )
		return;

	SpewOutputFunc( s_original_spew );
	s_original_spew = nullptr;

	s_messages.clear( );

	m_thread_stop = true;
	lock.unlock( );
	s_condition.notify_all( );
	m_thread.join( );
}

void console_adapter::queue_dispatcher( )
{
	while( !m_thread_stop )
	{
		std::vector<json::value> messages;
		{
			std::unique_lock<std::mutex> lock( s_mutex );
			s_condition.wait( lock, [this] { return !s_messages.empty( ) || m_thread_stop; } );
			if( m_thread_stop )
				break;

			std::swap( messages, s_messages );
		}

		for( const json::value &message : messages )
			m_callback( message );
	}
}

SpewRetval_t console_adapter::Log( SpewType_t spewType, const tchar *pMsg )
{
	std::unique_lock<std::mutex> lock( s_mutex );

	const Color &color = *GetSpewOutputColor( );

	json::object jcolor;
	jcolor["r"] = json::value( static_cast<double>( color.r( ) ) );
	jcolor["g"] = json::value( static_cast<double>( color.g( ) ) );
	jcolor["b"] = json::value( static_cast<double>( color.b( ) ) );
	jcolor["a"] = json::value( static_cast<double>( color.a( ) ) );

	json::object param;
	param["group"] = json::value( GetSpewOutputGroup( ) );
	param["severity"] = json::value( static_cast<double>( GetSpewOutputLevel( ) ) );
	param["color"] = json::value( jcolor );
	param["message"] = json::value( pMsg );

	s_messages.emplace_back( std::move( param ) );

	lock.unlock( );
	s_condition.notify_all( );

	return s_original_spew( spewType, pMsg );
}

#endif
