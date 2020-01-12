 
#include <cstring>
#include <cmath>
#include <search.h>
#include "Rasterizer.h"
#include <algorithm>

void swap(int &a, int &b)
{
	int temp = a;
	a = b;
	b = temp;
}

void
Rasterizer::SetFrameBuffer(uint32_t *frameBuffer,
                           unsigned int width, unsigned int height)
{
	m_FrameBuffer = frameBuffer;
	m_Width = width;
	m_Height = height;
}

void
Rasterizer::SetPixel(unsigned int x, unsigned int y, const Color &color)
{
	if(x >= m_Width || y >= m_Height)
		return;
	if (x < 0 || y < 0)
			return;

	m_FrameBuffer[y * m_Width + x] = color.fromByte();
}
 
void
Rasterizer::Clear()
{
	memset(m_FrameBuffer, 0, sizeof(uint32_t) * m_Height * m_Width);
}



void Rasterizer::Draw_Rect(int x, int y, int width, int height)
{
	 // must do clipping!

	for (int ymin = y; ymin < y + height; ymin++)
	{

		for (int xmin = x; xmin < x + width; xmin++)
		{

			SetPixel((unsigned int)xmin, (unsigned int)ymin, Color(255, 0, 0, 255));
		}
	}
}
 
 

void Rasterizer::DrawHorizontalLine(int x0, int y0, int w, Color color)
{
	DrawLine(color, x0, y0, color, x0 + w - 1, y0);
}

void Rasterizer::DrawTopTriangle(int x0, int y0, int x1, int y1, int x2, int y2, Color color)
{
	int temp_x;
	 
	// compute deltas
	float dxy_left = (float)(x2 - x0) / (y2 - y0);
	float dxy_right = (float)(x2 - x1) / (y2 - y1);
	// set starting and ending points for edge trace
	float xs = x0;
	float xe = x1;
	// draw each scanline
	for (int y = y0; y <= y2; y++)
	{
		// draw a line from xs to xe at y in color c
		DrawLine(color, (int)(xs + 0.5), y,color, (int)(xe + 0.5), y);
		// move down one scanline
		xs += dxy_left;
		xe += dxy_right;
	} // end for y
}
void Rasterizer::DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, Color color)
{
	int temp_x, // used for sorting
		temp_y;
	if ((x1 == x2 && x2 == x3) || (y1 == y2 && y2 == y3))
			return;
	// sort p1,p2,p3 in ascending y order
	if (y2 < y1)
	{
		temp_x = x2;
		temp_y = y2;
		x2 = x1;
		y2 = y1;
		x1 = temp_x;
		y1 = temp_y;
	} // end if
	  // now we know that p1 and p2 are in order
	if (y3 < y1)
	{
		temp_x = x3;
		temp_y = y3;
		x3 = x1;
		y3 = y1;
		x1 = temp_x;
		y1 = temp_y;
	} // end if
	  // finally test y3 against y2
	if (y3 < y2)
	{
		temp_x = x3;
		temp_y = y3;
		x3 = x2;
		y3 = y2;
		x2 = temp_x;
		y2 = temp_y;
	} // end if


	if (y2 == y3)
	{
		DrawBottomTriangle(x1, y1, x2,y2,x3,y3, color);
	}
	/* check for trivial case of top-flat triangle */
	else if (y1 == y2)
	{
		DrawTopTriangle(x1, y1, x2, y2, x3, y3, color);
	}
	else
	{
		/* general case - split the triangle in a topflat and bottom-flat one */
		int vTmpx;
		vTmpx = (int)(x1 + ((float)(y2 - y1) / (float)(y3 - y1)) * (x3 - x1));
		 
		DrawBottomTriangle(x1, y1, vTmpx, y2, x2, y2, color);
		DrawTopTriangle(x2, y2, vTmpx, y2, x3, y3, color);
	}

}
void Rasterizer::DrawBottomTriangle(int x0, int y0, int x1, int y1, int x2, int y2, Color color)
{
	int temp_x;
 
	float dxy_left = (float)(x2 - x0) / (y2 - y0);
	float dxy_right = (float)(x1 - x0) / (y1 - y0);
 
	// set starting and ending points for edge trace
	float xs = x0;
	float xe = x0;
 
	// draw each scanline
	for (int y = y0; y <= y1; y++)
	{
		// draw a line from xs to xe at y in color c
		DrawLine(color, (int)xs, y, color, (int)xe, (int)y);
		// move down one scanline
		xs += dxy_left;
		xe += dxy_right;
	} // end for y
}

Matrix ModelView;
Matrix Viewport;
Matrix Projection;

IShader::~IShader() {}

void Rasterizer::viewport(int x, int y, int w, int h) {
	Viewport = Matrix::identity();
	Viewport[0][3] = x + w / 2.f;
	Viewport[1][3] = y + h / 2.f;
	Viewport[2][3] = 1.f;
	Viewport[0][0] = w / 2.f;
	Viewport[1][1] = h / 2.f;
	Viewport[2][2] = 0;
}

