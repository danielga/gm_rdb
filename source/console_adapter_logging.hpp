#pragma once

#if __has_include(<tier0/logging.h>)

#include <mutex>
#include <thread>
#include <atomic>
#include <functional>
#include <condition_variable>

#include <picojson.h>

#include <tier0/logging.h>

#if IS_SERVERSIDE

class IVEngineServer;

#else

class IVEngineClient;

#endif

struct LoggingContext_t;

namespace json
{
	using namespace ::picojson;
}

class console_adapter : private ILoggingListener
{
public:
	console_adapter( );

	~console_adapter( );

	void set_callback( const std::function<void( const json::value &message )> &callback );

	void run_command( const std::string &command );

private:
	void start( std::unique_lock<std::mutex> &lock );

	void stop( std::unique_lock<std::mutex> &lock );

	void queue_dispatcher( );

	virtual void Log( const LoggingContext_t *pContext, const tchar *pMessage ) override;

	std::mutex m_mutex;
	std::condition_variable m_condition;
	std::thread m_thread;
	std::atomic_bool m_thread_stop;
	std::vector<json::value> m_messages;
	std::function<void( const json::value &message )> m_callback;

#if IS_SERVERSIDE

	IVEngineServer *m_engine_server;

#else

	IVEngineClient *m_engine_client;

#endif

};

#endif
