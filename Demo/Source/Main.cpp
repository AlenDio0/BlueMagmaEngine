#include <BlueMagma.hpp>
#include "DemoLayer.hpp"

int main(int argc, char* argv[])
{
	BM::ApplicationContext appContext;
	appContext._WindowContext._Size = { 960, 540 };
	appContext._WindowContext._VSync = false;

	BM::Application application(appContext);

	if (!application.GetAssets().LoadYaml("Config/LoadAsset.yml"))
		; // TODO: Log error

	application.PushLayer<DemoLayer>();
	application.Run();

	return 0;
}