void Rasterizer::projection(float coeff) {
	Projection = Matrix::identity();
	Projection[3][2] = coeff;
}

void Rasterizer::lookat(Vec3f eye, Vec3f center, Vec3f up) {
	Vec3f z = (eye - center).normalize();
	Vec3f x = cross(up, z).normalize();
	Vec3f y = cross(z, x).normalize();
	Matrix Minv = Matrix::identity();
	Matrix Tr = Matrix::identity();
	for (int i = 0; i<3; i++) {
		Minv[0][i] = x[i];
		Minv[1][i] = y[i];
		Minv[2][i] = z[i];
		Tr[i][3] = -center[i];
	}
	ModelView = Minv * Tr;
}

Vec3f Rasterizer::barycentric(Vec2f A, Vec2f B, Vec2f C, Vec2f P) {
	Vec3f s[2];
	for (int i = 2; i--; ) {
		s[i][0] = C[i] - A[i];
		s[i][1] = B[i] - A[i];
		s[i][2] = A[i] - P[i];
	}
	Vec3f u = cross(s[0], s[1]);
	if (std::abs(u[2])>1e-2) // dont forget that u[2] is integer. If it is zero then triangle ABC is degenerate
		return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
	return Vec3f(-1, 1, 1); // in this case generate negative coordinates, it will be thrown away by the rasterizator
}

void Rasterizer::triangle(mat<4, 3, float> &clipc, IShader &shader, float *zbuffer) {
	mat<3, 4, float> pts = (Viewport*clipc).transpose(); // transposed to ease access to each of the points
	mat<3, 2, float> pts2;
	for (int i = 0; i<3; i++) pts2[i] = proj<2>(pts[i] / pts[i][3]);

	Vec2f bboxmin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	Vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
	Vec2f clamp(m_Width - 1, m_Height - 1);
	for (int i = 0; i<3; i++) {
		for (int j = 0; j<2; j++) {
			bboxmin[j] = std::max(0.f, std::min(bboxmin[j], pts2[i][j]));
			bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts2[i][j]));
		}
	}
	Vec2i P;
	TGAColor color;
	for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) {
		for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) {
			Vec3f bc_screen = barycentric(pts2[0], pts2[1], pts2[2], P);
			Vec3f bc_clip = Vec3f(bc_screen.x / pts[0][3], bc_screen.y / pts[1][3], bc_screen.z / pts[2][3]);
			bc_clip = bc_clip / (bc_clip.x + bc_clip.y + bc_clip.z);
			float frag_depth = clipc[2] * bc_clip;
			if (bc_screen.x<0 || bc_screen.y<0 || bc_screen.z<0 || zbuffer[P.x + P.y*m_Width]>frag_depth) continue;
			bool discard = shader.fragment(bc_clip, color);
			if (!discard) {
				zbuffer[P.x + P.y*m_Width] = frag_depth;
				SetPixel(uint32_t(P.x), uint32_t(P.y), Color(color.bgra[2], color.bgra[1], color.bgra[0], color.bgra[3]));
			}
		}
	}
}




 
void Rasterizer::Draw_Rectangle(int left, int top, int right, int bottom, Color&color)
{
	for (int x = left; x <= right; ++x) 
	for (int y = top; y <= bottom; ++y)
		SetPixel((unsigned int)x, (unsigned int)y, color);
}

 
void
Rasterizer::DrawLine(const Color &color1, float x1, float y1,
                     const Color &color2, float x2, float y2)
{
	float xdiff = (x2 - x1);
	float ydiff = (y2 - y1);

	if(xdiff == 0.0f && ydiff == 0.0f) {
		SetPixel(x1, y1, color1);
		return;
	}

	if(fabs(xdiff) > fabs(ydiff)) {
		float xmin, xmax;

		// set xmin to the lower x value given
		// and xmax to the higher value
		if(x1 < x2) {
			xmin = x1;
			xmax = x2;
		} else {
			xmin = x2;
			xmax = x1;
		}

		// draw line in terms of y slope
		float slope = ydiff / xdiff;
		for(float x = xmin; x <= xmax; x += 1.0f) {
			float y = y1 + ((x - x1) * slope);
			Color color = color1 + ((color2 - color1) * ((x - x1) / xdiff));
			SetPixel(x, y, color);
		}
	} else {
		float ymin, ymax;

		// set ymin to the lower y value given
		// and ymax to the higher value
		if(y1 < y2) {
			ymin = y1;
			ymax = y2;
		} else {
			ymin = y2;
			ymax = y1;
		}

		// draw line in terms of x slope
		float slope = xdiff / ydiff;
		for(float y = ymin; y <= ymax; y += 1.0f) {
			float x = x1 + ((y - y1) * slope);
			Color color = color1 + ((color2 - color1) * ((y - y1) / ydiff));
			SetPixel(x, y, color);
		}
	}



}


