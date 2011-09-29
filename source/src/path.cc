
#include "path.hh"

#include <assert.h>
#include <iostream>

using namespace std;


inline bool within(int n,int above,int below)
{
  return above<=n && n<=below;
}


bool Touching(const Path& abovepath, const Path& belowpath)
{
  const int w = abovepath.getWidth();

  for (int c=0;c<w;c++)
    if (abovepath.bottomRow(c) >= belowpath.topRow(c)) return true;

  return false;
}

Position TouchPoint(const Path& abovepath, const Path& belowpath)
{
  const int w = abovepath.getWidth();

  /* Determine touching point.
   */

  int touching_column=-1;
  for (int c=0;c<w;c++)
    {
      assert(abovepath.topRow(c) <= belowpath.topRow(c));

      //cout << c << ": " <<  belowpath.toprow[c] << "<=" << abovepath.bottomrow[c] << endl;

      if (belowpath.topRow(c) <= abovepath.bottomRow(c))
	//if (within(belowpath.toprow[c], abovepath.toprow[c], abovepath.bottomrow[c]))
	{ touching_column=c; break; }
    }

  /*
  std::cout << "touch... ";
  ShortPrintPath(abovepath); cout << " ";
  ShortPrintPath(belowpath); cout << "\n";
  */

  assert(touching_column>=0);
  int touching_row = /*std::max(abovepath.toprow[touching_column],*/ (belowpath.topRow(touching_column));

  return Position(touching_column, touching_row);
}


Path AlignStrictlyBelow(const Path& abovepath, const Path& belowpath)
{
  Path newbelow = belowpath;
  int w = belowpath.getWidth();

  for (int c=0;c<w;c++)
    {
      if (newbelow.topRow(c)    < abovepath.topRow(c))    newbelow.topRow(c)   =abovepath.topRow(c);
      if (newbelow.bottomRow(c) < abovepath.bottomRow(c)) newbelow.bottomRow(c)=abovepath.bottomRow(c);
    }

  return newbelow;
}


void PrintPath(const Path& p)
{
  int w = p.getWidth();

  for (int x=0;x<w;x++)
    {
      cout << x << " / " << p.topRow(x);
      if (p.topRow(x) != p.bottomRow(x)) { cout << "---" << p.bottomRow(x); }
      cout << endl;
    }
}


void ShortPrintPath(const Path& p)
{
  int w = p.getWidth();

  cout << "[ "
       << p.topRow(0  ); if (p.topRow(0  )!=p.bottomRow(0  )) { cout << "-" << p.bottomRow(0  ); } cout << " ; "
       << p.topRow(1  ); if (p.topRow(1  )!=p.bottomRow(1  )) { cout << "-" << p.bottomRow(1  ); } cout << " ... "
       << p.topRow(w-2); if (p.topRow(w-2)!=p.bottomRow(w-2)) { cout << "-" << p.bottomRow(w-2); } cout << " ; "
       << p.topRow(w-1); if (p.topRow(w-1)!=p.bottomRow(w-1)) { cout << "-" << p.bottomRow(w-1); } cout << " ]";
}




