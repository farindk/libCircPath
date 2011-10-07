
#ifndef INPUT_HH
#define INPUT_HH

// #include "util_types.hh"
#include <libvideogfx.hh>
#include <string>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

#include "type_edges.hh"

using namespace videogfx;
using namespace std;


template <class Cost>
class WeightFactory_Nodes
{
public:
  virtual string GenerateNodes(Bitmap<Cost>& data) = 0; // return data name
};

template <class Cost>
class WeightFactory_Cylinder
{
public:
  virtual string GenerateEdges_Cyl(EdgeCosts_Cylinder<Cost>& edges) = 0;
};

template <class Cost>
class WeightFactory_Torus
{
public:
  virtual string GenerateEdges_Tor(EdgeCosts_Torus<Cost>& edges) = 0;
};


class InputSource
{
public:
  virtual ~InputSource() { }

  virtual void Init(const char *const *inputs, int inputs_num) { }
  virtual void SetSize(int w,int h) = 0;

  virtual void Reset() = 0; // start iterating through the data from the beginning
  virtual bool Next() = 0; // returns false if no more data
};



template <class Cost>
class InputSource_InputFile : public InputSource, public WeightFactory_Nodes<Cost>
{
public:
  void Init(const char *const *inputs, int inputs_num);
  void SetSize(int w,int h);

  void Reset() { d_next=0; }
  bool Next();

  string GenerateNodes(Bitmap<Cost>& data);

protected:
  virtual void Load(const char* filename, Bitmap<Cost>&) = 0;

private:
  const char*const* d_inputs;
  int d_num_inputs;
  int d_next;

  int d_output_w, d_output_h;

  Bitmap<Cost> d_cache;
  bool d_cache_full;
};



template <class Cost>
class InputSource_ASCII_Matrix : public InputSource_InputFile<Cost>
{
protected:
  void Load(const char* filename, Bitmap<Cost>& cost);
};


class InputSource_Image_Matrix : public InputSource_InputFile<Pixel>
{
protected:
  void Load(const char* filename, Bitmap<Pixel>& cost);
};


class InputSource_UniformNoise : public InputSource, public WeightFactory_Nodes<float>
{
public:
  typedef float Cost;

  InputSource_UniformNoise(int nmatrices) : d_w(0), d_h(0), d_nmatrices(nmatrices), d_n(0) { }

  void SetSize(int w,int h) { d_w=w; d_h=h; }

  void Reset() { d_n=0; }
  bool Next() { d_n++; return d_n<d_nmatrices; }

  string GenerateNodes(Bitmap<Cost>& cost);

private:
  int d_w,d_h;
  int d_nmatrices, d_n;
};


class InputSource_GaussianShape : public InputSource, public WeightFactory_Nodes<float>
{
public:
  typedef float Cost;

  InputSource_GaussianShape(double deviation, int nmatrices) :
    d_w(0), d_h(0), d_deviation(deviation), d_nmatrices(nmatrices), d_n(0) { }

  void SetSize(int w,int h) { d_w=w; d_h=h; }
  void Reset() { d_n=0; }
  bool Next() { d_n++; return d_n<d_nmatrices; }
  string GenerateNodes(Bitmap<Cost>& cost);

private:
  int d_w,d_h;
  int d_nmatrices, d_n;
  double d_deviation;
};



class InputSource_Synthetic : public InputSource, public WeightFactory_Nodes<float>
{
public:
  typedef float Cost;

  InputSource_Synthetic() : d_w(0), d_h(0), d_nmatrices(1), d_n(0) { }

  void SetSize(int w,int h) { d_w=w; d_h=h; }
  void Reset() { d_n=0; }
  bool Next() { d_n++; return false; }
  string GenerateNodes(Bitmap<Cost>& cost);

private:
  int d_w,d_h;
  int d_n, d_nmatrices;
};



class InputSource_Shapes : public InputSource, public WeightFactory_Nodes<float>
{
public:
  typedef float Cost;

  InputSource_Shapes(int skip);

  void Init(const char *const *inputs, int inputs_num);
  void EnableRotation(bool flag=true); // shift columns to one revolution, such that a segmentation-graph solution "looks like" a DTW-solution
  void EnableAllPairs(bool e) // enable all possible pairs (each pair (symmetrically) twice and pairs of twice the same)
  { d_allpairs=e; }
  void SetSize(int w,int h) { d_w=w; d_h=h; }
  void Reset() { InitIndex(); }
  bool Next();
  string GenerateNodes(Bitmap<Cost>& cost);

private:
  void InitIndex();

  int d_w,d_h;
  bool d_rotation;

  const char*const* d_inputs;
  int d_num_inputs;
  int d_next_x, d_next_y;
  int d_skip;
  bool d_allpairs;
};


class InputSource_DNA : public InputSource, public WeightFactory_Torus<unsigned char>
{
public:
  InputSource_DNA() { }

  void Init(const char *const *inputs, int inputs_num);
  void SetSize(int w,int h) { assert(0); }

  void Reset() { }
  bool Next() { return false; }

  string GenerateEdges_Tor(EdgeCosts_Torus<unsigned char>& edges);

  void ShowMatching(const class Path& p);

private:
  string d_seq1, d_seq2;
  int d_seq1len, d_seq2len;

  static string ReadSeq(const char* filename);
};


#include "input.inl"

#endif
