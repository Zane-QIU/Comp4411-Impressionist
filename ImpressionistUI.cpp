//
// impressionistUI.cpp
//
// The user interface part for the program.
//

#include <FL/fl_ask.h>

#include <math.h>

#include "ImpressionistUI.h"
#include "ImpressionistDoc.h"

#include "StrokeDirection.h"

#include "Paintly.h"

/*
//------------------------------ Widget Examples -------------------------------------------------
Here is some example code for all of the widgets that you may need to add to the
project.  You can copy and paste these into your code and then change them to
make them look how you want.  Descriptions for all of the widgets here can be found
in links on the fltk help session page.

//---------Window/Dialog and Menubar-----------------------------------

	//----To install a window--------------------------
	Fl_Window* myWindow = new Fl_Window(600, 300, "MyWindow");
		myWindow->user_data((void*)(this));	// record self to be used by static callback functions

		// install menu bar
		myMenubar = new Fl_Menu_Bar(0, 0, 600, 25);
		Fl_Menu_Item ImpressionistUI::myMenuItems[] = {
			{ "&File",		0, 0, 0, FL_SUBMENU },
				{ "&Load...",	FL_ALT + 'l', (Fl_Callback *)ImpressionistUI::cb_load },
				{ "&Save...",	FL_ALT + 's', (Fl_Callback *)ImpressionistUI::cb_save }.
				{ "&Quit",			FL_ALT + 'q', (Fl_Callback *)ImpressionistUI::cb_exit },
				{ 0 },
			{ "&Edit",		0, 0, 0, FL_SUBMENU },
				{ "&Copy",FL_ALT + 'c', (Fl_Callback *)ImpressionistUI::cb_copy, (void *)COPY },
				{ "&Cut",	FL_ALT + 'x', (Fl_Callback *)ImpressionistUI::cb_cut, (void *)CUT },
				{ "&Paste",	FL_ALT + 'v', (Fl_Callback *)ImpressionistUI::cb_paste, (void *)PASTE },
				{ 0 },
			{ "&Help",		0, 0, 0, FL_SUBMENU },
				{ "&About",	FL_ALT + 'a', (Fl_Callback *)ImpressionistUI::cb_about },
				{ 0 },
			{ 0 }
		};
		myMenubar->menu(myMenuItems);
	m_mainWindow->end();

	//----The window callback--------------------------
	// One of the callbacks
	void ImpressionistUI::cb_load(Fl_Menu_* o, void* v)
	{
		ImpressionistDoc *pDoc=whoami(o)->getDocument();

		char* newfile = fl_file_chooser("Open File?", "*.bmp", pDoc->getImageName() );
		if (newfile != NULL) {
			pDoc->loadImage(newfile);
		}
	}


//------------Slider---------------------------------------

	//----To install a slider--------------------------
	Fl_Value_Slider * mySlider = new Fl_Value_Slider(10, 80, 300, 20, "My Value");
	mySlider->user_data((void*)(this));	// record self to be used by static callback functions
	mySlider->type(FL_HOR_NICE_SLIDER);
	mySlider->labelfont(FL_COURIER);
	mySlider->labelsize(12);
	mySlider->minimum(1);
	mySlider->maximum(40);
	mySlider->step(1);
	mySlider->value(m_nMyValue);
	mySlider->align(FL_ALIGN_RIGHT);
	mySlider->callback(cb_MyValueSlides);

	//----The slider callback--------------------------
	void ImpressionistUI::cb_MyValueSlides(Fl_Widget* o, void* v)
	{
		((ImpressionistUI*)(o->user_data()))->m_nMyValue=int( ((Fl_Slider *)o)->value() ) ;
	}


//------------Choice---------------------------------------

	//----To install a choice--------------------------
	Fl_Choice * myChoice = new Fl_Choice(50,10,150,25,"&myChoiceLabel");
	myChoice->user_data((void*)(this));	 // record self to be used by static callback functions
	Fl_Menu_Item ImpressionistUI::myChoiceMenu[3+1] = {
	  {"one",FL_ALT+'p', (Fl_Callback *)ImpressionistUI::cb_myChoice, (void *)ONE},
	  {"two",FL_ALT+'l', (Fl_Callback *)ImpressionistUI::cb_myChoice, (void *)TWO},
	  {"three",FL_ALT+'c', (Fl_Callback *)ImpressionistUI::cb_myChoice, (void *)THREE},
	  {0}
	};
	myChoice->menu(myChoiceMenu);
	myChoice->callback(cb_myChoice);

	//-----The choice callback-------------------------
	void ImpressionistUI::cb_myChoice(Fl_Widget* o, void* v)
	{
		ImpressionistUI* pUI=((ImpressionistUI *)(o->user_data()));
		ImpressionistDoc* pDoc=pUI->getDocument();

		int type=(int)v;

		pDoc->setMyType(type);
	}


//------------Button---------------------------------------

	//---To install a button---------------------------
	Fl_Button* myButton = new Fl_Button(330,220,50,20,"&myButtonLabel");
	myButton->user_data((void*)(this));   // record self to be used by static callback functions
	myButton->callback(cb_myButton);

	//---The button callback---------------------------
	void ImpressionistUI::cb_myButton(Fl_Widget* o, void* v)
	{
		ImpressionistUI* pUI=((ImpressionistUI*)(o->user_data()));
		ImpressionistDoc* pDoc = pUI->getDocument();
		pDoc->startPainting();
	}


//---------Light Button------------------------------------

	//---To install a light button---------------------
	Fl_Light_Button* myLightButton = new Fl_Light_Button(240,10,150,25,"&myLightButtonLabel");
	myLightButton->user_data((void*)(this));   // record self to be used by static callback functions
	myLightButton->callback(cb_myLightButton);

	//---The light button callback---------------------
	void ImpressionistUI::cb_myLightButton(Fl_Widget* o, void* v)
	{
		ImpressionistUI *pUI=((ImpressionistUI*)(o->user_data()));

		if (pUI->myBool==TRUE) pUI->myBool=FALSE;
		else pUI->myBool=TRUE;
	}

//----------Int Input--------------------------------------

	//---To install an int input-----------------------
	Fl_Int_Input* myInput = new Fl_Int_Input(200, 50, 5, 5, "&My Input");
	myInput->user_data((void*)(this));   // record self to be used by static callback functions
	myInput->callback(cb_myInput);

	//---The int input callback------------------------
	void ImpressionistUI::cb_myInput(Fl_Widget* o, void* v)
	{
		((ImpressionistUI*)(o->user_data()))->m_nMyInputValue=int( ((Fl_Int_Input *)o)->value() );
	}

//------------------------------------------------------------------------------------------------
*/

