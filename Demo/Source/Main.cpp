#include <BlueMagma.hpp>
#include "DemoLayer.hpp"

int main(int argc, char* argv[])
{
	BM::ApplicationContext appContext;
	appContext._WindowContext._Size = { 960, 540 };
	appContext._WindowContext._VSync = false;

	BM::Application application(appContext);

	BM::AssetManager& assets = application.GetAssets();
	assets.Load<BM::Texture>("cat", "Asset/cat.png");
	assets.Load<BM::Font>("minecraft", "Asset/MinecraftRegular.otf");
	assets.Load<BM::SoundBuffer>("transition", "Asset/puh.mp3");

	application.PushLayer<DemoLayer>();
	application.Run();

	return 0;
}
