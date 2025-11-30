#include <BlueMagma.hpp>
#include "DemoLayer.hpp"

int main(int argc, char* argv[])
{
	BM_LOG_FILE("Log/trace.log", BM::Log::Trace);
	BM_LOG_FILE("Log/error.log", BM::Log::Error);

	BM_LOG_FILE("Log/core.log", BM::Log::Trace);
	BM_LOG_FILE("Log/app.log", BM::Log::Trace);

	BM_LOG_INIT("Demo", BM::Log::Warn, BM::Log::Trace);

	BM_FN("argc: {}", argc);

	BM::ApplicationContext appContext;
	appContext._WindowContext._Size = { 960, 540 };
	appContext._WindowContext._VSync = false;

	BM::Application application(appContext);

	if (!application.GetAssets().LoadYaml("Config/LoadAsset.yml"))
		BM_ERROR("Couldn't load Asset Yaml");

	application.QueuePushLayer<DemoLayer>();
	application.Run();

	return 0;
}
