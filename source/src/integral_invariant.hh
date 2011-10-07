
#ifndef INTEGRAL_INVARIANT_HH
#define INTEGRAL_INVARIANT_HH

#include <libvideogfx.hh>

using namespace videogfx;

void TraceContour(const Bitmap<Pixel>& bm, DynArray<Point2D<int> >& trace);
DynArray<double> GenerateIntegralInvariantsVector(const Bitmap<Pixel>& shape,
						  const DynArray<Point2D<int> >& trace,
						  double radius);

#endif
