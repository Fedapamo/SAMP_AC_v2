#include "VersionHelper.h"
#include "../Shared/SelfUpdater/CSelfUpdater.h"

#include "Utility.h"

namespace VersionHelper
{
#ifdef WIN32
	const char* AC_UPDATE_URL = "http://samp-ac.com/API/Updater/Server_Windows.html";
#else
	const char* AC_UPDATE_URL = "http://samp-ac.com/API/Updater/Server_Linux.html";
#endif

	char HTTP_REQUEST_USER_AGENT[32];
	char AC_SERVER_VERSION_STRING[16];
	CSelfUpdater::stVersion AC_SERVER_VERSION{ 2, 83, 0 };
	// Incrementing the version will force the server to update when they run it next.
	// Only MAJOR and/or MINOR version numbers determine if server is compatible with client.
	// If only PATCH version number is different, server and client are compatible.

	bool IsClientCompatible(CSelfUpdater::stVersion clientVersion)
	{
		if (AC_SERVER_VERSION.major != clientVersion.major) return 0;
		if (AC_SERVER_VERSION.minor != clientVersion.minor) return 0;

		return 1;
	}

	void Initialize()
	{
		sprintf(AC_SERVER_VERSION_STRING, "%d.%02d.%d", AC_SERVER_VERSION.major, AC_SERVER_VERSION.minor, AC_SERVER_VERSION.patch);
		sprintf(HTTP_REQUEST_USER_AGENT, "ACSVR_%s", AC_SERVER_VERSION_STRING);
	}

	void CheckForUpdate()
	{
		Utility::Printf("Checking for an update...");

		CSelfUpdater updater = CSelfUpdater(AC_SERVER_VERSION, AC_UPDATE_URL);
		if (!updater.CheckForNewVersion())
		{
			Utility::Printf("ERROR! Couldn't contact the update server or the response was malformed.");
			Utility::Printf("Continue using AC version: v%s.", AC_SERVER_VERSION_STRING);
			return;
		}
		if (updater.m_currentVersion >= updater.m_newVersion)
		{
			Utility::Printf("You are running the latest version.");
			return;
		}
		Utility::Printf("Update required! AC plugin will attempt to update itself. If you'll get any errors, you will have to update it manually or remove it from the server.cfg and adjust your scripts accordingly.");
		Utility::Printf("Downloading version v%d.%02d.%d from %s ...", updater.m_newVersion.major, updater.m_newVersion.minor, updater.m_newVersion.patch, updater.m_fileURL);
		Utility::Printf("Update notes: %s", updater.m_additionalInfo);
		if (!updater.DownloadUpdate())
		{
			Utility::Printf("ERROR! Couldn't download the updated binary.");
			Utility::Printf("Continue using AC version: v%s.", AC_SERVER_VERSION_STRING);
			Utility::Printf("WARNING! Players may not be able to connect to this server anymore unless you manually update the AC server plugin to the latest version.");
			return;
		}
		Utility::Printf("Applying update ...");
		Utility::Printf("WARNING! If update fails at this step, it is most likely that AC plugin will get corrupted or deleted and you will have to update it manually or remove it from the server.cfg and adjust your scripts accordingly.");
		if (!updater.ApplyUpdate())
		{
			Utility::Printf("ERROR! Couldn't apply the updated binary.");
			Utility::Printf("Continue using AC version: v%s.", AC_SERVER_VERSION_STRING);
			Utility::Printf("WARNING! Players may not be able to connect to this server anymore unless you manually update the AC server plugin to the latest version.");
			return;
		}
		Utility::Printf("AC server plugin should have been updated to version v%d.%02d.%d!", updater.m_newVersion.major, updater.m_newVersion.minor, updater.m_newVersion.patch);
		Utility::Printf("Server will now shut down. Please reopen the SA-MP server.");
		
		updater.ExitProcess();
	}
}