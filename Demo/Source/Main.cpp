#include <BlueMagma.hpp>
#include <BlueMagma/Core/Timer.hpp>
#include "DemoLayer.hpp"
#include <chrono>

static inline void InitLog(BM::Log::Level level, BM::Log::Level flushOn) noexcept {
	BM::ScopeTimer timer("InitLog");

	std::chrono::year_month_day ymd{ std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now()) };

	BM::Log::AddFileSink(BM::Log::FileContext{ .Name = std::format("Log/Trace/{}.log", ymd), .MaxFiles = 8192 });
	BM::Log::AddFileSink(BM::Log::FileContext{ .Name = "Log/error.log" }, BM::Log::Error, BM::Log::Error);

	BM::Log::AddFileSink(BM::Log::FileContext{ .Name = "Log/core.log" }, BM::Log::Trace, BM::Log::Off);
	BM::Log::AddFileSink(BM::Log::FileContext{ .Name = "Log/app.log" }, BM::Log::Error, BM::Log::Trace);

	BM_LOG_INIT("Demo", flushOn, level);
}

int main(int argc, char* argv[])
{
	InitLog(BM::Log::Trace, BM::Log::Warn);

	BM_FN("argc: {}", argc);

	BM::ApplicationContext appContext{
		.Window{.Size = { 1280, 720 }, .VSync = false}
	};

	BM::Application application(appContext);

	if (!application.GetAssets().LoadYaml("Config/LoadAsset.yml"))
		BM_ERROR("Couldn't load Asset Yaml");

	application.QueuePushLayer<DemoLayer>();
	application.Run();

	return 0;
}
