#include "SDLApp/SDLApp.h"

struct TestApp : public SDLApp::SDLApp {
	virtual std::string getTitle() const { return "SDL Test"; }
};

SDLAPP_MAIN(TestApp);
