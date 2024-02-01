//------------------------------------------------------------------------------
// main.cc
// (C) 2015-2022 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "config.h"
#include "exampleapp.h"

//------------------------------------------------------------------------------
/**
*/

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifdef _DEBUG
#define DBG_NEW new ( _NORMAL_BLOCK , FILE , LINE )
// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
// allocations to be of _CLIENT_BLOCK type
#else
#define DBG_NEW new
#endif

int
main(int argc, const char** argv)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	Example::ExampleApp app;
	if (app.Open())
	{
		app.Run();
		app.Close();
	}
	app.Exit();	
	_CrtDumpMemoryLeaks();
	return app.ExitCode();
}