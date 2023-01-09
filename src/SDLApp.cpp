#include "SDLApp/SDLApp.h"

#if !PLATFORM_MSVC && !PLATFORM_CLANG_WIN
#include <unistd.h>
#endif

//all just for the chdir ...
#if PLATFORM_LINUX
#include <linux/limits.h>	//PATH_MAX
#endif

//SDL_main...
// TODO move this to Common?
// but somewhere it isn't compiled into a symbol immediately
// either as a Common:: function or a macro or something
int main(int argc, char *argv[]) {
	std::vector<std::string> args;
	std::copy(argv, argv+argc, std::back_inserter<std::vector<std::string>>(args));

//fix the fact that osx doesn't know where it is being run from
#if PLATFORM_OSX
	if (args.size() == 0) throw Common::Exception() << "expected arg for the exe path";
	std::string exe = args[0];
	exe = exe.substr(0, exe.find_last_of('/'));
	exe = exe.substr(0, exe.find_last_of('/')) + "/Resources";
	if (chdir(exe.c_str())) {
		throw Common::Exception() << "chdir failed with error " << errno;
	}
#endif
	
//you know, no guarantees for Linux either
//https://stackoverflow.com/questions/4025370/can-an-executable-discover-its-own-path-linux
#if PLATFORM_LINUX
	pid_t pid = getpid();
	std::string path = "/proc/" + std::to_string(pid) + "/exe";

	std::vector<char> dest(PATH_MAX+1, 0);// readlink does not null terminate!
	if (readlink(path.c_str(), dest.data(), PATH_MAX) == -1) {
		throw Common::Exception() << "readlink failed to find the current path";
	}
	std::string deststr(dest.begin(), dest.end());
	deststr = deststr.substr(0, deststr.find_last_of('/'));
	if (chdir(deststr.c_str())) {
		throw Common::Exception() << "chdir failed with error " << errno;
	}
#endif
	
	std::shared_ptr<::SDLApp::SDLApp> app = ::SDLApp::SDLApp::createMainSDLApp();
	
	app->init(args);
	app->loop();
	return app->getExitCode();
}


namespace SDLApp {

void SDLApp::init(const Init& args) {
	int sdlInitError = SDL_Init(getSDLInitFlags());
	if (sdlInitError) throw Common::Exception() << "SDL_Init failed with error code " << sdlInitError;
	initWindow();
	onResize();
}

int SDLApp::getSDLInitFlags() {
	return SDL_INIT_VIDEO;
}

void SDLApp::initWindow() {
	window = SDL_CreateWindow(
		getTitle().c_str(),			// title
		SDL_WINDOWPOS_CENTERED,		// x
		SDL_WINDOWPOS_CENTERED,		// y
		screenSize.x,				// w
		screenSize.y,				// h
		getSDLCreateWindowFlags()	// flags
	);
	if (!window) throw Common::Exception() << "failed to create window";
}

Uint32 SDLApp::getSDLCreateWindowFlags() {
	return SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN;
}

std::string SDLApp::getTitle() const {
	return "SDL App";
}

SDLApp::~SDLApp() {
	if (window) SDL_DestroyWindow(window);
	SDL_Quit();
}

void SDLApp::loop() {
	SDL_Event event;
	while (!done) {
		while (SDL_PollEvent(&event) > 0) {
			switch (event.type) {
			case SDL_QUIT:
				done = true;
				break;
			case SDL_WINDOWEVENT:
				switch (event.window.event) {
				case SDL_WINDOWEVENT_RESIZED:
					screenSize.x = event.window.data1;
					screenSize.y = event.window.data2;
					aspectRatio = (float)screenSize.x / (float)screenSize.y;
					onResize();
					break;
				}
				break;
			case SDL_KEYDOWN:
#if PLATFORM_WINDOWS
					if (event.key.keysym.sym == SDLK_F4 && (event.key.keysym.mod & KMOD_ALT) != 0) {
						done = true;
					}
#endif
#if PLATFORM_OSX
					if (event.key.keysym.sym == SDLK_q && (event.key.keysym.mod & KMOD_GUI) != 0) {
						done = true;
					}
#endif
				break;
			}
			onSDLEvent(event);
		}

		onUpdate();

		postUpdate();
	}
}

void SDLApp::requestExit() {
	done = true;
}

void SDLApp::requestExit(int code) {
	exitCode = code;
	requestExit();
}

int SDLApp::getExitCode() const {
	return exitCode;
}

void SDLApp::onResize() {
	SDL_SetWindowSize(window, screenSize.x, screenSize.y);
}

void SDLApp::onSDLEvent(SDL_Event& event) {
}

void SDLApp::onUpdate() {
}

void SDLApp::postUpdate() {
}

Tensor::int2 SDLApp::getScreenSize() const {
	return screenSize;
}

float SDLApp::getAspectRatio() const {
	return aspectRatio;
}

SDL_Window * SDLApp::getWindow() const {
	return window;
}

}
