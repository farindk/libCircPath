
#include "input.hh"
#include "integral_invariant.hh"
#include "path.hh"

extern "C" {
#include <gsl/gsl_spline.h>
}


// --------- Image Matrix ----------


void InputSource_Image_Matrix::Load(const char* filename, Bitmap<Pixel>& cost)
{
  Image<Pixel> img;

  UnifiedImageLoader reader;
  reader.SetInput(filename);
  reader.SetTargetColorspace(Colorspace_Greyscale);
  reader.ReadImage(img);

  int w=img.AskWidth(), h=img.AskHeight();

  cost = img.AskBitmapY();

  //cost.Create(w,h);
  //ConvertBitmap(cost, img.AskBitmapY());
}


// --------- uniform noise ----------


string InputSource_UniformNoise::GenerateNodes(Bitmap<Cost>& cost)
{
  assert(d_w>0);

  cost.Create(d_w,d_h);

  for (int y=0;y<d_h;y++)
    for (int x=0;x<d_w;x++)
      cost[y][x] = rand()%256;

  char buf[100];
  sprintf(buf,"uniform-noise[%d/%d]",d_n,d_nmatrices);

  return string(buf);
}


// --------- uniform noise ----------


string InputSource_Synthetic::GenerateNodes(Bitmap<Cost>& cost)
{
  cost.Create(d_w,d_h);

  double* v1 = new double[d_w];
  double* v2 = new double[d_h];

  for (int i=0;i<d_w;i++)
    {
      double a=i*M_PI*2/d_w + M_PI/3;
      v1[i] = sin(a) + 0.7*cos(2*a) + 0.3*sin(3*a);
    }

  for (int i=0;i<d_h;i++)
    {
      double a=i*M_PI*2/d_h + M_PI/3;
      v2[i] = cos(a) + 1.2*sin(4*a) + 1.4*cos(5*a);
    }

  for (int y=0;y<d_h;y++)
    for (int x=0;x<d_w;x++)
      cost[y][x]=(v1[x]-v2[y])*(v1[x]-v2[y]);

  return "synthetic";
}

// ---------------------------------


DynArray<double> Interpolate(const DynArray<double>& vec, int size)
{
  int n = vec.AskSize();

  double* y = new double[n];
  double* x = new double[n];

  for (int i=0;i<n;i++)
    {
      x[i]=i;
      y[i]=vec[i];
    }

  gsl_interp_accel* acc = gsl_interp_accel_alloc();
  gsl_spline* spline = gsl_spline_alloc(gsl_interp_cspline_periodic, n);

  gsl_spline_init(spline,x,y,n);
  DynArray<double> newvec;
  newvec[size-1]=0.0; // extend to final size

  for (int i=0;i<size;i++)
    {
      double pos = i*n/size;
      newvec[i] = gsl_spline_eval(spline,pos,acc);
    }

  delete[] x;
  delete[] y;

  gsl_spline_free(spline);
  gsl_interp_accel_free(acc);

  return newvec;
}


// --------- gaussian shape ----------


string InputSource_GaussianShape::GenerateNodes(Bitmap<Cost>& cost)
{
  assert(d_w>0);
  assert(d_h>0);
  double *shape1 = new double[d_w];
  double *shape2 = new double[d_h];
  gsl_rng * r = gsl_rng_alloc (gsl_rng_taus);


  for (int x=0;x<d_w;x++)
    shape1[x] = gsl_ran_gaussian(r, d_deviation);
  for (int y=0;y<d_h;y++)
    shape2[y] = gsl_ran_gaussian(r, d_deviation);

  cost.Create(d_w,d_h);

  for (int y=0;y<d_h;y++)
    for (int x=0;x<d_w;x++)
      cost[y][x] = (shape1[x]-shape2[y])*(shape1[x]-shape2[y]);

  delete shape1;
  delete shape2;

  char buf[100];
  sprintf(buf,"uniform shapes [%d x %d]",d_n,d_nmatrices);

  return string(buf);
}


// --------- end of gaussian shape ----------


InputSource_Shapes::InputSource_Shapes(int skip)
  : d_w(0), d_h(0), d_skip(skip), d_allpairs(false), d_rotation(false)
{
}

void InputSource_Shapes::Init(const char *const *inputs, int inputs_num)
{
  d_inputs = inputs;
  d_num_inputs = inputs_num; 
  d_next_x=-1; d_next_y=-1;
}

void InputSource_Shapes::EnableRotation(bool flag)
{
  d_rotation = flag;
}

void InputSource_Shapes::InitIndex()
{
  d_next_x=d_next_y=0;

  if (!d_allpairs) Next();
}

bool InputSource_Shapes::Next()
{
  if (d_next_x<0) InitIndex();

  d_next_x+=d_skip;
  if (d_next_x>=d_num_inputs)
    {
      if (d_allpairs)
	{
	  d_next_x=0;
	  d_next_y+=d_skip;
	}
      else
	{
	  d_next_y+=d_skip;
	  d_next_x=d_next_y+d_skip;
	}
    }

  if (d_next_x >= d_num_inputs) return false;
  return d_next_y < d_num_inputs;
}