enum
{
	PAINTLY_IMPRESSIONIST = 0,
	PAINTLY_EXPRESSIONIST,
	PAINTLY_COLOR_WASH,
	PAINTLY_POINTILLIST,
	PAINTLY_CUSTOMIZE
};

enum
{
	PAINTLY_CURVE = 0,
	PAINTLY_BSPLINE,
	PAINTLY_CIRCLE,
	PAINTLY_CLIP,
	PAINTLY_LINE
};

//------------------------------------- Help Functions --------------------------------------------

//------------------------------------------------------------
// This returns the UI, given the menu item.  It provides a
// link from the menu items to the UI
//------------------------------------------------------------
ImpressionistUI *ImpressionistUI::whoami(Fl_Menu_ *o)
{
	return ((ImpressionistUI *)(o->parent()->user_data()));
}

//--------------------------------- Callback Functions --------------------------------------------

//------------------------------------------------------------------
// Brings up a file chooser and then loads the chosen image
// This is called by the UI when the load image menu item is chosen
//------------------------------------------------------------------
void ImpressionistUI::cb_load_image(Fl_Menu_ *o, void *v)
{
	ImpressionistDoc *pDoc = whoami(o)->getDocument();

	char *newfile = fl_file_chooser("Open File?", "*.bmp", pDoc->getImageName());
	if (newfile != NULL)
	{
		pDoc->loadImage(newfile);
	}
}

//------------------------------------------------------------------
// Brings up a file chooser and then saves the painted image
// This is called by the UI when the save image menu item is chosen
//------------------------------------------------------------------
void ImpressionistUI::cb_save_image(Fl_Menu_ *o, void *v)
{
	ImpressionistDoc *pDoc = whoami(o)->getDocument();

	char *newfile = fl_file_chooser("Save File?", "*.bmp", "save.bmp");
	if (newfile != NULL)
	{
		pDoc->saveImage(newfile);
	}
}

//-------------------------------------------------------------
// Brings up the paint dialog
// This is called by the UI when the brushes menu item
// is chosen
//-------------------------------------------------------------
void ImpressionistUI::cb_brushes(Fl_Menu_ *o, void *v)
{
	whoami(o)->m_brushDialog->show();
}

void ImpressionistUI::cb_colors(Fl_Menu_ *o, void *v)
{
	whoami(o)->m_colorDialog->show();
}

void ImpressionistUI::cb_paintly(Fl_Menu_ *o, void *v)
{
	ImpressionistDoc *pDoc = whoami(o)->getDocument();

	whoami(o)->setPaintly();

	pDoc->m_pPaintly->DrawPaintly();
	// whoami(o)->m_paintlyDialog->show();
}

void ImpressionistUI::cb_paintly_style(Fl_Widget *o, void *v)
{
}

void ImpressionistUI::cb_paintly_stroke(Fl_Widget *o, void *v)
{
}

void ImpressionistUI::cb_paintly_run(Fl_Widget *o, void *v)
{
}

void ImpressionistUI::cb_fade(Fl_Menu_ *o, void *v)
{
	whoami(o)->m_fadeDialog->show();
}

void ImpressionistUI::cb_fade_in(Fl_Widget *o, void *v)
{
	((ImpressionistUI *)(o->user_data()))->m_nfadeAlpha = double(((Fl_Slider *)o)->value());

	ImpressionistUI *pUI = ((ImpressionistUI *)(o->user_data()));
	ImpressionistDoc *pDoc = pUI->getDocument();

	pDoc->m_pUI->m_paintView->fade_in();
}

//------------------------------------------------------------
// Clears the paintview canvas.
// Called by the UI when the clear canvas menu item is chosen
//------------------------------------------------------------
void ImpressionistUI::cb_clear_canvas(Fl_Menu_ *o, void *v)
{
	ImpressionistDoc *pDoc = whoami(o)->getDocument();

	pDoc->clearCanvas();
}

void ImpressionistUI::cb_edge_image(Fl_Menu_ *o, void *v)
{
	ImpressionistDoc *pDoc = whoami(o)->getDocument();

	char *newfile = fl_file_chooser("Open File?", "*.bmp", pDoc->getImageName());
	if (newfile != NULL)
	{
		pDoc->edgeImage(newfile);
	}
}

