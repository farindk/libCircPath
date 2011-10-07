
//#include "input.hh"
//#include "integral_invariant.hh"
//#include "util_path.hh"

extern "C" {
  //#include <gsl/gsl_spline.h>
}


template <class Cost>
void InputSource_InputFile<Cost>::Init(const char *const *inputs, int inputs_num)
{
  d_inputs = inputs;
  d_num_inputs = inputs_num;
  d_next = 0;
  d_output_w=0; d_output_h=0;  // 0 means to keep original input size
  d_cache_full=false;
}

template <class Cost>
void InputSource_InputFile<Cost>::SetSize(int w,int h)
{
  d_output_w=w;
  d_output_h=h;
}

template <class Cost>
string InputSource_InputFile<Cost>::GenerateNodes(Bitmap<Cost>& data)
{
  assert(d_next<d_num_inputs);

  if (!d_cache_full)
    { Load(d_inputs[d_next], d_cache); d_cache_full=true; }

  if (d_output_w>0 &&
      (d_cache.AskWidth() != d_output_w || d_cache.AskHeight() != d_output_h))
    { AssertDescr(false, "resizing not implemented yet"); exit(20); }

  data = d_cache.Clone();

  return d_inputs[d_next];
}

template <class Cost>
bool InputSource_InputFile<Cost>::Next()
{
  d_next++;
  d_cache_full=false;
  return d_next<d_num_inputs;
}


// --------- ASCII Matrix ----------


template <class Cost>
void InputSource_ASCII_Matrix<Cost>::Load(const char* filename, Bitmap<Cost>& cost)
{
  ifstream istr(filename);

  double wd,hd;
  istr >> wd >> hd;
  int w = (int)wd;
  int h = (int)hd;

  cost.Create(w,h);

  for (int y=0;y<h;y++)
    for (int x=0;x<w;x++)
      istr >> cost[y][x];
}
