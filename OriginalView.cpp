//
// originalview.cpp
//
// The code maintaining the original view of the input images
//

#include "impressionist.h"
#include "impressionistDoc.h"
#include "originalview.h"

#ifndef WIN32
#define min(a, b)	( ( (a)<(b) ) ? (a) : (b) )
#endif

OriginalView::OriginalView(int			x, 
						   int			y, 
						   int			w, 
						   int			h, 
						   const char*	l)
							: Fl_Gl_Window(x,y,w,h,l)
{
	m_nWindowWidth	= w;
	m_nWindowHeight	= h;

}

void OriginalView::draw()
{
	if(!valid())
	{
		glClearColor(0.7f, 0.7f, 0.7f, 1.0);

		// We're only using 2-D, so turn off depth 
		glDisable(GL_DEPTH_TEST);

		// Tell openGL to read from the front buffer when capturing
		// out paint strokes 
		glReadBuffer(GL_FRONT);
		ortho();

	}

	glClear(GL_COLOR_BUFFER_BIT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (m_pDoc->m_ucBitmap) 
	{

		// note that both OpenGL pixel storage and the Windows BMP format
		// store pixels left-to-right, BOTTOM-to-TOP!!  thus all the fiddling
		// around with startrow.

		m_nWindowWidth=w();
		m_nWindowHeight=h();

		int drawWidth, drawHeight;
		GLvoid* bitstart;

		// we are not using a scrollable window, so ignore it
		Point scrollpos;	// = GetScrollPosition();
		scrollpos.x=scrollpos.y=0;

		drawWidth	= min(m_nWindowWidth, m_pDoc->m_nWidth);
		drawHeight	= min(m_nWindowHeight, m_pDoc->m_nHeight);

		int	startrow = m_pDoc->m_nHeight - (scrollpos.y + drawHeight);
		if (startrow < 0) 
			startrow = 0;


		bitstart = m_pDoc->m_ucOriginal + 3 * ((m_pDoc->m_nWidth * startrow) + scrollpos.x);

		// just copy image to GLwindow conceptually
		glRasterPos2i(0, m_nWindowHeight - drawHeight);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, m_pDoc->m_nWidth);
		glDrawBuffer(GL_BACK);
		glDrawPixels(drawWidth, drawHeight, GL_RGB, GL_UNSIGNED_BYTE, bitstart);

		Point p = m_pDoc->getMousePos();
		p.y = m_pDoc->m_nHeight-p.y;
		if (p.x >= 0 && p.x < drawWidth && p.y >= 0 && p.y < drawHeight)
		{
			int red_dot_size = 5; //side length
			glColor4f(1, 0, 0, 1);
			glTranslated(0, m_nWindowHeight - drawHeight, 0);
			glBegin(GL_QUADS);
				glVertex2d(p.x, p.y);
				glVertex2d(p.x + red_dot_size, p.y);
				glVertex2d(p.x + red_dot_size, p.y+red_dot_size);
				glVertex2d(p.x , p.y+red_dot_size);
			glEnd();
			glTranslated(0, -(m_nWindowHeight - drawHeight), 0);
		}
	}
			
	glEnd();
	//glFlush();
}

void OriginalView::refresh()
{
	redraw();
}

void OriginalView::resizeWindow(int	width, 
								int	height)
{
	resize(x(), y(), width, height);
}


int OriginalView::isSameSize(int width, int height)
{
	int drawWidth	= min( m_nWindowWidth, m_pDoc->m_nWidth );
	int drawHeight	= min( m_nWindowHeight, m_pDoc->m_nHeight );
	if (width == drawWidth && height == drawHeight)
		return 1;
	return 0;
}

int OriginalView::draw_fade(int old_width, int old_height, int new_width, int new_height, unsigned char* data, unsigned char* old_original)
{
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	glPixelStorei( GL_UNPACK_ROW_LENGTH, m_pDoc->m_nWidth );
	glDrawPixels( m_pDoc->m_nWidth, 
				  m_pDoc->m_nHeight, 
				  GL_RGB, 
				  GL_UNSIGNED_BYTE, 
				  m_pDoc->m_ucBitmap);

	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	glPixelStorei( GL_UNPACK_ROW_LENGTH, old_width );
	glDrawPixels( old_width, 
				  old_height, 
				  GL_RGB, 
				  GL_UNSIGNED_BYTE, 
				  old_original);

	unsigned char* new_data = new unsigned char [m_pDoc->m_nWidth*m_pDoc->m_nHeight*4];
	for(int x = 0; x < new_width*new_height; ++x)
	{
		memcpy (new_data + x*4 , data + x*3, 3);
		memset (new_data + x*4 + 3, 0.5 * 255, 1);
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	glPixelStorei( GL_UNPACK_ROW_LENGTH, new_width );
	glDrawPixels( new_width, 
				  new_height, 
				  GL_RGBA, 
				  GL_UNSIGNED_BYTE, 
				  new_data);
	glDisable(GL_BLEND);

	glReadBuffer(GL_FRONT_AND_BACK);
	glPixelStorei( GL_PACK_ALIGNMENT, 1 );
	glPixelStorei( GL_PACK_ROW_LENGTH, m_pDoc->m_nWidth );
	glReadPixels( 0, 
				  0, 
				  m_pDoc->m_nWidth, 
				  m_pDoc->m_nHeight, 
				  GL_RGB, 
				  GL_UNSIGNED_BYTE, 
				  m_pDoc->m_ucBitmap );

	return 1;
}