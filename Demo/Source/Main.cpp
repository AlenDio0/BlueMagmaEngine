#include <BlueMagma.hpp>
#include <BlueMagma/Core/Timer.hpp>
#include <BlueMagma/Core/Random.hpp>
#include "GameLayer.hpp"
#include <chrono>

static inline void InitLog(BM::Log::Level flushOn, BM::Log::Level consoleLevel) noexcept {
	BM::ScopeTimer timer(std::format("{} > {}(...)", BM_FILENAME, __FUNCTION__));

	std::chrono::year_month_day ymd{ std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now()) };

	BM::Log::AddFileSink(BM::Log::FileContext{ .Name = std::format("Log/Trace/{}.log", ymd), .MaxFiles = 1024, .RotateOnOpen = false });
	BM::Log::AddFileSink(BM::Log::FileContext{ .Name = "Log/error.log" }, BM::Log::Error, BM::Log::Error);

	BM::Log::AddFileSink(BM::Log::FileContext{ .Name = "Log/core.log" }, BM::Log::Trace, BM::Log::Off);
	BM::Log::AddFileSink(BM::Log::FileContext{ .Name = "Log/app.log" }, BM::Log::Error, BM::Log::Trace);

	BM::Log::Init("Demo", flushOn, consoleLevel);
}

static inline void RunApplication(BM::Vec2u windowSize) {
	BM_FN_ARGS(windowSize);

	const size_t cRadomSeed = BM_RANDOM(1, 10);
	BM_RANDOM_SEED(cRadomSeed);
	BM_INFO("Main Random seed is set to: '{}'", cRadomSeed);

	BM::ApplicationContext appContext{};
	BM::Application application(appContext);

	if (!application.Assets.LoadYaml("Config/LoadAsset.yml"))
		BM_ERROR("Couldn't load Asset Yaml");

	application.GetRenderer();

	if (auto font = application.Assets.Retrieve<BM::Font>("Minecraft").lock())
	{
		BM::Font newFont(*font);
		newFont.setSmooth(false);
		application.Assets.LoadAsset("Minecraft Nearest", std::make_shared<BM::Font>(newFont));
	}

	BM::WindowContext windowContext{ .InitialMode{ windowSize } };
	application.CreateOrReplaceWindow(windowContext);

	BM::ScopeTimer timer("Main Application");

	application.QueuePushLayer<GameLayer>();
	application.Run();
}

int main(int argc, char* argv[])
{
	InitLog(BM::Log::Trace, BM::Log::Trace);
	BM_FN_ARGS(argc, argv[0]);

	RunApplication({ 1920u, 1080u });

	return 0;
}
