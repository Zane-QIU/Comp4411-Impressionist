//
// paintview.cpp
//
// The code maintaining the painting view of the input images
//

#include <iostream>
#include "impressionist.h"
#include "impressionistDoc.h"
#include "impressionistUI.h"
#include "paintview.h"
#include "ImpBrush.h"
#include "StrokeDirection.h"
#include <math.h>
#include <iostream>


#define LEFT_MOUSE_DOWN		1
#define LEFT_MOUSE_DRAG		2
#define LEFT_MOUSE_UP		3
#define RIGHT_MOUSE_DOWN	4
#define RIGHT_MOUSE_DRAG	5
#define RIGHT_MOUSE_UP		6


#ifndef WIN32
#define min(a, b)	( ( (a)<(b) ) ? (a) : (b) )
#define max(a, b)	( ( (a)>(b) ) ? (a) : (b) )
#endif

static int		eventToDo;
static int		isAnEvent=0;
static Point	coord;
static int a;

LinkedList::LinkedList(int w, int h)
{
	photo = new char[w*h*3];
	next = nullptr;
}

LinkedList::~LinkedList()
{
}

static int		autopaint = 0;
static int		autopaintspacing = 0;
static int		isFade = 0;

PaintView::PaintView(int			x, 
					 int			y, 
					 int			w, 
					 int			h, 
					 const char*	l)
						: Fl_Gl_Window(x,y,w,h,l)
{
	m_nWindowWidth	= w;
	m_nWindowHeight	= h;
	m_SavedPhoto = nullptr;

}


