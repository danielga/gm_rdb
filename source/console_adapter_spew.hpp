#pragma once

#if !__has_include(<tier0/logging.h>)

#include <mutex>
#include <thread>
#include <atomic>
#include <functional>
#include <condition_variable>

#include <picojson.h>

#include <tier0/dbg.h>

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

// Doesn't support multiple instances.
class console_adapter
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

	static SpewRetval_t Log( SpewType_t spewType, const tchar *pMsg );

	static SpewOutputFunc_t s_original_spew;
	static std::mutex s_mutex;
	static std::condition_variable s_condition;
	static std::vector<json::value> s_messages;

	std::thread m_thread;
	std::atomic_bool m_thread_stop;
	std::function<void( const json::value &message )> m_callback;

#if IS_SERVERSIDE

	IVEngineServer *m_engine_server;

#else

	IVEngineClient *m_engine_client;

#endif

};

#endif