string InputSource_Shapes::GenerateNodes(Bitmap<Cost>& cost)
{
  if (d_next_x<0) InitIndex();

  UnifiedImageLoader reader;
  reader.SetTargetColorspace(Colorspace_Greyscale);

  Image<Pixel> shape1, shape2;

  reader.SetInput(d_inputs[d_next_x]);
  reader.ReadImage(shape1);
  BinaryThreshold(shape1.AskBitmapY(),(Pixel)128,(Pixel)0,(Pixel)255);

  reader.SetInput(d_inputs[d_next_y]);
  reader.ReadImage(shape2);
  BinaryThreshold(shape2.AskBitmapY(),(Pixel)128,(Pixel)0,(Pixel)255);

  DynArray<Point2D<int> > trace1, trace2;
  TraceContour(shape1.AskBitmapY(), trace1);
  TraceContour(shape2.AskBitmapY(), trace2);
  DynArray<double> vec1 = GenerateIntegralInvariantsVector(shape1.AskBitmapY(), trace1, 10.0);
  DynArray<double> vec2 = GenerateIntegralInvariantsVector(shape2.AskBitmapY(), trace2, 10.0);

  if (d_w) vec1 = Interpolate(vec1, d_w);
  if (d_h) vec2 = Interpolate(vec2, d_h);

  int w = vec1.AskSize();
  int h = vec2.AskSize();
  cost.Create(w,h);
  Cost*const* pc = cost.AskFrame();

  int maxshift = (d_rotation ? h : 0);

  for (int y=0;y<h;y++)
    for (int x=0;x<w;x++)
      {
	double diff = (vec2[y]-vec1[x]);
	pc[(y+h/3 + h-maxshift*x/w)%h][(x+h/4)%w] = diff*diff; // VARIABLE
      }

  return string(d_inputs[d_next_y]) + "-" + string(d_inputs[d_next_x]);
}



void InputSource_DNA::Init(const char *const *inputs, int inputs_num)
{
  assert(inputs_num==2);

  d_seq1 = ReadSeq(inputs[0]);
  d_seq2 = ReadSeq(inputs[1]);
}

string InputSource_DNA::ReadSeq(const char* filename)
{
  string seq;
  ifstream istr(filename);

  for (;;)
    {
      char c;
      istr.get(c);
      if (istr.eof()) break;

      if (c=='A' || c=='C' || c=='G' || c=='T' || c=='X')
	seq = seq+c;
    }

  return seq;
}

string InputSource_DNA::GenerateEdges_Tor(EdgeCosts_Torus<unsigned char>& edges)
{
  d_seq1len = d_seq1.length(); // vertically
  d_seq2len = d_seq2.length(); // horizontally

  edges.create(d_seq2len, d_seq1len);

  for (int x=0;x<d_seq2len;x++)
    for (int y=0;y<d_seq1len;y++)
      {
	if (d_seq1[y] == d_seq2[x] || d_seq1[y]=='X' || d_seq2[x]=='X') { edges.setCostSE(x,y, 0); }
	else { edges.setCostSE(x,y, 1); }
	edges.setCostE(x,y, 1);
	edges.setCostS(x,y, 1);

	//cout << "c:" << x << " r:" << y << " = " << pE[y][x] << ";" << pSE[y][x] << ";" << pS[y][x] << endl;
      }

  return "DNA";
}

struct node
{
  int c,r;
  enum Dir { Right,Diag,Down } dir;
};

void InputSource_DNA::ShowMatching(const Path& p)
{
  std::vector<node> path;

  for (int c=0;c<d_seq2len;c++)
    {
      node n;

      /* If there are several nodes in one column, all but the
	 last one are "down"-edges.
      */

      if (p.topRow(c) != p.bottomRow(c))
	for (int r=p.topRow(c);r<p.bottomRow(c);r++)
	  {
	    n.c=c;
	    n.r=r;
	    n.dir=node::Down;
	    path.push_back(n);
	  }

      /* Check last node in column. If next node is straight to
	 the right, it is a "right"-edge, otherwise diagonal.
      */

      n.c=c;
      n.r=p.bottomRow(c);

      if (n.r%d_seq1len == p.bottomRow((c+d_seq2len+1)%d_seq2len)%d_seq1len)
	n.dir = node::Right;
      else
	n.dir = node::Diag;

      path.push_back(n);
    }

#if 0
  for (int i=0;i<path.AskSize();i++)
    {
      cout << path[i].c << " " << path[i].r << " ";
      if (path[i].dir == node::Right) cout << "-\n";
      if (path[i].dir == node::Down ) cout << "|\n";
      if (path[i].dir == node::Diag ) cout << "\\\n";
    }
#endif

  for (int i=0;i<path.size();i++)
    {
      if (path[i].dir == node::Right) cout << "-";
      else cout << d_seq1[path[i].r%d_seq1len];
    }
  cout << endl;

  for (int i=0;i<path.size();i++)
    {
      if (path[i].dir == node::Down) cout << "-";
      else cout << d_seq2[path[i].c%d_seq2len];
    }
  cout << endl;
}



#if 0
Bitmap<Cost> ConstructWorstCase(int size)
{
  int w=size, h=size;

  Bitmap<Cost> cost;
  cost.Create(w,h);

  for (int y=0;y<h;y++)
    for (int x=0;x<w;x++)
      cost[y][x]=0; //(rand()%10);

  for (int x=0;x<w;x++)
    {
      cost[ x       ][x]=1.0;
      cost[(x+1)  %h][x]=255.0;
      //cost[(x+h/2)%h][x]=0.0;
      //cost[(x+h-3)%h][x]=255.0;
      cost[(x+h-2)%h][x]=0; //rand()%50;
      cost[(x+h-1)%h][x]=255.0;
    }

  for (int y=0;y<h;y++)
    {
      for (int x=0;x<w;x++)
	cout << setw(3) << cost[y][x] << " ";
      cout << endl;
    }

  cerr << "the -W option does not work... forget it.\n";

  return cost;
}
#endif