void ImpressionistUI::cb_another_image(Fl_Menu_ *o, void *v)
{
	ImpressionistDoc *pDoc = whoami(o)->getDocument();

	char *newfile = fl_file_chooser("Open File?", "*.bmp", pDoc->getImageName());
	if (newfile != NULL)
	{
		pDoc->anotherImage(newfile);
	}
}

//------------------------------------------------------------
// Causes the Impressionist program to exit
// Called by the UI when the quit menu item is chosen
//------------------------------------------------------------
void ImpressionistUI::cb_exit(Fl_Menu_ *o, void *v)
{
	whoami(o)->m_mainWindow->hide();
	whoami(o)->m_brushDialog->hide();
}

void ImpressionistUI::cb_swap(Fl_Menu_ *o, void *v)
{
	ImpressionistDoc *pDoc = whoami(o)->getDocument();

	pDoc->swap();
}

void ImpressionistUI::cb_undo(Fl_Menu_ *o, void *v)
{
	ImpressionistDoc *pDoc = whoami(o)->getDocument();

	pDoc->loadForUndo();
}

void ImpressionistUI::cb_dissolve(Fl_Menu_ *o, void *v)
{
	ImpressionistDoc *pDoc = whoami(o)->getDocument();

	char *newfile = fl_file_chooser("Open File?", "*.bmp", pDoc->getImageName());
	if (newfile != NULL)
	{
		pDoc->dissolve(newfile);
	}
}

void ImpressionistUI::cb_view_original(Fl_Menu_ *o, void *v)
{
	ImpressionistDoc *pDoc = whoami(o)->getDocument();

	if (pDoc->m_ucBitmap)
	{
		pDoc->m_ucOriginal = pDoc->m_ucBitmap;
		pDoc->refresh();
	}
}

void ImpressionistUI::cb_view_edge(Fl_Menu_ *o, void *v)
{
	ImpressionistDoc *pDoc = whoami(o)->getDocument();

	if (pDoc->m_ucEdge)
	{
		pDoc->m_ucOriginal = pDoc->m_ucEdge;
		pDoc->refresh();
	}
}

void ImpressionistUI::cb_view_another(Fl_Menu_ *o, void *v)
{
	ImpressionistDoc *pDoc = whoami(o)->getDocument();

	if (pDoc->m_ucAnotherImage)
	{
		pDoc->m_ucOriginal = pDoc->m_ucAnotherImage;
		pDoc->refresh();
	}
}

//-----------------------------------------------------------
// Brings up an about dialog box
// Called by the UI when the about menu item is chosen
//-----------------------------------------------------------
void ImpressionistUI::cb_about(Fl_Menu_ *o, void *v)
{
	fl_message("Impressionist FLTK version for CS341, Spring 2002");
}

//------- UI should keep track of the current for all the controls for answering the query from Doc ---------
//-------------------------------------------------------------
// Sets the type of brush to use to the one chosen in the brush
// choice.
// Called by the UI when a brush is chosen in the brush choice
//-------------------------------------------------------------
void ImpressionistUI::cb_brushChoice(Fl_Widget *o, void *v)
{
	ImpressionistUI *pUI = ((ImpressionistUI *)(o->user_data()));
	ImpressionistDoc *pDoc = pUI->getDocument();

	int type = (int)v;

	if (type == BRUSH_LINES || type == BRUSH_SCATTERED_LINES)
	{
		pUI->m_BrushWidthSlider->activate();
		pUI->m_BrushAngleSlider->activate();
		pUI->m_StrokeDirectionChoice->activate();
		pUI->m_EdgeClippingButton->activate();
		pUI->m_AnotherGradientButton->activate();
	}
	else
	{
		pUI->m_BrushWidthSlider->deactivate();
		pUI->m_BrushAngleSlider->deactivate();
		pUI->m_StrokeDirectionChoice->deactivate();
		pUI->m_EdgeClippingButton->deactivate();
		pUI->m_AnotherGradientButton->deactivate();
	}

	if (type == BRUSH_BLUR || type == BRUSH_SHARP)
	{
		pUI->m_BlurSharpSlider->activate();
	}
	else
	{
		pUI->m_BlurSharpSlider->deactivate();
	}

	pDoc->setBrushType(type);
}

void ImpressionistUI::cb_strokeChoice(Fl_Widget *o, void *v)
{
	ImpressionistUI *pUI = ((ImpressionistUI *)(o->user_data()));
	ImpressionistDoc *pDoc = pUI->getDocument();

	int type = (int)v;

	pDoc->setStrokeType(type);
}

void ImpressionistUI::cb_paintlyStyleChoice(Fl_Widget *o, void *v)
{
}

void ImpressionistUI::cb_paintlyStrokeChoice(Fl_Widget *o, void *v)
{
}

//------------------------------------------------------------
// Clears the paintview canvas.
// Called by the UI when the clear canvas button is pushed
//------------------------------------------------------------
void ImpressionistUI::cb_clear_canvas_button(Fl_Widget *o, void *v)
{
	ImpressionistDoc *pDoc = ((ImpressionistUI *)(o->user_data()))->getDocument();

	pDoc->m_pUI->m_paintView->SaveForUndo();

	pDoc->clearCanvas();
}

