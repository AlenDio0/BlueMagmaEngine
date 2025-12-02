#include <BlueMagma.hpp>
#include "DemoLayer.hpp"
#include <chrono>

static inline void InitLog(BM::Log::Level level, BM::Log::Level flushOn) noexcept {

	std::chrono::year_month_day ymd{ std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now()) };

	BM::Log::AddFileSink({ std::format("Log/Trace/{}.log", ymd), BM::Log::Trace, 8192 });
	BM::Log::AddFileSink({ "Log/error.log", BM::Log::Error });

	BM::Log::AddFileSink({ "Log/core.log", BM::Log::Trace });
	BM::Log::AddFileSink({ "Log/app.log", BM::Log::Trace });

	BM_LOG_INIT("Demo", flushOn, level);
}

int main(int argc, char* argv[])
{
	InitLog(BM::Log::Trace, BM::Log::Warn);

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