void PaintView::draw()
{
	#ifndef MESA
	// To avoid flicker on some machines.
	glDrawBuffer(GL_FRONT_AND_BACK);
	#endif // !MESA

	if(!valid())
	{

		glClearColor(0.7f, 0.7f, 0.7f, 1.0);

		// We're only using 2-D, so turn off depth 
		glDisable( GL_DEPTH_TEST );

		ortho();

		glClear( GL_COLOR_BUFFER_BIT );
	}

	Point scrollpos;// = GetScrollPosition();
	scrollpos.x = 0;
	scrollpos.y	= 0;

	m_nWindowWidth	= w();
	m_nWindowHeight	= h();

	int drawWidth, drawHeight;
	drawWidth = min( m_nWindowWidth, m_pDoc->m_nPaintWidth );
	drawHeight = min( m_nWindowHeight, m_pDoc->m_nPaintHeight );

	int startrow = m_pDoc->m_nPaintHeight - (scrollpos.y + drawHeight);
	if ( startrow < 0 ) startrow = 0;

	m_pPaintBitstart = m_pDoc->m_ucPainting + 
		3 * ((m_pDoc->m_nPaintWidth * startrow) + scrollpos.x);

	m_nDrawWidth	= drawWidth;
	m_nDrawHeight	= drawHeight;

	m_nStartRow		= startrow;
	m_nEndRow		= startrow + drawHeight;
	m_nStartCol		= scrollpos.x;
	m_nEndCol		= m_nStartCol + drawWidth;

	if ( m_pDoc->m_ucPainting && !isAnEvent) 
	{
		RestoreContent();

	}
	
	glEnable( GL_BLEND );
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (autopaint == 1)
	{
		int x = 0, y = 0;
		// glReadBuffer(GL_BACK);
		glDrawBuffer(GL_FRONT_AND_BACK);

		glPixelStorei( GL_PACK_ALIGNMENT, 1 );
		glPixelStorei( GL_PACK_ROW_LENGTH, m_pDoc->m_nPaintWidth );

		int count = 0, temp = 0, temp_size = m_pDoc->getSize();
		for (int y = 0; y < m_nDrawHeight; y += autopaintspacing)
			for(int x = 0; x < m_nDrawWidth; x += autopaintspacing)
				++count;

		Point* xy = new Point [count];
		for (int y = 0; y < m_nDrawHeight; y += autopaintspacing)
			for(int x = 0; x < m_nDrawWidth; x += autopaintspacing)
				xy[temp++] = Point(x, y);

		Point temp_p;
		for (int i = 0; i < count; i++)
		{
			temp = rand() % (count);
			temp_p = xy[temp];
			xy[temp] = xy[i];
			xy[i] = temp_p;
		}

		glTranslated(0, m_nWindowHeight - m_nDrawHeight, 0);
		for(int i = 0; i < count; ++i)
		{
			if (m_pDoc->m_pUI->getRandomSize())
			{
				temp = temp_size;
				if (temp != 1)
				{
					temp = temp / 2 + temp % 2;
					temp = rand() % (temp + 1) + (temp_size - temp);
				}
				m_pDoc->setSize(temp);
			}
			m_pDoc->m_pCurrentBrush->BrushBegin( xy[i], xy[i] );
		}
		m_pDoc->setSize(temp_size);
		glTranslated(0, -(m_nWindowHeight - m_nDrawHeight), 0);

		delete [] xy;

		autopaint = 0;
		m_pDoc->m_pCurrentBrush->BrushEnd(m_pAutoPaintPoint, m_pAutoPaintPoint);

		SaveCurrentContent();
		RestoreContent();

		
		if (m_pDoc->m_ucFadePainting)
			memcpy(m_pDoc->m_ucFadePainting, m_pPaintBitstart, m_nDrawWidth*m_nDrawHeight*3);
	}

	int originalSize = m_pDoc->getSize();

	if ( m_pDoc->m_ucPainting && isAnEvent) 
	{
		if (coord.x < 0) coord.x = 0;
		if (coord.y < 0) coord.y = 0;
		if (coord.x >= m_nDrawWidth) coord.x = m_nDrawWidth - 1;
		if (coord.y >= m_nDrawHeight) coord.y = m_nDrawHeight - 1;

		Point source( coord.x + m_nStartCol, m_nEndRow - coord.y );
		Point target( coord.x, m_nWindowHeight - coord.y );

		// This is the event handler
		switch (eventToDo) 
		{
		case LEFT_MOUSE_DOWN:
			if (m_pDoc->m_ucFadePainting && isFade)
			{
				glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
				glPixelStorei( GL_UNPACK_ROW_LENGTH, m_pDoc->m_nPaintWidth );
				glDrawPixels( m_nDrawWidth, 
							m_nDrawHeight, 
							GL_RGB, 
							GL_UNSIGNED_BYTE, 
							m_pDoc->m_ucFadePainting);

				m_pDoc->m_pCurrentBrush->BrushBegin( source, target );

				glPixelStorei( GL_PACK_ALIGNMENT, 1 );
				glPixelStorei( GL_PACK_ROW_LENGTH, m_pDoc->m_nPaintWidth );
				glReadPixels( 0, 
								m_nWindowHeight - m_nDrawHeight, 
								m_nDrawWidth, 
								m_nDrawHeight, 
								GL_RGB, 
								GL_UNSIGNED_BYTE, 
								m_pDoc->m_ucFadePainting );
			}
			else
				m_pDoc->m_pCurrentBrush->BrushBegin( source, target );
			break;
		case LEFT_MOUSE_DRAG:
			if (m_pDoc->m_ucFadePainting && isFade)
			{
				glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
				glPixelStorei( GL_UNPACK_ROW_LENGTH, m_pDoc->m_nPaintWidth );
				glDrawPixels( m_nDrawWidth, 
							m_nDrawHeight, 
							GL_RGB, 
							GL_UNSIGNED_BYTE, 
							m_pDoc->m_ucFadePainting);

				m_pDoc->m_pCurrentBrush->BrushBegin( source, target );

				glPixelStorei( GL_PACK_ALIGNMENT, 1 );
				glPixelStorei( GL_PACK_ROW_LENGTH, m_pDoc->m_nPaintWidth );
				glReadPixels( 0, 
							m_nWindowHeight - m_nDrawHeight, 
							m_nDrawWidth, 
							m_nDrawHeight, 
							GL_RGB, 
							GL_UNSIGNED_BYTE, 
							m_pDoc->m_ucFadePainting );
			}
			else
				m_pDoc->m_pCurrentBrush->BrushMove( source, target );
			break;
		case LEFT_MOUSE_UP:
			m_pDoc->m_pCurrentBrush->BrushEnd( source, target );
			m_pDoc->m_pStrokeDirection->resetLastMousePos();

			SaveCurrentContent();
			RestoreContent();
			break;
		default:
			printf("Unknown event!!\n");		
			break;
		}
	}

	glDisable(GL_BLEND);

	if (isFade)
	{
		m_pDoc->setSize(1);

		glTranslated(0, (m_nWindowHeight - m_nDrawHeight), 0);

		ImpBrush* IBtemp = m_pDoc->m_pCurrentBrush;
		m_pDoc->setBrushType(0);

		for (int y = 0; y < m_nDrawHeight ; ++y)
			for(int x = 0; x < m_nDrawWidth; ++x)
				m_pDoc->m_pCurrentBrush->BrushBegin( Point(x, y), Point(x, y) );

		m_pDoc->m_pCurrentBrush = IBtemp;

		glEnable( GL_BLEND );
		glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);
		GLubyte color[4];
		glPointSize(1);
		for (int y = 0; y < m_nDrawHeight ; ++y)
			for(int x = 0; x < m_nDrawWidth; ++x)
			{
				memcpy ( color, m_pDoc->m_ucFadePainting  + 3 * (y*m_nDrawWidth + x), 3 );
				memset ( color+3, m_pDoc->m_pUI->getFadeAlpha()*255, 1);
				glColor4ubv( color );
				glBegin( GL_POINTS );
					glVertex2d( x, y );
				glEnd();
			}

		glDisable(GL_BLEND);

		glTranslated(0, -(m_nWindowHeight - m_nDrawHeight), 0);

		m_pDoc->setSize(originalSize);

		SaveCurrentContent();
	}
	glDisable(GL_BLEND);

	

	if ( m_pDoc->m_ucPainting && isAnEvent) 
	{
		// Clear it after processing.
		isAnEvent	= 0;
		
		Point source( coord.x + m_nStartCol, m_nEndRow - coord.y );
		Point target( coord.x, m_nWindowHeight - coord.y );

		
		switch (eventToDo) 
		{
		case RIGHT_MOUSE_DOWN:
			if (m_pDoc->m_nStrokeType == STROKE_SLIDER)
			{
				RestoreContent();
				SaveCurrentContent();
				int offset = m_pDoc->m_pUI->m_mainWindow->h() - m_pDoc->m_nPaintHeight - 25;
				m_pDoc->m_pStrokeDirection->StrokeBegin(source, offset);
			}
			break;
		case RIGHT_MOUSE_DRAG:
			if (m_pDoc->m_nStrokeType == STROKE_SLIDER)
			{
				RestoreContent();
				int offset = m_pDoc->m_pUI->m_mainWindow->h() - m_pDoc->m_nPaintHeight - 25;
				m_pDoc->m_pStrokeDirection->StrokeMove(source, offset);
			}
			break;
		case RIGHT_MOUSE_UP:
			if (m_pDoc->m_nStrokeType == STROKE_SLIDER)
			{
				RestoreContent();
				m_pDoc->m_pStrokeDirection->StrokeEnd(source);

				m_pDoc->setAngle(m_pDoc->m_pStrokeDirection->getAngle(m_pDoc, source, target, m_pDoc->m_nStrokeType));
			}
			break;
		}
	}

	glFlush();

	#ifndef MESA
	// To avoid flicker on some machines.
	glDrawBuffer(GL_BACK);
	#endif // !MESA
}


