#include "App.h"
#include "Melon.h"
#include "Pyramid.h"
#include "Box.h"
#include "Sheet.h"
#include "SkinnedBox.h"
#include <memory>
#include <algorithm>
#include "ChiliMath.h"
#include "Surface.h"
#include "GDIPlusManager.h"
#include "imgui/imgui.h"

namespace dx = DirectX;

GDIPlusManager gdipm;

App::App()
	:
	wnd( 1920,1080,"ASLI GPU Rendering" )
{
	class Factory
	{
	public:
		Factory( Graphics& gfx )
			:
			gfx( gfx )
		{}
		std::unique_ptr<Drawable> operator()()
		{
			switch( typedist( rng ) )
			{
			case 0:
				return std::make_unique<Pyramid>(
					gfx,rng,adist,ddist,
					odist,rdist
				);
			case 1:
				return std::make_unique<Box>(
					gfx,rng,adist,ddist,
					odist,rdist,bdist
				);
			case 2:
				return std::make_unique<Melon>(
					gfx,rng,adist,ddist,
					odist,rdist,longdist,latdist
				);
			case 3:
				return std::make_unique<Sheet>(
					gfx,rng,adist,ddist,
					odist,rdist
				);
			case 4:
				return std::make_unique<SkinnedBox>(
					gfx,rng,adist,ddist,
					odist,rdist
				);
			default:
				assert( false && "bad drawable type in factory" );
				return {};
			}
		}
	private:
		Graphics& gfx;
		std::mt19937 rng{ std::random_device{}() };
		std::uniform_real_distribution<float> adist{ 0.0f,PI * 2.0f };
		std::uniform_real_distribution<float> ddist{ 0.0f,PI * 0.5f };
		std::uniform_real_distribution<float> odist{ 0.0f,PI * 0.08f };
		std::uniform_real_distribution<float> rdist{ 6.0f,20.0f };
		std::uniform_real_distribution<float> bdist{ 0.4f,3.0f };
		std::uniform_int_distribution<int> latdist{ 5,20 };
		std::uniform_int_distribution<int> longdist{ 10,40 };
		std::uniform_int_distribution<int> typedist{ 0,4 };
	};

	drawables.reserve( nDrawables );
	std::generate_n( std::back_inserter( drawables ),nDrawables,Factory{ wnd.Gfx( 1,true,true ) } );

	wnd.Gfx( 1,true,true ).SetProjection(dx::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));
	wnd.Gfx( 2,true,true ).SetProjection(dx::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));
}

void App::DoFrame(INT i)
{
	const auto dt = timer.Mark() * speed_factor;

	if ( i == 2 )
	{
		if ( GPU2 )
		{
			wnd.Gfx(i, GPU1, GPU2).BeginFrame(0.07f, 0.0f, 0.12f);
			wnd.Gfx(i, GPU1, GPU2).SetCamera(cam.GetMatrix());
		}
		else
		{
			i = 1;
		}
	}
	if (i == 1)
	{
		if ( GPU1 )
		{
			wnd.Gfx(i, GPU1, GPU2).BeginFrame(0.07f, 0.0f, 0.12f);
			wnd.Gfx(i, GPU1, GPU2).SetCamera(cam.GetMatrix());
		}
		else
		{
			if ( GPU2 )
			{
				i = 2;
				wnd.Gfx(i, GPU1, GPU2).BeginFrame(0.07f, 0.0f, 0.12f);
				wnd.Gfx(i, GPU1, GPU2).SetCamera(cam.GetMatrix());
			}
		}
	}

	if ( GPU2 )
	{
		if ( i == 2 )
		{
			for (auto& d : drawables)
			{
				d->Update(wnd.kbd.KeyIsPressed(VK_SPACE) ? 0.0f : dt);
				d->Draw(wnd.Gfx(i, GPU1, GPU2));
			}
		}
	}
	if ( GPU1 )
	{
		if ( i == 1 )
		{
			for (auto& d : drawables)
			{
				d->Update(wnd.kbd.KeyIsPressed(VK_SPACE) ? 0.0f : dt);
				d->Draw(wnd.Gfx(i, GPU1, GPU2));
			}
		}
	}

	if ( !GPU1 )
	{
		if ( !GPU2 )
		{
			std::exit(EXIT_SUCCESS);
		}
	}

	// imgui window to control simulation speed
	if( ImGui::Begin( "Simulation Speed" ) )
	{
		ImGui::SliderFloat( "Speed Factor",&speed_factor,0.0f,4.0f );
		ImGui::Checkbox( "GPU1",&GPU1 );
		ImGui::Checkbox( "GPU2",&GPU2 );
		ImGui::Text( "%.3f ms/frame (%.1f FPS)",1000.0f / ImGui::GetIO().Framerate,ImGui::GetIO().Framerate );
		ImGui::Text( "Status: %s",wnd.kbd.KeyIsPressed( VK_SPACE ) ? "PAUSED" : "RUNNING (hold spacebar to pause)" );
	}
	ImGui::End();
	// imgui window to control camera
	cam.SpawnControlWindow();

	// present
	wnd.Gfx(i, GPU1, GPU2).EndFrame();
}

App::~App()
{}


int App::Go()
{
	INT i = 1;
	while( true )
	{
		// process all messages pending, but to not block for new messages
		if( const auto ecode = Window::ProcessMessages() )
		{
			// if return optional has value, means we're quitting so return exit code
			return *ecode;
		}
		if ( i == 1 )
		{
			i = 2;
		}
		else
		{
			i = 1;
		}
		DoFrame( i );
	}
}