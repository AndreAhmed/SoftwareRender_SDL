#ifndef __RASTERIZER_H__
#define __RASTERIZER_H__

#include "Color.h"
#include "tgaimage.h"
#include "geometry.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

extern Matrix ModelView;
extern Matrix Projection;

struct IShader {
	virtual ~IShader();
	virtual Vec4f vertex(int iface, int nthvert) = 0;
	virtual bool fragment(Vec3f bar, TGAColor &color) = 0;
};

class Rasterizer
{
	public:
		uint32_t *m_FrameBuffer;
 		unsigned int m_Width, m_Height;
 	public:
		void SetFrameBuffer(uint32_t *frameBuffer, unsigned int width, unsigned int height);
		void SetPixel(unsigned int x, unsigned int y, const Color &color);
		void Clear();
		void DrawLine(const Color &color1, float x1, float y1, const Color &color2, float x2, float y2);
		void Draw_Rect(int x, int y, int width, int height);
		void viewport(int x, int y, int w, int h);
		void projection(float coeff);
		void lookat(Vec3f eye, Vec3f center, Vec3f up);
		Vec3f barycentric(Vec2f A, Vec2f B, Vec2f C, Vec2f P);
		void triangle(mat<4, 3, float> &clipc, IShader &shader, float *zbuffer);
		void Draw_Rectangle(int left, int top, int right, int bottom, Color&color);
		void DrawHorizontalLine(int x0, int y0, int w, Color color);
		void DrawTopTriangle(int x0, int y0, int x1, int y1, int x2, int y2, Color color);
		void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, Color color);
		void DrawBottomTriangle(int x0, int y0, int x1, int y1, int x2, int y3, Color color);

 
};

#endif /* __RASTERIZER_H__ */

