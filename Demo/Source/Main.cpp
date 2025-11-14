#include <BlueMagma.hpp>

int main(int argc, char* argv[])
{
	BM::ApplicationContext appContext;
	appContext._WindowContext._Width = 960;
	appContext._WindowContext._Height = 540;

	BM::Application application(appContext);
	application.Run();

	return 0;
}
