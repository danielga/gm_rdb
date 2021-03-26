#include "console_adapter_logging.hpp"

#if __has_include(<tier0/logging.h>)

#include <GarrysMod/InterfacePointers.hpp>

#include <cdll_int.h>
#include <eiface.h>

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
	std::unique_lock<std::mutex> lock( m_mutex );

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

	LoggingSystem_RegisterLoggingListener( this );

	m_thread_stop = false;
	m_thread = std::thread( &console_adapter::queue_dispatcher, this );
}

void console_adapter::stop( std::unique_lock<std::mutex> &lock )
{
	if( m_thread_stop )
		return;

	LoggingSystem_UnregisterLoggingListener( this );

	m_messages.clear( );

	m_thread_stop = true;
	lock.unlock( );
	m_condition.notify_all( );
	m_thread.join( );
}

void console_adapter::queue_dispatcher( )
{
	while( !m_thread_stop )
	{
		std::vector<json::value> messages;
		{
			std::unique_lock<std::mutex> lock( m_mutex );
			m_condition.wait( lock, [this] { return !m_messages.empty( ) || m_thread_stop; } );
			if( m_thread_stop )
				break;

			std::swap( messages, m_messages );
		}

		for( const json::value &message : messages )
			m_callback( message );
	}
}

void console_adapter::Log( const LoggingContext_t *pContext, const tchar *pMessage )
{
	std::unique_lock<std::mutex> lock( m_mutex );
	if( pContext->m_Flags & LCF_DO_NOT_ECHO )
		return;

	Color color = pContext->m_Color;
	if( color == UNSPECIFIED_LOGGING_COLOR )
		color = Color( 255, 255, 255, 255 );

	json::object jcolor;
	jcolor["r"] = json::value( static_cast<double>( color.r( ) ) );
	jcolor["g"] = json::value( static_cast<double>( color.g( ) ) );
	jcolor["b"] = json::value( static_cast<double>( color.b( ) ) );
	jcolor["a"] = json::value( static_cast<double>( color.a( ) ) );

	json::object param;
	param["channel_id"] = json::value( static_cast<double>( pContext->m_ChannelID ) );
	param["severity"] = json::value( static_cast<double>( pContext->m_Severity ) );
	param["color"] = json::value( jcolor );
	param["message"] = json::value( pMessage );

	m_messages.emplace_back( std::move( param ) );

	lock.unlock( );
	m_condition.notify_all( );
}

#endif
