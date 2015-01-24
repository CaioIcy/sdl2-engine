#include "core/SDL_Systems.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <cstdlib>
#include <string>
#include "util/Logger.h"

namespace {

/**
* @brief Properly closes SDL_Mixer.
*
* Also logs a small about of debug info about the audio.
*/
void CloseMixer() {
	// Query the specifications of the opened audio, to figure out how many times we need to
	// close the audio.
	int frequency = 0;
	int channels = 0;
	Uint16 format = 0;
	const int k_numbers_of_times_opened = Mix_QuerySpec(&frequency, &format, &channels);
	int times_to_close = k_numbers_of_times_opened;

	if(k_numbers_of_times_opened != 0) {
		// Build a string with the format.
		std::string format_text{"Unknown format"};
	    switch(format) {
	        case AUDIO_U8:
	        	format_text = "U8";
	        	break;
	        case AUDIO_S8:
		        format_text = "S8";
	        	break;
	        case AUDIO_U16LSB:
		        format_text = "U16LSB";
	        	break;
	        case AUDIO_S16LSB:
		        format_text = "S16LSB";
	        	break;
	        case AUDIO_U16MSB:
		        format_text = "U16MSB";
	        	break;
	        case AUDIO_S16MSB:
		        format_text = "S16MSB";
	        	break;
	        default:
	        	// Already defined as "Unknown format".
	        	break;
	    }

	    // Build a string with the channels type.
	    std::string channels_text{"Unknown channels"};
	    switch(channels) {
	    	case 1:
	    		channels_text = "(mono)";
	    		break;
	    	case 2:
	    		channels_text = "(stereo)";
	    		break;
	    	default:
	    		// Already defined as "Unknown channels".
	    		break;
	    }

		log_debug() << "Audio opened " << k_numbers_of_times_opened <<
			" time(s). Frequency: " << frequency << "Hz. Format: " << format_text <<
			". Channels: " << channels << " " << channels_text << ".";
	}
	else {
		log_error() << "Error querying the specifications of the audio. " << Mix_GetError();

		// Set the amount of times to close the audio to one, just for safety.
		times_to_close = 1;
	}

	// Close the audio the necessary amount of times.
	while(times_to_close > 0){
		Mix_CloseAudio();
		times_to_close--;
	}

	// "Since each call to Mix_Init may set different flags, there is no way, currently, to
	// request how many times each one was initted. In other words, the only way to quit for
	// sure is to do a loop like so: "
	while(Mix_Init(0)){
		Mix_Quit();
	}
}

/**
* Logs the SDL API version.
* @param library : What API/Library is being logged about.
* @param version_compiled : The compiled version.
* @param revision : If any, the revision.
*/
void LogSdlVersion(const std::string& library, const SDL_version& version_compiled) {
	log_debug() << library << " Version (compiled): " <<
		static_cast<int>(version_compiled.major) << "." <<
		static_cast<int>(version_compiled.minor) << "." <<
		static_cast<int>(version_compiled.patch);
}

/**
* Logs the SDL API version.
* @param library : What API/Library is being logged about.
* @param version_compiled : The compiled version.
* @param revision : If any, the revision.
*/
void LogSdlVersion(const std::string& library, const SDL_version& version_compiled,
		const std::string& revision) {
	log_debug() << library << " Version (compiled): " <<
		static_cast<int>(version_compiled.major) << "." <<
		static_cast<int>(version_compiled.minor) << "." <<
		static_cast<int>(version_compiled.patch) << " " << revision;
}

using FuncDriverName = const char* (*) (int);
using FuncNumDrivers = int (*) ();
/**
* @brief Logs information about available drivers.
*/
void LogSdlDrivers(FuncDriverName func_driver_name, FuncNumDrivers func_num_drivers,
		LogBuffer& drive_log) {
	///@todo Something for SDL_DisplayMode
	const int k_num_drivers = func_num_drivers();
	if(k_num_drivers >= 1) {
		drive_log << "\n > " << k_num_drivers << " drivers compiled into SDL:\n\t  ";
		for(int i = 0; i < k_num_drivers; i++) {
			drive_log << "(" << (i+1) << ") " << func_driver_name(i) << ", ";
		}
	}
	else {
		log_error() << "Could not get number of drivers. " << SDL_GetError();
	}
}

} // namespace

namespace sdl {

bool Initialize() {
	///@todo Separate all these initializations into separate functions.
	bool success_sdl = false;
	bool success_img = false;
	bool success_mixer = false;
	bool success_ttf = false;

	SDL_version compiled;

	log_debug() << "Initializing systems...";

	// Initializing SDL_TTF.
	const int k_ttf_init = TTF_Init();
	if(k_ttf_init == 0) {
		success_ttf = true;

		SDL_TTF_VERSION(&compiled);
		LogSdlVersion("SDL_TTF", compiled);
	}
	else {
		log_error() << "Could not initialize TTF." << TTF_GetError();
	}
	std::atexit(TTF_Quit);
	std::at_quick_exit(TTF_Quit);

	// Initializing SDL with k_init_flags.
	const Uint32 k_init_flags = SDL_INIT_EVERYTHING;
	const int sdl_init = SDL_Init(k_init_flags);
	if(sdl_init == 0) {
		success_sdl = true;

		SDL_version linked;
		SDL_VERSION(&compiled);
		SDL_GetVersion(&linked);

		LogSdlVersion("SDL", compiled, SDL_GetRevision());
	}
	else {
		log_error() << "Could not initialize SDL." << SDL_GetError();
	}
	std::atexit(SDL_Quit);
	std::at_quick_exit(SDL_Quit);

	// Initializing SDL_image with k_img_flags.
	const Uint32 k_img_flags = IMG_INIT_PNG;
	success_img = (static_cast<Uint32>(IMG_Init(k_img_flags)) & k_img_flags);
	if(success_img) {
		SDL_IMAGE_VERSION(&compiled);
		LogSdlVersion("SDL_image", compiled);
	}
	else {
		log_error() << "Could not initialize SDL_Image." << IMG_GetError();
	}
	std::atexit(IMG_Quit);
	std::at_quick_exit(IMG_Quit);

	// Initializing SDL_mixer.
	const int k_frequency = 44100;
	const int k_channels = 2;
	const int k_chunk_size = 4096;
	const int k_initialized_audio = Mix_OpenAudio(k_frequency, MIX_DEFAULT_FORMAT, k_channels,
		k_chunk_size);
	if(k_initialized_audio == 0) {
		success_mixer = true;

		SDL_MIXER_VERSION(&compiled);
		LogSdlVersion("SDL_mixer", compiled);

		const int k_channels_to_allocate = 25;
		const int k_channels_allocated = Mix_AllocateChannels(k_channels_to_allocate);

		log_debug() << "Allocated " << k_channels_allocated << " out of "
			<< k_channels_to_allocate << " requested channels for the mixer.";
	}
	else {
		log_error() << "Could not initialize SDL_Mixer" << Mix_GetError();
	}
	std::atexit(CloseMixer);
	std::at_quick_exit(CloseMixer);

	// Getting some video driver information. Enclosing this in a block so the logger gets
	// destroyed and therefore flushed.
	{
		LogBuffer driver_log{log_debug()};
		driver_log << "Getting driver information.";
		LogSdlDrivers(SDL_GetVideoDriver, SDL_GetNumVideoDrivers, driver_log);
		LogSdlDrivers(SDL_GetAudioDriver, SDL_GetNumAudioDrivers, driver_log);
	}

	// If even one system fails to initialize, returns false.
	return (success_sdl && success_img && success_mixer && success_ttf);
}

} // namespace sdl