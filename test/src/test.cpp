#include "SDLApp/SDLApp.h"

struct TestApp : public SDLApp::SDLApp {
	virtual std::string getTitle() { return "SDL Test"; }
};

SDLAPP_MAIN(TestApp);