//-----------------------------------------------------------
// Updates the brush size to use from the value of the size
// slider
// Called by the UI when the size slider is moved
//-----------------------------------------------------------
void ImpressionistUI::cb_sizeSlides(Fl_Widget *o, void *v)
{
	((ImpressionistUI *)(o->user_data()))->m_nSize = int(((Fl_Slider *)o)->value());

	ImpressionistDoc *pDoc = ((ImpressionistUI *)(o->user_data()))->getDocument();
	ImpressionistUI *pUI = (ImpressionistUI *)(o->user_data());
	pDoc->m_pPaintly->genRefImage(pDoc->m_ucOriginal, pDoc->m_nPaintWidth, pDoc->m_nPaintHeight, pDoc->getSize());
}

//-----------------------------------------------------------
// Updates the brush width to use from the value of the width
// slider
// Called by the UI when the width slider is moved
//-----------------------------------------------------------
void ImpressionistUI::cb_widthSlides(Fl_Widget *o, void *v)
{
	((ImpressionistUI *)(o->user_data()))->m_nWidth = int(((Fl_Slider *)o)->value());
}

//-----------------------------------------------------------
// Updates the brush size to use from the value of the angle
// slider
// Called by the UI when the angle slider is moved
//-----------------------------------------------------------
void ImpressionistUI::cb_angleSlides(Fl_Widget *o, void *v)
{
	int value = int(((Fl_Slider *)o)->value());
	((ImpressionistUI *)(o->user_data()))->m_nAngle = value;
	((ImpressionistDoc *)(((ImpressionistUI *)(o->user_data()))->getDocument()))->m_pStrokeDirection->setAngle(value);
}

void ImpressionistUI::cb_alphaSlides(Fl_Widget *o, void *v)
{
	((ImpressionistUI *)(o->user_data()))->m_nAlpha = double(((Fl_Slider *)o)->value());
}

//-----------------------------------------------------------
// Updates the status of edge clipping
// Called by the UI when the edge clipping button is clicked
//-----------------------------------------------------------
void ImpressionistUI::cb_edge_clipping_button(Fl_Widget *o, void *v)
{
	((ImpressionistUI *)(o->user_data()))->m_nEdgeClipping = int(((Fl_Light_Button *)o)->value());
}

//-----------------------------------------------------------
// Updates the status of another gradient
// Called by the UI when the edge clipping button is clicked
//-----------------------------------------------------------
void ImpressionistUI::cb_another_gradient_button(Fl_Widget *o, void *v)
{
	((ImpressionistUI *)(o->user_data()))->m_nAnotherGradient = int(((Fl_Light_Button *)o)->value());
}

void ImpressionistUI::cb_set_spacing(Fl_Widget *o, void *v)
{
	((ImpressionistUI *)(o->user_data()))->m_nSpacing = int(((Fl_Slider *)o)->value());
}

void ImpressionistUI::cb_random_size(Fl_Widget *o, void *v)
{
	((ImpressionistUI *)(o->user_data()))->m_nRandomSize = int(((Fl_Button *)o)->value());
}

void ImpressionistUI::cb_auto_paint(Fl_Widget *o, void *v)
{
	ImpressionistDoc *pDoc = ((ImpressionistUI *)(o->user_data()))->getDocument();

	pDoc->m_pUI->m_paintView->AutoPaint(((ImpressionistUI *)(o->user_data()))->m_nSpacing);
}

void ImpressionistUI::cb_edgeThreasholdSlides(Fl_Widget *o, void *v)
{
	((ImpressionistUI *)(o->user_data()))->m_nEdgeThreshold = int(((Fl_Slider *)o)->value());
}

void ImpressionistUI::cb_edge_detection_button(Fl_Widget *o, void *v)
{
	((ImpressionistDoc *)(((ImpressionistUI *)(o->user_data()))->getDocument()))->edgeDetection();
}

void ImpressionistUI::cb_blursharpSlides(Fl_Widget *o, void *v)
{
	((ImpressionistUI *)(o->user_data()))->m_nBlurSharpLevel = int(((Fl_Slider *)o)->value());
}

//---------------------------------- per instance functions --------------------------------------

//------------------------------------------------
// Return the ImpressionistDoc used
//------------------------------------------------
ImpressionistDoc *ImpressionistUI::getDocument()
{
	return m_pDoc;
}

//------------------------------------------------
// Draw the main window
//------------------------------------------------
void ImpressionistUI::show()
{
	m_mainWindow->show();
	m_paintView->show();
	m_origView->show();
}

//------------------------------------------------
// Change the paint and original window sizes to
// w by h
//------------------------------------------------
void ImpressionistUI::resize_windows(int w, int h)
{
	m_paintView->size(w, h);
	m_origView->size(w, h);
}

//------------------------------------------------
// Set the ImpressionistDoc used by the UI to
// communicate with the brushes
//------------------------------------------------
void ImpressionistUI::setDocument(ImpressionistDoc *doc)
{
	m_pDoc = doc;

	m_origView->m_pDoc = doc;
	m_paintView->m_pDoc = doc;
}

//------------------------------------------------
// Return the brush size
//------------------------------------------------
int ImpressionistUI::getSize()
{
	return m_nSize;
}

//-------------------------------------------------
// Set the brush size
//-------------------------------------------------
void ImpressionistUI::setSize(int size)
{
	m_nSize = size;

	if (size <= 40)
		m_BrushSizeSlider->value(m_nSize);
}

//------------------------------------------------
// Return the brush size
//------------------------------------------------
int ImpressionistUI::getWidth()
{
	return m_nWidth;
}

//-------------------------------------------------
// Set the brush size
//-------------------------------------------------
void ImpressionistUI::setWidth(int width)
{
	m_nWidth = width;

	if (width <= 40)
		m_BrushWidthSlider->value(m_nWidth);
}

