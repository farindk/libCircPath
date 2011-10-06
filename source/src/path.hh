
#ifndef PATH_HH
#define PATH_HH

#include "type_basic.hh"
#include "type_cell.hh"
#include "type_matrix.hh"

#include <assert.h>
#include <vector>

#include <iostream>

/*
  Stores a path in such a way that it is efficient to query
  through which rows it passes in a specific column.
  Restrictions of applicaton:
  - the path may only pass through a successive sequence of rows.
  - the path must go from lower rows to higher rows only.

  Usage:
  The range of rows through which the path goes for a column is
  stored in toprow[]-bottomrow[]. Hence, it enters the column in
  toprow[] and leaves it again in bottomrow[].
 */
struct Path
{
  struct range { int toprow,bottomrow; };

  std::vector<range> mPath;
  double cost; // total cost along this path (should be of generic type "Cost", but for simplicity, we set it to "double")

  int& topRow(int c)    { return mPath[c].toprow; }
  int& bottomRow(int c) { return mPath[c].bottomrow; }

  int topRow(int c)    const { return mPath[c].toprow; }
  int bottomRow(int c) const { return mPath[c].bottomrow; }

  void create(int width)
  {
    mPath.resize(width);

    setRows(-1);
  }

  void setRows(int value)
  {
    size_t width = mPath.size();
    for (size_t i=0;i<width;i++)
      mPath[i].toprow = mPath[i].bottomrow = value;
  }

  void makeShiftedCopy(const Path& p, int offset)
  {
    const int w = p.getWidth();
    mPath.resize(w);
    cost = p.cost;

    for (int x=0;x<w;x++)
      {
	mPath[x].toprow    = p.mPath[x].toprow   +offset;
	mPath[x].bottomrow = p.mPath[x].bottomrow+offset;
      }
  }

  int getWidth() const { return mPath.size(); }

  int startRow() const { return mPath[0].toprow; }
  int endRow() const   { return mPath.back().bottomrow; }
  bool isOnPath(int r,int c) const { return r>=mPath[c].toprow && r<=mPath[c].bottomrow; }
};


template <class Cell>  Path ExtractPath(const CellMatrix<Cell>& nodes, int row_in_last_column);
template <class Cell>  Path ExtractPath_Left2Right(const CellMatrix<Cell>& nodes, int row_in_first_column);

bool PathsAreTouching(const Path& abovepath, const Path& belowpath);
Position TouchPoint(const Path& abovepath, const Path& belowpath);


/* Returns modified belowpath, so that it is always below abovepath. This is to
   correct situations in which belowpath is sometimes above abovepath, when
   there is an alternative subpath with the same cost.
*/
Path AlignStrictlyBelow(const Path& abovepath, const Path& belowpath);


template <class EdgeCosts> double PathCost(const Path& p, const EdgeCosts& cost);

void PrintPath(const Path&);
void ShortPrintPath(const Path&);


// ------------ implementation ---------------


template <class Cell>  Path ExtractPath(const CellMatrix<Cell>& nodes, int row_in_last_column)
{
  typedef typename Cell::IndexType Index;

  //const Cell*const* nodep = nodes.AskFrame();
  //Index h = nodes.AskHeight();
  Index w = nodes.getWidth(); // the padded width (== width of cost array +1)

  Path path;

  path.create(w);
  path.cost = nodes(w-1,row_in_last_column).costSum;

  // start traceback at indicated node in last column

  Index x = w-1;
  Index y = row_in_last_column;

  for (;;)
    {
      // enter node into the path

      if (path.topRow(x)<0) // first node in this column -> set "start" and "end" to same value
	path.topRow(x) = path.bottomRow(x) = y;
      else // another node in the column -> modify top ("start" of column), since we are tracing backwards
	path.topRow(x) = y;

      // go to previous node

      Index newX = nodes(x,y).prevX;
      Index newY = nodes(x,y).prevY;

      if (newX==-1) break;
      assert(newX==x || newX==x-1);

      x=newX;
      y=newY;
    }

  return path;
}


template <class Cell>  Path ExtractPath_Left2Right(const CellMatrix<Cell>& nodes, int row_in_first_column)
{
  typedef typename Cell::IndexType Index;

  //const Cell*const* nodep = nodes.AskFrame();
  //Index h = nodes.AskHeight();
  int w = nodes.getWidth(); // the padded width (== width of cost array +1)

  Path path;

  path.create(w);
  path.cost = nodes(0,row_in_first_column).costSum;

  // start traceback at indicated node in last column

  Index x = 0;
  Index y = row_in_first_column;

  for (;;)
    {
      // enter node into the path

      if (path.topRow(x)<0) // first node in this column -> set "start" and "end" to same value
	path.topRow(x) = path.bottomRow(x) = y;
      else // another node in the column -> modify bottom ("end" of column), since we are tracing forewards
	path.bottomRow(x) = y;

      // go to previous node

      Index newX = nodes(x,y).prevX;
      Index newY = nodes(x,y).prevY;

      //std::cout << x << ";" << y << " -> " << newX << ";" << newY << "\n";

      if (newX==-1) break;
      assert(newX==x || newX==x+1);

      x=newX;
      y=newY;
    }

  return path;
}


#if 0
template <class CostMatrix>
double PathCost(const Path& p, const CostMatrix& cost)
{
  typedef typename CostMatrix::CostType Cost;

  int w = p.getWidth();
  int h = cost.getHeight();

  //typename CostTraits<typename CostMatrix::CostType>::SumType costsum=0;
  double costsum=0;
  for (int x=0;x<w+1;x++)
    {
      const int top = p.topRow(x);
      const int bottom = p.bottomRow(x);

      for (int y=top;y<=bottom;y++)
	costsum += cost.costS(x%w,y);
    }

  return costsum;
}
#endif


#endif
