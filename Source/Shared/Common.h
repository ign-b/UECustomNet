#pragma once

#define NETWORK_MODULE_VERSION 0x3

#define SAFE_DELETE(memory) if(memory) { delete (memory); } (memory) = nullptr
#define SAFE_FREE(memory) if(memory) { free(memory); } (memory) = nullptr

/*#ifdef WIN32
	#ifdef CORE_EXPORTS
		#define UNET_API __declspec(dllexport)
	#else
		#define UNET_API __declspec(dllimport)
	#endif
#else*/
	#define UNET_API
/*#endif*/

namespace un {
    using Size = unsigned int;
};