//------------------------------------------------
// Return the brush size
//------------------------------------------------
int ImpressionistUI::getAngle()
{
	return m_nAngle;
}

//-------------------------------------------------
// Set the brush size
//-------------------------------------------------
void ImpressionistUI::setAngle(int angle)
{
	m_nAngle = angle;

	if (angle <= 360)
		m_BrushAngleSlider->value(m_nAngle);
}

//------------------------------------------------
// Return the brush alpha
//------------------------------------------------
double ImpressionistUI::getAlpha()
{
	return m_nAlpha;
}

//-------------------------------------------------
// Set the brush alpha
//-------------------------------------------------
void ImpressionistUI::setAlpha(double alpha)
{
	m_nAlpha = alpha;

	if (alpha <= 1.0)
		m_BrushAlphaSlider->value(m_nAlpha);
}

int ImpressionistUI::getEdgeClipping()
{
	return m_nEdgeClipping;
}

void ImpressionistUI::setEdgeClipping(int value)
{
	m_nEdgeClipping = value;

	m_EdgeClippingButton->value(value);
}

int ImpressionistUI::getAnotherGradient()
{
	return m_nAnotherGradient;
}

void ImpressionistUI::setAnotherGradient(int value)
{
	m_nAnotherGradient = value;

	m_AnotherGradientButton->value(value);
}

//------------------------------------------------
// Return the edge threashold
//------------------------------------------------
int ImpressionistUI::getEdgeThreashold()
{
	return m_nEdgeThreshold;
}

//-------------------------------------------------
// Set the edge threashold
//-------------------------------------------------
void ImpressionistUI::setEdgeThreashold(int threashold)
{
	m_nEdgeThreshold = threashold;

	if (threashold <= 1.0)
		m_EdgeThresholdSlider->value(m_nEdgeThreshold);
}

int ImpressionistUI::getBlurSharpLevel()
{
	return m_nBlurSharpLevel;
}

void ImpressionistUI::setBlurSharpLevel(int level)
{
	m_nBlurSharpLevel = level;

	if (level <= 7)
		m_BlurSharpSlider->value(m_nBlurSharpLevel);
}

bool ImpressionistUI::getIsNormalized()
{
	return (bool)m_nIsNormalized;
}

int ImpressionistUI::getRandomSize()
{
	return m_nRandomSize;
}

double ImpressionistUI::getFadeAlpha()
{
	return m_nfadeAlpha;
}

void ImpressionistUI::setPaintly()
{
	m_pDoc->m_nPaintlyTheshold = 100;
	m_pDoc->m_nPaintlyCurvature = 1;
	m_pDoc->m_nPaintlyBlur = 0.5;
	m_pDoc->m_nPaintlyGridSize = 1;
	m_pDoc->m_nPaintlyMinStrokeL = 4;
	m_pDoc->m_nPaintlyMaxStrokeL = 16;
	m_pDoc->m_nPaintlyAlpha = 1;
	m_pDoc->m_nPaintlyLayer = 1;
	m_pDoc->m_nPaintlyR0Level = 5;
}

// Main menu definition
Fl_Menu_Item ImpressionistUI::menuitems[] = {
	{"&File", 0, 0, 0, FL_SUBMENU},
	{"&Load Image...", FL_ALT + 'l', (Fl_Callback *)ImpressionistUI::cb_load_image},
	{"&Save Image...", FL_ALT + 's', (Fl_Callback *)ImpressionistUI::cb_save_image},
	{"&Brushes...", FL_ALT + 'b', (Fl_Callback *)ImpressionistUI::cb_brushes},
	{"&Color...", FL_ALT + 'k', (Fl_Callback *)ImpressionistUI::cb_colors},
	{"&Add Faded Background...", FL_ALT + 'a', (Fl_Callback *)ImpressionistUI::cb_fade},
	{"&Clear Canvas", FL_ALT + 'c', (Fl_Callback *)ImpressionistUI::cb_clear_canvas, 0, FL_MENU_DIVIDER},

	{"&Load Edge Image", NULL, (Fl_Callback *)ImpressionistUI::cb_edge_image},
	{"&Load Another Image", NULL, (Fl_Callback *)ImpressionistUI::cb_another_image},

	{"&Quit", FL_ALT + 'q', (Fl_Callback *)ImpressionistUI::cb_exit},
	{0},
	{"&Edit", 0, 0, 0, FL_SUBMENU},
	{"&Swap", 's', (Fl_Callback *)ImpressionistUI::cb_swap},
	{"Undo", FL_CTRL + 'z', (Fl_Callback *)ImpressionistUI::cb_undo},
	{"Dissolve", FL_CTRL + 'd', (Fl_Callback *)ImpressionistUI::cb_dissolve},
	{0},
	{"&View", 0, 0, 0, FL_SUBMENU},
	{"&Original View", FL_ALT + 'o', (Fl_Callback *)ImpressionistUI::cb_view_original},
	{"&Edge View", FL_ALT + 'e', (Fl_Callback *)ImpressionistUI::cb_view_edge},
	{"&Another Image", FL_ALT + 'i', (Fl_Callback *)ImpressionistUI::cb_view_another},
	{0},
	{"&Help", 0, 0, 0, FL_SUBMENU},
	{"&About", FL_ALT + 'a', (Fl_Callback *)ImpressionistUI::cb_about},
	{0},

	{0}};

