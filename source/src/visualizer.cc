
#include "visualizer.hh"

Visualizer::~Visualizer()
{
  if (opened)
    {
      assert(d_win);
      delete d_win;
    }
}


void Visualizer::ImgSize(int& width,int& height)
{
  const int w=d_cost_bkg.AskWidth(), h=d_cost_bkg.AskHeight();

  switch (d_type)
    {
    case Cylinder:      width=w;   height=  h; break;
    case Torus:         width=w;   height=2*h; break;
    case FourQuadrants: width=2*w; height=  h; break;
    }
}


void Visualizer::PrepareVisualization()
{
  int w=d_cost_bkg.AskWidth(), h=d_cost_bkg.AskHeight();

  int imgwidth, imgheight;
  ImgSize(imgwidth, imgheight);

  switch (d_type)
    {
    case FourQuadrants:
      Copy(d_disp,w,0, d_cost_bkg,0,0,w,h);
      Copy(d_disp,w,h, d_cost_bkg,0,0,w,h);
      DrawLine(d_disp,0,h,2*w,h, Color<Pixel>(155,155,255));
      DrawLine(d_disp,w,0,w,2*h, Color<Pixel>(155,155,255));
      // FALL THROUGH

    case Torus:
      Copy(d_disp,0,h, d_cost_bkg,0,0,w,h);
      DrawLine(d_disp,0,h,w,h, Color<Pixel>(155,155,255));
      // FALL THROUGH

    case Cylinder:
      Copy(d_disp,0,0, d_cost_bkg,0,0,w,h);
      break;
    }
}

char Visualizer::ShowVisualization(bool wait_for_keypress)
{
  d_win->Display(d_disp);
  if (!wait_for_keypress) return d_win->CheckForKeypress();
  return d_win->WaitForKeypress();
}
