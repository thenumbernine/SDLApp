#include "Tensor/Vector.h"
#include "SDL.h"
#include <vector>
#include <string>

namespace SDLApp {

struct SDLApp {
protected:
	bool done = {};
	Tensor::int2 screenSize = {640, 480};
	float aspectRatio = (float)screenSize.x / (float)screenSize.y;
	SDL_Window *window = {};
	int exitCode = {};

public:
	//init args
	using Init = std::vector<std::string>;

	/*
	the external program's subclass of SDLApp will implement this to return an instance of itself
	 (using the SDLAPP_MAIN macro)
	*/
	static std::shared_ptr<::SDLApp::SDLApp> createMainSDLApp();
	
	virtual ~SDLApp();

	//vtable isn't set up yet so don't pass args into SDLApp via ctor
	virtual void init(Init const & args);
protected:
	//used for window construction during init()

	// initialize the 'window' field
	//vtable isn't set up yet so don't pass args into SDLApp via ctor
	virtual void initWindow();
	virtual Uint32 getSDLCreateWindowFlags();
	virtual int getSDLInitFlags();

public:
	virtual std::string getTitle() const;

	virtual void loop();

	virtual void requestExit();	//request exit
	virtual void requestExit(int code);

	virtual int getExitCode() const;

	virtual void onResize();
	virtual void onSDLEvent(SDL_Event &event);
	virtual void onUpdate();
	virtual void postUpdate();

	virtual Tensor::int2 getScreenSize() const;
	virtual float getAspectRatio() const;

	//used for access
	SDL_Window * getWindow() { return window; }
};

}

/*
Define this somewhere to provide the function which SDLApp.cpp expects in its main() for building the App object

When declaring type names, when the namespace matches the class name that this is a static method within, I often have to use :: prefix to deonate the namespace is found in global scope and the class name is found in the namespace.
However I cannot use the :: prefix on the function name, or else it will give me an error "error: ‘SDLApp’ in ‘class std::shared_ptr<SDLApp::SDLAppASDF>’ does not name a type" (even if I keep the namespace and class names distinct, i.e. struct SDLApp -> struct SDLAppASDF).
*/
#define SDLAPP_MAIN(classname)\
namespace SDLApp {\
	std::shared_ptr<::SDLApp::SDLApp> SDLApp::SDLApp::createMainSDLApp() {\
		return std::dynamic_pointer_cast<::SDLApp::SDLApp>(std::make_shared<classname>());\
	}\
}