// Brush choice menu definition
Fl_Menu_Item ImpressionistUI::brushTypeMenu[NUM_BRUSH_TYPE + 1] = {
	{"Points", FL_ALT + 'p', (Fl_Callback *)ImpressionistUI::cb_brushChoice, (void *)BRUSH_POINTS},
	{"Lines", FL_ALT + 'l', (Fl_Callback *)ImpressionistUI::cb_brushChoice, (void *)BRUSH_LINES},
	{"Circles", FL_ALT + 'c', (Fl_Callback *)ImpressionistUI::cb_brushChoice, (void *)BRUSH_CIRCLES},
	{"Scattered Points", FL_ALT + 'q', (Fl_Callback *)ImpressionistUI::cb_brushChoice, (void *)BRUSH_SCATTERED_POINTS},
	{"Scattered Lines", FL_ALT + 'm', (Fl_Callback *)ImpressionistUI::cb_brushChoice, (void *)BRUSH_SCATTERED_LINES},
	{"Scattered Circles", FL_ALT + 'd', (Fl_Callback *)ImpressionistUI::cb_brushChoice, (void *)BRUSH_SCATTERED_CIRCLES},
	{"Blurring", FL_ALT + 'r', (Fl_Callback *)ImpressionistUI::cb_brushChoice, (void *)BRUSH_BLUR},
	{"Sharpening", FL_ALT + 'f', (Fl_Callback *)ImpressionistUI::cb_brushChoice, (void *)BRUSH_SHARP},
	{0}};

Fl_Menu_Item ImpressionistUI::strokeDirectionTypeMenu[NUM_STROKE_TYPE + 1] = {
	{"Slider/Right Mouse", FL_ALT + 's', (Fl_Callback *)ImpressionistUI::cb_strokeChoice, (void *)STROKE_SLIDER},
	{"Gradient", FL_ALT + 'g', (Fl_Callback *)ImpressionistUI::cb_strokeChoice, (void *)STROKE_GRADIENT},
	{"Brush Direction", FL_ALT + 'b', (Fl_Callback *)ImpressionistUI::cb_strokeChoice, (void *)STROKE_BRUSH_DIRECTION},
	{0}};

Fl_Menu_Item ImpressionistUI::styleTypeMenu[] = {
	{"Impressionist", FL_ALT + 'i', (Fl_Callback *)ImpressionistUI::cb_paintlyStyleChoice, (void *)PAINTLY_IMPRESSIONIST},
	{"Expressionist", FL_ALT + 'e', (Fl_Callback *)ImpressionistUI::cb_paintlyStyleChoice, (void *)PAINTLY_EXPRESSIONIST},
	{"Color Wash", FL_ALT + 'w', (Fl_Callback *)ImpressionistUI::cb_paintlyStyleChoice, (void *)PAINTLY_COLOR_WASH},
	{"Pointillist", FL_ALT + 'p', (Fl_Callback *)ImpressionistUI::cb_paintlyStyleChoice, (void *)PAINTLY_POINTILLIST},
	{"Customize", FL_ALT + 'c', (Fl_Callback *)ImpressionistUI::cb_paintlyStyleChoice, (void *)PAINTLY_CUSTOMIZE},
	{0}};

Fl_Menu_Item ImpressionistUI::strokeTypeMenu[] = {
	{"Curve Brush", FL_ALT + 'r', (Fl_Callback *)ImpressionistUI::cb_paintlyStrokeChoice, (void *)PAINTLY_CURVE},
	{"BSpline Brush", FL_ALT + 'b', (Fl_Callback *)ImpressionistUI::cb_paintlyStrokeChoice, (void *)PAINTLY_BSPLINE},
	{"Circle Brush", FL_ALT + 'c', (Fl_Callback *)ImpressionistUI::cb_paintlyStrokeChoice, (void *)PAINTLY_CIRCLE},
	{"Clip Line Brush", FL_ALT + 'p', (Fl_Callback *)ImpressionistUI::cb_paintlyStrokeChoice, (void *)PAINTLY_CLIP},
	{"Line Brush", FL_ALT + 'l', (Fl_Callback *)ImpressionistUI::cb_paintlyStrokeChoice, (void *)PAINTLY_LINE},
	{0}};

