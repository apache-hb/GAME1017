#include <SDL_main.h>
#include "Engine.h"

int main(int argc, char* argv[]) // char** argv
{
	return Engine::Instance().Run();
}
