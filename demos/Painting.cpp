/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/MACE.h>

using namespace mc;

class PainterDemo: public gfx::Entity2D {
public:
	void onInit() override {
		lena = gfx::Texture::createFromFile(MACE_DEMO_ASSETS + std::string("lena.jpg"), gfx::Enums::ImageFormat::RGBA);

		star = gfx::Texture::createFromFile(MACE_DEMO_ASSETS + std::string("star.png"), gfx::Enums::ImageFormat::LUMINANCE_ALPHA);
	}

	void onUpdate() override {}

	void onRender(gfx::Painter& p) override {
		p.getTransformation().scaler = { 1.0f / 3.0f, 1.0f / 3.0f, 1.0f };
		p.getTransformation().translation = { -2.0f / 3.0f, 2.0f / 3.0f, 0.0f };

		//normal image (no filter)
		p.drawImage(lena);

		p.translate(2.0f / 3.0f, 0.0f);
		//give the texture a blueish hue
		p.setTexture(lena, gfx::Enums::TextureSlot::FOREGROUND);
		p.setForegroundColor(Color(0.0f, 0.0f, 1.0f, 0.5f));
		p.drawQuad(gfx::Enums::Brush::TEXTURE);

		p.translate(2.0f / 3.0f, 0.0f);
		//sepia filter
		p.setFilter(Matrix4f({
			{ 0.3588f, 0.7044f, 0.1368f, 0.0f },
			{ 0.2990f, 0.5870f, 0.1140f, 0.0f },
			{ 0.2392f, 0.4696f, 0.0912f, 0.0f },
			{ 0.0f, 0.0f, 0.0f, 1.0f }
		}));
		p.drawImage(lena);

		p.translate(0.0f, -2.0f / 3.0f);
		//filter out green in the texture
		p.setFilter(1.0f, 0.0f, 1.0f);
		p.drawImage(lena);

		p.translate(-2.0f / 3.0f, 0.0f);
		//reset image filter to normal
		p.setFilter(1.0f, 1.0f, 1.0f);
		//blend lena and star together (70% lena, 30% star)
		p.blendImages(lena, star, 0.3f);

		p.translate(-2.0f / 3.0f, 0.0f);
		//render lena with a star mask
		p.maskImage(lena, star);

		p.translate(0.0f, -2.0f / 3.0f);
		//render half lena half star in one draw call
		p.blendImagesMasked(star, lena, gfx::Texture::getGradient(), 0.0f, 0.5f);

		p.translate(2.0f / 3.0f, 0.0f);
		p.setTexture(lena, gfx::Enums::TextureSlot::FOREGROUND);
		p.setForegroundTransform(Vector4f({0.25f, 0.25f, 0.5f, 0.5f}));
		//render lena slightly zoomed in
		p.drawQuad(gfx::Enums::Brush::TEXTURE);

		p.translate(2.0f / 3.0f, 0.0f);
		p.setForegroundColor(Colors::YELLOW);
		p.fillRect();//fill the last square with yellow
	}

	void onDestroy() override {
		lena.destroy();
		star.destroy();
	}
private:
	gfx::Texture lena;
	gfx::Texture star;
};

int main() {
	Instance instance = Instance();
	try {
		gfx::WindowModule::LaunchConfig config = gfx::WindowModule::LaunchConfig(768, 768, "Painting Demo");
		config.resizable = true;

		gfx::WindowModule module = gfx::WindowModule(config);
		instance.addModule(module);
		module.addChild(std::shared_ptr<gfx::Entity>(new PainterDemo()));

		os::ErrorModule errModule = os::ErrorModule();
		instance.addModule(errModule);

		instance.start();
	} catch (const std::exception& e) {
		Error::handleError(e, instance);
		return -1;
	}
	return 0;
}