//----------------------------------------------------
// Constructor.  Creates all of the widgets.
// Add new widgets here
//----------------------------------------------------
ImpressionistUI::ImpressionistUI()
{
	// Create the main window
	m_mainWindow = new Fl_Window(600, 300, "Impressionist");
	m_mainWindow->user_data((void *)(this)); // record self to be used by static callback functions
	// install menu bar
	m_menubar = new Fl_Menu_Bar(0, 0, 600, 25);
	m_menubar->menu(menuitems);

	// Create a group that will hold two sub windows inside the main
	// window
	Fl_Group *group = new Fl_Group(0, 25, 600, 275);

	// install paint view window
	m_paintView = new PaintView(300, 25, 300, 275, "This is the paint view"); // 0jon
	m_paintView->box(FL_DOWN_FRAME);

	// install original view window
	m_origView = new OriginalView(0, 25, 300, 275, "This is the orig view"); // 300jon
	m_origView->box(FL_DOWN_FRAME);
	m_origView->deactivate();

	group->end();
	Fl_Group::current()->resizable(group);
	m_mainWindow->end();

	// init values

	m_nSize = 10;
	m_nWidth = 1;
	m_nAngle = 0;
	m_nAlpha = 1.0;
	m_nStrokeType = 0;
	m_nEdgeClipping = 1;
	m_nAnotherGradient = 0;
	m_nSpacing = 4;
	m_nEdgeThreshold = 200;
	m_nIsNormalized = 1;
	m_nBlurSharpLevel = 1;
	m_nRandomSize = 1;
	m_nfadeAlpha = 0;

	// brush dialog definition
	m_brushDialog = new Fl_Window(400, 470, "Brush Dialog");
	// Add a brush type choice to the dialog
	m_BrushTypeChoice = new Fl_Choice(50, 10, 150, 25, "&Brush");
	m_BrushTypeChoice->user_data((void *)(this)); // record self to be used by static callback functions
	m_BrushTypeChoice->menu(brushTypeMenu);
	m_BrushTypeChoice->callback(cb_brushChoice);

	m_ClearCanvasButton = new Fl_Button(240, 10, 150, 25, "&Clear Canvas");
	m_ClearCanvasButton->user_data((void *)(this));
	m_ClearCanvasButton->callback(cb_clear_canvas_button);

	m_StrokeDirectionChoice = new Fl_Choice(115, 45, 150, 25, "&Stroke Direction");
	m_StrokeDirectionChoice->user_data((void *)(this));
	m_StrokeDirectionChoice->menu(strokeDirectionTypeMenu);
	m_StrokeDirectionChoice->value(m_nStrokeType);
	m_StrokeDirectionChoice->callback(cb_strokeChoice);
	m_StrokeDirectionChoice->deactivate();

	// Add brush size slider to the dialog
	m_BrushSizeSlider = new Fl_Value_Slider(10, 80, 300, 20, "Size");
	m_BrushSizeSlider->user_data((void *)(this)); // record self to be used by static callback functions
	m_BrushSizeSlider->type(FL_HOR_NICE_SLIDER);
	m_BrushSizeSlider->labelfont(FL_COURIER);
	m_BrushSizeSlider->labelsize(12);
	m_BrushSizeSlider->minimum(1);
	m_BrushSizeSlider->maximum(40);
	m_BrushSizeSlider->step(1);
	m_BrushSizeSlider->value(m_nSize);
	m_BrushSizeSlider->align(FL_ALIGN_RIGHT);
	m_BrushSizeSlider->when(FL_WHEN_RELEASE);
	m_BrushSizeSlider->callback(cb_sizeSlides);

	// Add brush width slider to the dialog
	m_BrushWidthSlider = new Fl_Value_Slider(10, 110, 300, 20, "Line Width");
	m_BrushWidthSlider->user_data((void *)(this)); // record self to be used by static callback functions
	m_BrushWidthSlider->type(FL_HOR_NICE_SLIDER);
	m_BrushWidthSlider->labelfont(FL_COURIER);
	m_BrushWidthSlider->labelsize(12);
	m_BrushWidthSlider->minimum(1);
	m_BrushWidthSlider->maximum(40);
	m_BrushWidthSlider->step(1);
	m_BrushWidthSlider->value(m_nWidth);
	m_BrushWidthSlider->align(FL_ALIGN_RIGHT);
	m_BrushWidthSlider->callback(cb_widthSlides);
	m_BrushWidthSlider->deactivate();

	// Add brush angle slider to the dialog
	m_BrushAngleSlider = new Fl_Value_Slider(10, 140, 300, 20, "Line Angle");
	m_BrushAngleSlider->user_data((void *)(this)); // record self to be used by static callback functions
	m_BrushAngleSlider->type(FL_HOR_NICE_SLIDER);
	m_BrushAngleSlider->labelfont(FL_COURIER);
	m_BrushAngleSlider->labelsize(12);
	m_BrushAngleSlider->minimum(0);
	m_BrushAngleSlider->maximum(360);
	m_BrushAngleSlider->step(1);
	m_BrushAngleSlider->value(m_nAngle);
	m_BrushAngleSlider->align(FL_ALIGN_RIGHT);
	m_BrushAngleSlider->callback(cb_angleSlides);
	m_BrushAngleSlider->deactivate();

	m_BrushAlphaSlider = new Fl_Value_Slider(10, 170, 300, 20, "Alpha");
	m_BrushAlphaSlider->user_data((void *)(this)); // record self to be used by static callback functions
	m_BrushAlphaSlider->type(FL_HOR_NICE_SLIDER);
	m_BrushAlphaSlider->labelfont(FL_COURIER);
	m_BrushAlphaSlider->labelsize(12);
	m_BrushAlphaSlider->minimum(0);
	m_BrushAlphaSlider->maximum(1);
	m_BrushAlphaSlider->step(0.01);
	m_BrushAlphaSlider->value(m_nAlpha);
	m_BrushAlphaSlider->align(FL_ALIGN_RIGHT);
	m_BrushAlphaSlider->callback(cb_alphaSlides);

	m_EdgeClippingButton = new Fl_Light_Button(10, 200, 115, 25, "&Edge Clipping");
	m_EdgeClippingButton->user_data((void *)(this));
	m_EdgeClippingButton->value(m_nEdgeClipping);
	m_EdgeClippingButton->callback(cb_edge_clipping_button);
	m_EdgeClippingButton->deactivate();

	m_AnotherGradientButton = new Fl_Light_Button(240, 200, 150, 25, "&Another Gradient");
	m_AnotherGradientButton->user_data((void *)(this));
	m_AnotherGradientButton->callback(cb_another_gradient_button);
	m_AnotherGradientButton->deactivate();

	m_SpacingSlider = new Fl_Value_Slider(10, 10, 140, 20, "Spacing");
	m_SpacingSlider->user_data((void *)(this));
	m_SpacingSlider->type(FL_HOR_NICE_SLIDER);
	m_SpacingSlider->labelfont(FL_COURIER);
	m_SpacingSlider->labelsize(12);
	m_SpacingSlider->minimum(1);
	m_SpacingSlider->maximum(16);
	m_SpacingSlider->step(1);
	m_SpacingSlider->value(m_nSpacing);
	m_SpacingSlider->align(FL_ALIGN_RIGHT);
	m_SpacingSlider->callback(cb_set_spacing);

	m_SizeRandomButton = new Fl_Light_Button(210, 10, 100, 20, "&Size Rand.");
	m_SizeRandomButton->user_data((void *)(this));
	m_SizeRandomButton->set();
	m_SizeRandomButton->callback(cb_random_size);

	m_PaintButton = new Fl_Button(320, 10, 50, 20, "&Paint");
	m_PaintButton->user_data((void *)(this));
	m_PaintButton->callback(cb_auto_paint);

	m_PaintBox = new Fl_Window(10, 230, 380, 40);
	m_PaintBox->box(FL_UP_BOX);
	m_PaintBox->add(m_SpacingSlider);
	m_PaintBox->add(m_SizeRandomButton);
	m_PaintBox->add(m_PaintButton);
	m_PaintBox->end();

	m_EdgeThresholdSlider = new Fl_Value_Slider(10, 8, 200, 20, "Edge Threshold");
	m_EdgeThresholdSlider->user_data((void *)(this));
	m_EdgeThresholdSlider->type(FL_HOR_NICE_SLIDER);
	m_EdgeThresholdSlider->labelfont(FL_COURIER);
	m_EdgeThresholdSlider->labelsize(12);
	m_EdgeThresholdSlider->minimum(0);
	m_EdgeThresholdSlider->maximum(600);
	m_EdgeThresholdSlider->step(1);
	m_EdgeThresholdSlider->value(m_nEdgeThreshold);
	m_EdgeThresholdSlider->align(FL_ALIGN_RIGHT);
	m_EdgeThresholdSlider->callback(cb_edgeThreasholdSlides);

	m_EdgeDetectionButton = new Fl_Button(320, 8, 50, 20, "&Do it");
	m_EdgeDetectionButton->user_data((void *)(this));
	m_EdgeDetectionButton->callback(cb_edge_detection_button);

	m_EdgeDetectionBox = new Fl_Window(10, 275, 380, 40);
	m_EdgeDetectionBox->box(FL_UP_BOX);
	m_EdgeDetectionBox->add(m_EdgeThresholdSlider);
	m_EdgeDetectionBox->add(m_EdgeDetectionButton);
	m_EdgeDetectionBox->end();

	m_BlurSharpSlider = new Fl_Value_Slider(10, 320, 250, 20, "Blurring/Sharpening Level");
	m_BlurSharpSlider->user_data((void *)(this)); // record self to be used by static callback functions
	m_BlurSharpSlider->type(FL_HOR_NICE_SLIDER);
	m_BlurSharpSlider->labelfont(FL_COURIER);
	m_BlurSharpSlider->labelsize(12);
	m_BlurSharpSlider->minimum(1);
	m_BlurSharpSlider->maximum(4);
	m_BlurSharpSlider->step(1);
	m_BlurSharpSlider->value(m_nBlurSharpLevel);
	m_BlurSharpSlider->align(FL_ALIGN_RIGHT);
	m_BlurSharpSlider->callback(cb_blursharpSlides);
	m_BlurSharpSlider->deactivate();

	m_brushDialog->end();

	m_colorDialog = new Fl_Window(300, 200, "Color Dialog");
	m_colorChooser = new Fl_Color_Chooser(10, 10, 280, 180);
	m_colorChooser->hsv(0.0, 0.0, 1.0);
	m_colorDialog->end();

	m_paintlyDialog = new Fl_Window(400, 270, "Paintly Dialog");
	m_PaintlyStyleChoice = new Fl_Choice(50, 10, 120, 20, "&Style");
	m_PaintlyStyleChoice->user_data((void *)(this));
	m_PaintlyStyleChoice->menu(styleTypeMenu);
	m_PaintlyStyleChoice->callback(cb_paintly_style);

	m_PaintlyStrokeChoice = new Fl_Choice(225, 10, 100, 20, "&Stroke");
	m_PaintlyStrokeChoice->user_data((void *)(this));
	m_PaintlyStrokeChoice->menu(strokeTypeMenu);
	m_PaintlyStrokeChoice->callback(cb_paintly_stroke);

	m_paintlyDialog->end();

	m_fadeDialog = new Fl_Window(240, 40, "Fade Dialog");
	m_FadeSlider = new Fl_Value_Slider(10, 10, 140, 20, "Fade In");
	m_FadeSlider->user_data((void *)(this));
	m_FadeSlider->type(FL_HOR_NICE_SLIDER);
	m_FadeSlider->labelfont(FL_COURIER);
	m_FadeSlider->labelsize(12);
	m_FadeSlider->minimum(0);
	m_FadeSlider->maximum(1);
	m_FadeSlider->step(0.01);
	m_FadeSlider->value(m_nfadeAlpha);
	m_FadeSlider->align(FL_ALIGN_RIGHT);
	m_FadeSlider->callback(cb_fade_in);
	m_fadeDialog->end();
}
