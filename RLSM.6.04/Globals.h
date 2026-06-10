#pragma once

namespace rslm {
	namespace client {
		namespace config {

			// Ces variables existent dans Globals.cpp
			// Elles sont partagées avec le reste du programme
			extern char g_serverHost[64];
			extern int g_serverPort;
			extern bool g_enablePersistence;
			extern bool g_hideOnStart;

		}// namespace config
	}// namespace client
}// namespace rslm