int PaintView::handle(int event)
{
	switch(event)
	{
	case FL_ENTER:
	    redraw();
		break;
	case FL_PUSH:
		SaveForUndo();
		coord.x = Fl::event_x();
		coord.y = Fl::event_y();
		if (Fl::event_button()>1)
			eventToDo=RIGHT_MOUSE_DOWN;
		else
			eventToDo=LEFT_MOUSE_DOWN;
		isAnEvent=1;
		redraw();
		break;
	case FL_DRAG:
		coord.x = Fl::event_x();
		coord.y = Fl::event_y();
		if (Fl::event_button()>1)
			eventToDo=RIGHT_MOUSE_DRAG;
		else
			eventToDo=LEFT_MOUSE_DRAG;
		isAnEvent=1;
		redraw();
		break;
	case FL_RELEASE:
		coord.x = Fl::event_x();
		coord.y = Fl::event_y();
		if (Fl::event_button()>1)
			eventToDo=RIGHT_MOUSE_UP;
		else
			eventToDo=LEFT_MOUSE_UP;
		isAnEvent=1;
		redraw();
		break;
	case FL_MOVE:
		coord.x = Fl::event_x();
		coord.y = Fl::event_y();
		break;
	default:
		return 0;
		break;

	}


	m_pDoc->setMousePos(Point(coord.x, coord.y));

	return 1;
}

void PaintView::refresh()
{
	redraw();
}

void PaintView::resizeWindow(int width, int height)
{
	resize(x(), y(), width, height);
}

void PaintView::SaveCurrentContent()
{
	// Tell openGL to read from the front buffer when capturing
	// out paint strokes
	glReadBuffer(GL_FRONT);

	glPixelStorei( GL_PACK_ALIGNMENT, 1 );
	glPixelStorei( GL_PACK_ROW_LENGTH, m_pDoc->m_nPaintWidth );
	
	glReadPixels( 0, 
				  m_nWindowHeight - m_nDrawHeight, 
				  m_nDrawWidth, 
				  m_nDrawHeight, 
				  GL_RGB, 
				  GL_UNSIGNED_BYTE, 
				  m_pPaintBitstart );
}


