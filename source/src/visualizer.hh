
#ifndef VISUALIZER_HH
#define VISUALIZER_HH

#include "path.hh"


class Visualizer
{
public:
  virtual ~Visualizer() { };

  enum GraphType { Cylinder, Torus /*, FourQuadrants*/ };

#if 0
  template <class Cost> void Initialize(const Bitmap<Cost>& costmatrix, GraphType type=Torus);
  template <class Cost> void Initialize(const Bitmap<Cost>& costmatrix, bool segmentation_graph, bool fourquadrants)
  {
    /**/ if (fourquadrants)      Initialize(costmatrix, FourQuadrants);
    else if (segmentation_graph) Initialize(costmatrix, Cylinder);
    else                         Initialize(costmatrix, Torus);
  }
#endif

  virtual void prepareVisualization() { }
  virtual char showVisualization(bool wait_for_keypress=true) { }  // returns the key pressed
  virtual void drawPath(const Path& path, int colorHex) { }
};


#endif
