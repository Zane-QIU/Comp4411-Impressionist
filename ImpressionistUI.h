//
// impressionistUI.h
//
// The header file for the UI part
//

#ifndef ImpressionistUI_h
#define ImpressionistUI_h

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/fl_file_chooser.H>		// FLTK file chooser
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Light_Button.H>
#include <FL/Fl_Color_Chooser.H>
#include <FL/Fl_Multiline_Input.H>
#include <FL/Fl_BMP_Image.H>

#include "Impressionist.h"
#include "OriginalView.h"
#include "PaintView.h"

#include "ImpBrush.h"

#include "Matrix.h"

class ImpressionistUI {
public:
	ImpressionistUI();

	// The FLTK widgets
	Fl_Window*			m_mainWindow;
	Fl_Menu_Bar*		m_menubar;
								
	PaintView*			m_paintView;
	OriginalView*		m_origView;

// for brush dialog
	Fl_Window*			m_brushDialog;

	Fl_Choice*			m_BrushTypeChoice;
	Fl_Choice*			m_StrokeDirectionChoice;

	Fl_Slider*			m_BrushSizeSlider;
	Fl_Slider*			m_BrushWidthSlider;
	Fl_Slider*			m_BrushAngleSlider;
	Fl_Slider*			m_BrushAlphaSlider;
	Fl_Slider*			m_SpacingSlider;
	Fl_Slider*			m_EdgeThresholdSlider;
	Fl_Slider*			m_BlurSharpSlider;

	Fl_Button*          m_ClearCanvasButton;
	Fl_Button*			m_EdgeClippingButton;
	Fl_Button*			m_AnotherGradientButton;
	Fl_Button*			m_SizeRandomButton;
	Fl_Button*			m_PaintButton;
	Fl_Button*			m_EdgeDetectionButton;

	Fl_Window*			m_PaintBox;
	Fl_Window*			m_EdgeDetectionBox;


// for color dialog
	Fl_Window*			m_colorDialog;

	Fl_Color_Chooser*	m_colorChooser;



	Fl_Window*			m_paintlyDialog;
	Fl_Choice*			m_PaintlyStyleChoice;
	Fl_Choice*			m_PaintlyStrokeChoice;



//for faded dialog
	Fl_Window*			m_fadeDialog;
	Fl_Slider*			m_FadeSlider;

	// Member functions
	void				setDocument(ImpressionistDoc* doc);
	ImpressionistDoc*	getDocument();

	void				show();
	void				resize_windows(int w, int h);

	// Interface to get attribute

	int					getSize();
	void				setSize(int size);

	int					getWidth();
	void				setWidth(int width);

	int					getAngle();
	void				setAngle(int angle);

	double				getAlpha();
	void				setAlpha(double alpha);

	int					getEdgeClipping();
	void				setEdgeClipping(int value);

	int					getAnotherGradient();
	void				setAnotherGradient(int value);

	int					getEdgeThreashold();
	void				setEdgeThreashold(int threashold);

	int					getBlurSharpLevel();
	void				setBlurSharpLevel(int level);

	bool				getIsNormalized();

	int 				getRandomSize();


	double				getFadeAlpha();
	void				setPaintly();

private:
	ImpressionistDoc*	m_pDoc;		// pointer to document to communicate with the document

	// All attributes here
	int		m_nSize;
	int		m_nWidth;
	int		m_nAngle;
	double	m_nAlpha;
	int		m_nStrokeType;
	int		m_nEdgeClipping;
	int		m_nAnotherGradient;
	int		m_nSpacing;
	double	m_nfadeAlpha;
	int		m_nEdgeThreshold;
	int		m_nBlurSharpLevel;
	int		m_nRandomSize;

	int		m_nIsNormalized;

	// Static class members
	static Fl_Menu_Item		menuitems[];
	static Fl_Menu_Item		brushTypeMenu[NUM_BRUSH_TYPE+1];
	static Fl_Menu_Item		styleTypeMenu[];
	static Fl_Menu_Item		strokeTypeMenu[];
	static Fl_Menu_Item		strokeDirectionTypeMenu[NUM_STROKE_TYPE+1];

	static ImpressionistUI*	whoami(Fl_Menu_* o);

	// All callbacks here.  Callbacks are declared 
	// static
	static void	cb_load_image(Fl_Menu_* o, void* v);
	static void	cb_save_image(Fl_Menu_* o, void* v);
	static void	cb_brushes(Fl_Menu_* o, void* v);
	static void cb_colors(Fl_Menu_* o, void* v);
	static void cb_paintly(Fl_Menu_* o, void* v);
	static void cb_paintly_style(Fl_Widget* o, void* v);
	static void cb_paintly_stroke(Fl_Widget* o, void* v);
	static void cb_paintly_run(Fl_Widget* o, void* v);
	static void	cb_paintlyStyleChoice(Fl_Widget* o, void* v);
	static void	cb_paintlyStrokeChoice(Fl_Widget* o, void* v);
	static void cb_fade(Fl_Menu_* o, void* v);
	static void cb_fade_in(Fl_Widget* o, void* v);
	static void	cb_clear_canvas(Fl_Menu_* o, void* v);
	static void cb_edge_image(Fl_Menu_* o, void* v);
	static void cb_another_image(Fl_Menu_* o, void* v);
	static void	cb_exit(Fl_Menu_* o, void* v);
	static void	cb_swap(Fl_Menu_* o, void* v);
	static void	cb_undo(Fl_Menu_* o, void* v);
	static void	cb_dissolve(Fl_Menu_* o, void* v);
	static void cb_view_original(Fl_Menu_* o, void* v);
	static void cb_view_edge(Fl_Menu_* o, void* v);
	static void cb_view_another(Fl_Menu_* o, void* v);
	static void	cb_about(Fl_Menu_* o, void* v);
	static void	cb_brushChoice(Fl_Widget* o, void* v);
	static void cb_strokeChoice(Fl_Widget* o, void* v);
	static void cb_styleChoice(Fl_Widget* o, void* v);
	static void	cb_clear_canvas_button(Fl_Widget* o, void* v);
	static void	cb_sizeSlides(Fl_Widget* o, void* v);
	static void	cb_widthSlides(Fl_Widget* o, void* v);
	static void	cb_angleSlides(Fl_Widget* o, void* v);
	static void cb_alphaSlides(Fl_Widget* o, void* v);
	static void cb_edge_clipping_button(Fl_Widget* o, void* v);
	static void cb_another_gradient_button(Fl_Widget* o, void* v);
	static void cb_set_spacing(Fl_Widget* o, void* v);
	static void cb_random_size(Fl_Widget* o, void* v);
	static void cb_auto_paint(Fl_Widget* o, void* v);
	static void cb_edgeThreasholdSlides(Fl_Widget* o, void* v);
	static void cb_edge_detection_button(Fl_Widget* o, void* v);
	static void cb_blursharpSlides(Fl_Widget* o, void* v);

};

#endif