void PaintView::RestoreContent()
{
	glDrawBuffer(GL_BACK);

	glClear( GL_COLOR_BUFFER_BIT );

	glRasterPos2i( 0, m_nWindowHeight - m_nDrawHeight );
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	glPixelStorei( GL_UNPACK_ROW_LENGTH, m_pDoc->m_nPaintWidth );
	glDrawPixels( m_nDrawWidth, 
				  m_nDrawHeight, 
				  GL_RGB, 
				  GL_UNSIGNED_BYTE, 
				  m_pPaintBitstart);

//	glDrawBuffer(GL_FRONT);
}

void PaintView::CreateSavedPhoto()
{
	m_SavedPhoto = new LinkedList(m_pDoc->m_nPaintWidth, m_pDoc->m_nPaintHeight);
	memset(m_SavedPhoto->photo, 0, m_pDoc->m_nPaintWidth*m_pDoc->m_nPaintHeight*3);
}

void PaintView::SaveForUndo()
{
	if (m_SavedPhoto != NULL)
	{
		LinkedList* temp = m_SavedPhoto;
		while (temp->next != nullptr)
			temp = temp->next;
		temp->next = new LinkedList(m_nDrawWidth, m_nDrawHeight);
		memcpy(temp->next->photo, m_pPaintBitstart, m_nDrawWidth*m_nDrawHeight*3);
	}
}

void PaintView::LoadForUndo()
{
	if (m_SavedPhoto !=nullptr)
	{
		if (m_SavedPhoto->next != nullptr)
		{
			LinkedList* temp = m_SavedPhoto;
			while (temp->next->next != nullptr)
				temp = temp->next;
			memcpy(m_pPaintBitstart, temp->next->photo, m_nDrawWidth*m_nDrawHeight*3);
			delete temp->next;
			temp->next = nullptr;
			redraw();
		}
	}
}

void PaintView::AutoPaint(int spacing)
{
	autopaintspacing = spacing;
	autopaint = 1;
	redraw();
}

//this is dissovle
void PaintView::draw_fade(int old_width, int old_height, unsigned char* old_painting)
{
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	glPixelStorei( GL_UNPACK_ROW_LENGTH, m_pDoc->m_nPaintWidth );
	glDrawPixels(  m_pDoc->m_nPaintWidth, 
				   m_pDoc->m_nPaintHeight, 
				  GL_RGB, 
				  GL_UNSIGNED_BYTE, 
				   m_pDoc->m_ucPainting);
				
	glFlush();

	if (isFade)
		memcpy(old_painting, m_pDoc->m_ucFadePainting, old_width*old_height*3);

	if (old_painting)
	{
		GLubyte color[3];
		for(int y = 0; y < old_height; ++y)
			for(int x = 0; x < old_width; ++x)
			{
				memcpy (color, old_painting+3*(x+y*old_width), 3);
				glColor3ubv(color);
				glBegin(GL_POLYGON);
					glVertex2f(x, y);
					glVertex2f(x, y+1);
					glVertex2f(x+1, y+1);
					glVertex2f(x+1, y);
				glEnd();
			}

		glFlush();
	}

	glPixelStorei( GL_PACK_ALIGNMENT, 1 );
	glPixelStorei( GL_PACK_ROW_LENGTH,  m_pDoc->m_nPaintWidth );
	glReadPixels( 0, 
				  0, 
				   m_pDoc->m_nPaintWidth, 
				   m_pDoc->m_nPaintHeight, 
				  GL_RGB, 
				  GL_UNSIGNED_BYTE, 
				   m_pDoc->m_ucPainting );	
}

void PaintView::fade_in()
{
	if (!m_pDoc->m_ucFadePainting)
	{
		glReadBuffer(GL_FRONT);
		m_pDoc->m_ucFadePainting = new unsigned char [m_nDrawWidth*m_nDrawHeight*3];
		memcpy (m_pDoc->m_ucFadePainting, m_pDoc->m_ucPainting, m_nDrawWidth*m_nDrawHeight*3);
	}
	if (!isFade)
		memcpy (m_pDoc->m_ucFadePainting, m_pDoc->m_ucPainting, m_nDrawWidth*m_nDrawHeight*3);

	isFade = 1;
	if (m_pDoc->m_pUI->getFadeAlpha() == 0)
	{
		memcpy (m_pDoc->m_ucPainting, m_pDoc->m_ucFadePainting, m_nDrawWidth*m_nDrawHeight*3);
		isFade = 0;
	}
	redraw();
}

int PaintView::getIsFade()
{
	return isFade;
}

void PaintView::setIsFade(int isfade)
{
	isFade = isfade;
}
