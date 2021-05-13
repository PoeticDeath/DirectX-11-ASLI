#pragma once
#include "Window.h"
#include "ChiliTimer.h"
#include "ImguiManager.h"
#include "Camera.h"

class App
{
public:
	App(INT i);
	// master frame / message loop
	int Go(INT i);
	~App();
private:
	void DoFrame( INT i );
private:
	ImguiManager imgui;
	Window wnd;
	ChiliTimer timer;
	std::vector<std::unique_ptr<class Drawable>> drawables1;
	std::vector<std::unique_ptr<class Drawable>> drawables2;
	float speed_factor = 1.0f;
	Camera cam;
	static constexpr size_t nDrawables = 1280;
};