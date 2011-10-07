
#ifndef VISUALIZER_VIDEOGFX_HH
#define VISUALIZER_VIDEOGFX_HH

#include "visualizer.hh"

#include <libvideogfx.hh>

using namespace videogfx;


class Visualizer_VideoGfx : public Visualizer
{
public:
  Visualizer_VideoGfx() : m_opened(false), m_win(NULL) { }
  ~Visualizer_VideoGfx();

  template <class Cost> void initialize(const Bitmap<Cost>& costmatrix, GraphType type=Torus);
#if 0
  template <class Cost> void initialize(const Bitmap<Cost>& costmatrix, bool segmentation_graph, bool fourquadrants)
  {
    /**/ if (fourquadrants)      Initialize(costmatrix, FourQuadrants);
    else if (segmentation_graph) Initialize(costmatrix, Cylinder);
    else                         Initialize(costmatrix, Torus);
  }
#endif

  void prepareVisualization();
  char showVisualization(bool wait_for_keypress=true);  // returns the key pressed while showing the image
  void drawPath(const Path& path, int colorHex);
  void drawArea(const Path& abovepath, const Path& belowpath, int colorHex);
  void markRange(int start,int end, int endOffset, int colorHex);

  const Image<Pixel>& getImage() const { return m_disp; }

private:
  bool m_opened;
  GraphType m_type;
  ImageWindow_Autorefresh_X11* m_win;
  Image<Pixel> m_costBkg;

  Image<Pixel> m_disp; // render all the drawings here

  void calcImgSize(int& width,int& height);
};


void drawPath(Image<Pixel>& img, const Path& path, Color<Pixel> col);
void drawArea(Image<Pixel>& img, const Path& abovepath, const Path& belowpath, Color<Pixel> col);


// -------------------- implementation --------------------

template <class Cost>
void Visualizer_VideoGfx::initialize(const Bitmap<Cost>& costmatrix, GraphType type)
{
  m_type = type;

  bool oldsize = (costmatrix.getWidth() == m_costBkg.AskWidth() &&
		  costmatrix.getHeight() == m_costBkg.AskHeight());

  Bitmap<Cost> rescaled_cost = costmatrix.Clone();
  StretchValues(rescaled_cost, (Cost)0, (Cost)255);

  m_costBkg.Create(costmatrix.AskWidth(), costmatrix.AskHeight(), Colorspace_RGB);
  ConvertBitmap(m_costBkg.AskBitmapR(), rescaled_cost);
  m_costBkg.AskBitmapG() = m_costBkg.AskBitmapR().Clone();
  m_costBkg.AskBitmapB() = m_costBkg.AskBitmapR().Clone();

  if (m_opened)
    {
      if (oldsize) return;

      assert(m_win);
      delete m_win;
    }

  int winwidth, winheight;
  calcImgSize(winwidth,winheight);

  m_disp.Create(winwidth, winheight, Colorspace_RGB);

  m_win = new ImageWindow_Autorefresh_X11;
  m_win->Create(winwidth,winheight,"shortest circular-path   (c) Dirk Farin");
  m_opened=true;
}


#endif
