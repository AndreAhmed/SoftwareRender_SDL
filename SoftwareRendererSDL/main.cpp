/*
3D Software Renderer by Ahmed Saleh Tolba
Adapted to wrok for ARM Cortex M3
Last edit 13.7.2014
An Educational project for Both Embedded and Software Engineers in Egypt.
*/


#include <cstdio>
#include <cmath>
#include <SDL.h>
#include "Rasterizer.h"
#include "Timer.h"
#include "Model.h"
#include "geometry.h"
#include "tgaimage.h"
#include <algorithm>
Vec3f light_dir(1, 1, 1);
Vec3f       eye(1, 1, -220);
Vec3f    center(0, 0, 0);
Vec3f        up(0, 1, 0);

static bool g_Running = true;
cTimer timer;
Model * model = nullptr;

struct Shader : public IShader {
	mat<2, 3, float> varying_uv;  // triangle uv coordinates, written by the vertex shader, read by the fragment shader
	mat<4, 3, float> varying_tri; // triangle coordinates (clip coordinates), written by VS, read by FS
	mat<3, 3, float> varying_nrm; // normal per vertex to be interpolated by FS
	mat<3, 3, float> ndc_tri;     // triangle in normalized device coordinates

	virtual Vec4f vertex(int iface, int nthvert) {
		varying_uv.set_col(nthvert, model->uv(iface, nthvert));
		varying_nrm.set_col(nthvert, proj<3>((Projection*ModelView).invert_transpose()*embed<4>(model->normal(iface, nthvert), 0.f)));
		Vec4f gl_Vertex = Projection * ModelView*embed<4>(model->vert(iface, nthvert));
		varying_tri.set_col(nthvert, gl_Vertex);
		ndc_tri.set_col(nthvert, proj<3>(gl_Vertex / gl_Vertex[3]));
		return gl_Vertex;
	}

	virtual bool fragment(Vec3f bar, TGAColor &color) {
		Vec3f bn = (varying_nrm*bar).normalize();
		Vec2f uv = varying_uv * bar;

		mat<3, 3, float> A;
		A[0] = ndc_tri.col(1) - ndc_tri.col(0);
		A[1] = ndc_tri.col(2) - ndc_tri.col(0);
		A[2] = bn;

		mat<3, 3, float> AI = A.invert();

		Vec3f i = AI * Vec3f(varying_uv[0][1] - varying_uv[0][0], varying_uv[0][2] - varying_uv[0][0], 0);
		Vec3f j = AI * Vec3f(varying_uv[1][1] - varying_uv[1][0], varying_uv[1][2] - varying_uv[1][0], 0);

		mat<3, 3, float> B;
		B.set_col(0, i.normalize());
		B.set_col(1, j.normalize());
		B.set_col(2, bn);

		Vec3f n = (B*model->normal(uv)).normalize();

		float diff = std::max(0.f, n*light_dir);
		color = model->diffuse(uv)*diff;

		return false;
	}
};



int main(int argc, char *argv[])
{
	timer.Initialize();

	// initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		fprintf(stderr, "SDL_Init failed\n");
		return 1;
	}

	// create window for drawing
	SDL_Surface *screen = SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, 32, SDL_HWSURFACE);
	if (!screen) {
		SDL_Quit();
		fprintf(stderr, "SDL_SetVideoMode failed\n");
		return 1;
	}

	SDL_WM_SetCaption("Softawre Renderer on ARM ", NULL);

	Rasterizer raster;
	raster.SetFrameBuffer((uint32_t *)screen->pixels, WINDOW_WIDTH, WINDOW_HEIGHT);
	Uint32 old_time, current_time;
	float ftime;

	// Need to initialize this here for event loop to work
	current_time = SDL_GetTicks();

	float r = 0.0f;
	unsigned int lastTicks = SDL_GetTicks();
	model = new Model("african_head/african_head.obj");

	float *zbuffer = new float[WINDOW_WIDTH*WINDOW_HEIGHT];
	for (int i = WINDOW_WIDTH * WINDOW_HEIGHT; i--; zbuffer[i] = -std::numeric_limits<float>::max());




	// loop until we're done running the program
	while (g_Running) {
		timer.UpdateTimer();
		timer.UpdateFPS();
		float time = timer.GetTime();
		// handle events
		SDL_Event event;
		if (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				g_Running = false; // exit
			}

			if (event.type == SDL_KEYDOWN)
			{
				SDLKey keyPressed = event.key.keysym.sym;

				switch (keyPressed)
				{
				case SDLK_UP:
					break;
				case SDLK_DOWN:
					break;
				}
			}
		}

		// lock surface and clear framebuffer
		SDL_LockSurface(screen);
		raster.Clear();

		const float tau = std::atan(1.0f) * 8.0f; // tau is another name for 2*pi
		//printf("time is %f", time);
		static float cnt = 0;
		const float angular_frequency = tau / 3.0f;  // 3 seconds per cycle
		const float radius = 10.0f;

		cnt += time * 0.001f;


		raster.lookat(eye, center, up);
		raster.viewport(WINDOW_WIDTH / 8, WINDOW_HEIGHT / 8, WINDOW_WIDTH * 3 / 4, WINDOW_HEIGHT * 3 / 4);
		raster.projection(-1.f / (eye - center).norm());
		light_dir = proj<3>((Projection*ModelView*embed<4>(light_dir, 0.f))).normalize();

		Shader shader;
		for (int i = 0; i < model->nfaces(); i++) {
			for (int j = 0; j < 3; j++) {
				shader.vertex(i, j);
			}
			raster.triangle(shader.varying_tri, shader, zbuffer);
		}



		// unlock and update surface
		SDL_UnlockSurface(screen);
		SDL_UpdateRect(screen, 0, 0, 0, 0);
		// display frames per second
		int fps = timer.GetFps();
		printf("Frames per second: %u\t\r", fps);




		_sleep(30); // put some delay to sync the CPU loop, and not stall it -_- 

	}

	SDL_Quit();
	return 0;
}

