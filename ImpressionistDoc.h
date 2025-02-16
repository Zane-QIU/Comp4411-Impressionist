// 
// impressionistDoc.h
//
// header file for Doc 
//

#ifndef ImpressionistDoc_h
#define ImpressionistDoc_h

#include "impressionist.h"
#include "bitmap.h"

class ImpressionistUI;

class Point;

class StrokeDirection;

class ThreeDTree;

class Paintly;

class ImpressionistDoc 
{
public:
	ImpressionistDoc();

	~ImpressionistDoc();

	void	setUI(ImpressionistUI* ui);		// Assign the UI to use

	int		loadImage(char *iname);			// called by the UI to load image
	int		saveImage(char *iname);			// called by the UI to save image
	int		edgeImage(char *iname);
	int		anotherImage(char *iname);
	int		dissolve(char *iname);
	int     clearCanvas();                  	// called by the UI to clear the drawing canvas
	void	setBrushType(int type);				// called by the UI to set the brushType
	void	setStrokeType(int type);			// called by the UI to set the strokeType
	int		getSize();							// get the brush size
	void	setSize(int size);					// set the brush size
	int		getWidth();							// get the brush width
	void	setWidth(int width);				// set the brush width
	int		getAngle();							// get the brush angle
	void	setAngle(int angle);				// set the brush anlge
	double	getAlpha();							// get the brush alpha
	void	setAlpha(double alpha);				// set the brush alpha
	int		getEdgeClipping();
	void	setEdgeClipping(int value);
	int		getEdgeThreashold();				// get the edge threashold
	void	setEdgeThreashold(int threashold);	// set the edge threashold
	int		getBlurSharpLevel();
	void	setBlurSharpLevel(int level);

	char*	getImageName();						// get the current image name
	
	void	setMousePos(Point source);
	Point	getMousePos();
	
	void 	swap();
	void	loadForUndo();
	void	refresh();

	void	edgeDetection();

	// void	cropImage();

// Attributes
public:
	// Dimensions of original window.
	int				m_nWidth, 
					m_nHeight;
	// Dimensions of the paint window.
	int				m_nPaintWidth, 
					m_nPaintHeight;	
	// Bitmaps for original image and painting.
	unsigned char*	m_ucOriginal;
	unsigned char*	m_ucBitmap;
	unsigned char*	m_ucPainting;
	unsigned char*	m_ucFadePainting;
	unsigned char*	m_ucTemp;
	unsigned char*	m_ucEdge;
	unsigned char*	m_ucAnotherImage;

	// mouse coordinates
	Point			m_pMousePos = Point(0, 0);

	// The current active brush.
	ImpBrush*					m_pCurrentBrush;
	// The stroke direction object.
	StrokeDirection*			m_pStrokeDirection;
	// The stroke direction type.
	int							m_nStrokeType;

	Paintly*					m_pPaintly;

	// Size of the brush.
	int m_nSize;							

	ImpressionistUI*	m_pUI;

	int 	m_nPaintlyTheshold;
	double 	m_nPaintlyCurvature;
	double	m_nPaintlyBlur;
	double	m_nPaintlyGridSize;
	int		m_nPaintlyMinStrokeL;
	int		m_nPaintlyMaxStrokeL;
	double	m_nPaintlyAlpha;
	int		m_nPaintlyLayer;
	int		m_nPaintlyR0Level;

// Operations
public:
	// Get the color of the original picture at the specified coord
	GLubyte* GetOriginalPixel( int x, int y );   
	// Get the color of the original picture at the specified point	
	GLubyte* GetOriginalPixel( const Point p );  

private:
	char			m_imageName[256];

};

extern void MessageBox(char *message);

#endif
