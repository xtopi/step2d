#include "program.h"
#include "progapp.h"

// Choose the target renderer
//#define USE_D3D
#define USE_OPENGL_2
//#define USE_OPENGL_1

#ifdef USE_D3D
#include <ClanLib/d3d.h>
#endif

#ifdef USE_OPENGL_1
#include <ClanLib/gl1.h>
#endif

#ifdef USE_OPENGL_2
#include <ClanLib/gl.h>
#endif

int ClanMain(const std::vector<std::string> &args)
{
	try
	{
		// Initialize ClanLib base components
		SetupCore setup_core;

		// Initialize the ClanLib display component
		SetupDisplay setup_display;

		#ifdef USE_D3D
			SetupD3D setup_d3d;
		#endif

		#ifdef USE_OPENGL_1
			SetupGL1 setup_gl1;
		#endif

		#ifdef USE_OPENGL_2
			SetupGL setup_gl;
		#endif

		// Start the Application
		TApplication app;
		int retval = app.start(args);
		return retval;
	}
	catch(Exception &exception)
	{
		// Create a console window for text-output if not available
		ConsoleWindow console("Console", 80, 160);
		Console::write_line("Exception caught: " + exception.get_message_and_stack_trace());
		console.display_close_message();

		return -1;
	}
}

// Instantiate Application
Application app(&ClanMain);
