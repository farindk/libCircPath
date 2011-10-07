
#include <iostream>
#include <algorithm>
#include <libvideogfx.hh>

using namespace std;
using namespace videogfx;



void TraceContour_OLD(const Bitmap<Pixel>& bm, DynArray<Point2D<int> >& trace)
{
  const Pixel OnObject = 0;
  const Pixel Background = 255;

  Bitmap<Pixel> contour = bm.Clone();

  // find starting point

  int w=bm.AskWidth(), h=bm.AskHeight();

  /* */ Pixel*const* p = contour.AskFrame();
  const Pixel*const* p0 = bm.AskFrame();

  int startx,starty;
  for (int y=0;y<h;y++)
    for (int x=0;x<w;x++)
      if (p[y][x]==0) // found an "on"-pixel
	{ startx=x; starty=y; goto found; }

  cerr << "did not find any object in the input shape\n";
  exit(5);

 found:
  // filter out interior points

  for (int y=std::max(starty,1);y<h-1;y++)
    for (int x=1;x<w-1;x++)
      {
	if (p0[y-1][x]==0 && p0[y+1][x]==0 && p0[y][x-1]==0 && p0[y][x+1]==0)
	  p[y][x] = 255;
      }

  // start to scan the contour

  int x=startx, y=starty;

  for (;;)
    {
      trace.Append(Point2D<int>(x,y));
      p[y][x] = Background;

      /**/ if (x<w-1 &&          p[y  ][x+1]==OnObject) x++;
      else if (y<h-1 &&          p[y+1][x  ]==OnObject) y++;
      else if (x>0   &&          p[y  ][x-1]==OnObject) x--;
      else if (y>0   &&          p[y-1][x  ]==OnObject) y--;
      else if (x<w-1 && y<h-1 && p[y+1][x+1]==OnObject) { x++; y++; }
      else if (x>0   && y<h-1 && p[y+1][x-1]==OnObject) { x--; y++; }
      else if (x>0   && y>0   && p[y-1][x-1]==OnObject) { x--; y--; }
      else if (x<w-1 && y>0   && p[y-1][x+1]==OnObject) { x++; y--; }
      else { break; }

      DisplayX11(MakeImage(contour));
    }

  if (abs(x-startx)>1 || abs(y-starty)>1)
    { cerr << "contour is not closed\n"; exit(5); }

  //WriteImage_PNG(MakeImage(contour), "out.png");
}


static struct { int dx,dy; } dir_vector[8] =
  {
    {  1,0 }, {  1, 1 }, { 0, 1 }, { -1, 1 },
    { -1,0 }, { -1,-1 }, { 0,-1 }, {  1,-1 }
  };

void TraceContour(const Bitmap<Pixel>& bm, DynArray<Point2D<int> >& trace)
{
  const Pixel OnObject = 0;
  const Pixel Background = 255;

  // find starting point

  int w=bm.AskWidth(), h=bm.AskHeight();

  const Pixel*const* p = bm.AskFrame();

  int startx,starty;
  for (int y=0;y<h;y++)
    for (int x=0;x<w;x++)
      if (p[y][x]==0) // found an "on"-pixel
	{ startx=x; starty=y; goto found; }

  cerr << "did not find any object in the input shape\n";
  exit(5);

 found:
  //Image<Pixel> disp;
  //ChangeColorspace(disp, MakeImage(bm), Colorspace_RGB);
  
  // start to scan the contour

  int x=startx, y=starty;
  int dir=0; // current direction: go right

  for (;;)
    {
      trace.Append(Point2D<int>(x,y));
      //DrawPoint(disp,x,y, Color<Pixel>(255,0,0));
      //DisplayX11(disp);

      int dir_begin = (dir-2+8)%8;
      int dir_end   = (dir+4) +1;
      bool found=false;

      for (int d=dir_begin; d!=dir_end; d=(d+1)%8)
	{
	  int nx = x+dir_vector[d].dx;
	  int ny = y+dir_vector[d].dy;
	  if (nx<0 || nx>=w || ny<0 || ny>=h)
	    continue;

	  //cout << "x/y: " << x << "/" << y << " -> nx/ny " << nx << "/" << ny << endl;

	  if (p[ny][nx]==OnObject)
	    {
	      //cout << "[" << d << "] " << nx << " " << ny << endl;

	      x=nx; y=ny;
	      dir=d;
	      found=true;
	      break;
	    }
	}

      if (!found)
	{
	  cerr << "Outline tracing got stuck! "
	       << "This should not happen except we have only a single pixel contour.\n";
	  exit(5);
	}

      if (x==startx && y==starty)
	break;
    }
}



DynArray<double> GenerateIntegralInvariantsVector(const Bitmap<Pixel>& shape,
						  const DynArray<Point2D<int> >& trace,
						  double radius)
{
  DynArray<double> featurevec;

  int sz = trace.AskSize();
  int w=shape.AskWidth(), h=shape.AskHeight();
  const Pixel*const* p = shape.AskFrame();

  int iRadius = (int)floor(radius);

  for (int i=0;i<sz;i++)
    {
      int px = trace[i].x;
      int py = trace[i].y;

      double sum=0;
      double area=0.0;

      for (int dy=-iRadius;dy<=iRadius;dy++)
	{
	  if (py+dy<0 || py+dy>=h) continue;

	  int dx = (int)sqrt(radius*radius - dy*dy);
	  area += 2*dx+1;

	  int startx=px-dx;
	  int endx  =px+dx;
	  if (startx<0) startx=0;
	  if (endx>=w) endx=w-1;

	  for (int x=startx;x<=endx;x++)
	    if (p[py+dy][x]==0)
	      sum+=1.0;
	}

      featurevec[i] = sum/area;
      //cout << i << " " << featurevec[i] << endl;
    }

  return featurevec;
}

#if 0
int main(int argc,char** argv)
{
  try
    {
      UnifiedImageLoader reader;
      reader.SetTargetColorspace(Colorspace_Greyscale);
      reader.SetInput(argv[1]);

      Image<Pixel> shape;
      reader.ReadImage(shape);
      BinaryThreshold(shape.AskBitmapY(),(Pixel)128,(Pixel)0,(Pixel)255);

      DynArray<Point2D<int> > trace;
      TraceContour(shape.AskBitmapY(), trace);
      DynArray<double> vec = GenerateIntegralInvariantsVector(shape.AskBitmapY(), trace, 10.0);
    }
  catch(const Excpt_Base& e)
    {
      MessageDisplay::Show(e);
    }

  return 0;
}
#endif
