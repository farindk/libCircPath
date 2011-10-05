
#include "visualizer_videogfx.hh"


Visualizer_VideoGfx::~Visualizer_VideoGfx()
{
  if (m_opened)
    {
      assert(m_win);
      delete m_win;
    }
}


void Visualizer_VideoGfx::calcImgSize(int& width,int& height)
{
  const int w=m_costBkg.AskWidth(), h=m_costBkg.AskHeight();

  switch (m_type)
    {
    case Cylinder:      width=w;   height=  h; break;
    case Torus:         width=w;   height=2*h; break;
      //case FourQuadrants: width=2*w; height=  h; break;
    }
}


void Visualizer_VideoGfx::prepareVisualization()
{
  int w=m_costBkg.AskWidth(), h=m_costBkg.AskHeight();

  int imgwidth, imgheight;
  calcImgSize(imgwidth, imgheight);

  switch (m_type)
    {
      /*
    case FourQuadrants:
      Copy(d_disp,w,0, d_cost_bkg,0,0,w,h);
      Copy(d_disp,w,h, d_cost_bkg,0,0,w,h);
      DrawLine(d_disp,0,h,2*w,h, Color<Pixel>(155,155,255));
      DrawLine(d_disp,w,0,w,2*h, Color<Pixel>(155,155,255));
      // FALL THROUGH
      */

    case Torus:
      Copy(m_disp,0,h, m_costBkg,0,0,w,h);
      DrawLine(m_disp,0,h,w,h, Color<Pixel>(155,155,255));
      // FALL THROUGH

    case Cylinder:
      Copy(m_disp,0,0, m_costBkg,0,0,w,h);
      break;
    }
}

char Visualizer_VideoGfx::showVisualization(bool wait_for_keypress)
{
  m_win->Display(m_disp);
  if (!wait_for_keypress) return 0; //m_win->CheckForKeypress();
  return m_win->WaitForKeypress();
}





void Visualizer_VideoGfx::drawPath(const Path& path, int colorHex)
{
  ::drawPath(m_disp, path, Color<Pixel>(colorHex>>16, (colorHex>>8)&0xFF, colorHex&0xFF));
}


void drawPath(Image<Pixel>& img, const Path& path, Color<Pixel> col)
{
  int w=img.AskWidth(), h=img.AskHeight();

  for (int c=0;c<w;c++)
    {
      if (path.topRow(c)/h != path.bottomRow(c)/h)
        {
          assert(path.topRow(c)/h < path.bottomRow(c)/h);

          // overwrap case

          if (path.topRow(c)/h == path.bottomRow(c)/h-1)
            {
              // one wraparound
              DrawLine(img, c, path.topRow(c)%h, c, h-1, col);
              DrawLine(img, c, 0, c, path.bottomRow(c)%h, col);
            }
          else
            {
              // more than once
              DrawLine(img, c, 0, c, h-1, col);
            }
        }
      else
        {
          // normal case

          DrawLine(img,
                   c, path.topRow(c)%h,
                   c, path.bottomRow(c)%h,
                   col);
        }
    }
}


void Visualizer_VideoGfx::drawArea(const Path& abovepath, const Path& belowpath, int colorHex)
{
  ::drawArea(m_disp, abovepath, belowpath, Color<Pixel>(colorHex>>16, (colorHex>>8)&0xFF, colorHex&0xFF));
}


void drawArea(Image<Pixel>& img, const Path& abovepath, const Path& belowpath, Color<Pixel> col)
{
  int w = abovepath.getWidth();

  for (int c=0;c<w;c++)
    {
      DrawLine(img, c,abovepath.topRow(c), c,belowpath.bottomRow(c), col);
    }
}

