PROJECT_GENERATOR_VERSION = 2

newoption({
	trigger = "gmcommon",
	description = "Sets the path to the garrysmod_common (https://github.com/danielga/garrysmod_common) directory",
	value = "path to garrysmod_common directory"
})

include(assert(_OPTIONS.gmcommon or os.getenv("GARRYSMOD_COMMON"),
	"you didn't provide a path to your garrysmod_common (https://github.com/danielga/garrysmod_common) directory"))

CreateWorkspace({name = "rdb"})
	filter("system:windows")
		defines("_WIN32_WINNT=0x0601")

	CreateProject({serverside = true})
		sysincludedirs({
			"LRDB/include",
			"asio/asio/include",
			"picojson"
		})
		IncludeLuaShared()
		IncludeHelpersExtended()
		IncludeSDKCommon()
		IncludeSDKTier0()
		IncludeSDKTier1()

	CreateProject({serverside = false})
		sysincludedirs({
			"LRDB/include",
			"asio/asio/include",
			"picojson"
		})
		IncludeLuaShared()
		IncludeHelpersExtended()
		IncludeSDKCommon()
		IncludeSDKTier0()
		IncludeSDKTier1()
