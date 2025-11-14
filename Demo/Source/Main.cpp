#include <BlueMagma.hpp>

int main(int argc, char* argv[])
{
	BM::ApplicationContext appContext;
	appContext._WindowWidth = 1600;
	appContext._WindowHeight = 900;

	BM::Application application(appContext);
	application.Run();

	return 0;
}
