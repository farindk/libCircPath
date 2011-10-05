
#include "scp_dynprog.hh"

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <assert.h>

using namespace std;



/*
  Process two non-touching paths.

  We maintain a list of paths computed so far. One of these paths will be the final solution.
  The path originate from two sources: the subdivision-paths when we cut the graph, and the
  concatenated paths when solving for a complete range.
 */
template <Topology topology, class EdgeCostsT, class Cell>
void SCP_Maes<topology,EdgeCostsT,Cell>::rangeRecurse(Path& bestpath,
						      const Path& toppath,
						      const Path& bottompath)
{
  int w=m_edges->getWidth();
  int h=m_edges->getHeight();

  int endRowOffset;

  switch (topology)
    {
    case Torus:
      m_matrix.create(w+1,2*h, 2);
      endRowOffset = h;
      break;

    case Cylinder:
      m_matrix.create(w+1,h+1, 1);
      endRowOffset = 0;
      break;
    }

  assert(toppath   .startRow()%h == toppath   .endRow()%h);
  assert(bottompath.startRow()%h == bottompath.endRow()%h);

  // if no path fits inbetween top-path and bottom-path, we cannot subdivide further
  if (bottompath.startRow() - toppath.startRow() <= 1)
    return;

  /* Top and bottom paths do not touch.
     We flood the area inbetween, starting from the center row.
  */

  int center_row = (toppath.startRow()+bottompath.startRow())/2;

  if (isLoggingEnabled())
    {
      std::cout << "handle (non-touching) area between between\n"
		<< "  the top path    "; ShortPrintPath(toppath);
      std::cout << "  the bottom path "; ShortPrintPath(bottompath);
      std::cout << "-> the center path starts at " << center_row << endl;
    }

  Flooding<EdgeCostsT,CellMatrix<Cell> >::Flood_Bounded(*m_edges, m_matrix, center_row, toppath, bottompath);

  Visualizer* v = getVisualizer();
  if (v)
    {
      v->prepareVisualization();
      v->drawArea(toppath, bottompath, 0x00FF00); //, torus ? center_row : -1);
      v->showVisualization(true);
    }

  Path center_path = ExtractPath(m_matrix, center_row + endRowOffset);

  if (center_path.cost < bestpath.cost)
    bestpath = center_path;

  rangeRecurse(bestpath,toppath,center_path);
  rangeRecurse(bestpath,center_path,bottompath);
}


template <Topology topology, class EdgeCostsT, class Cell>
Path SCP_Maes<topology, EdgeCostsT, Cell>::computeMinCostCircularPath()
{
  int w=m_edges->getWidth();
  int h=m_edges->getHeight();

  Path toppath;
  Path bottompath;
  Path bestpath;

  switch (topology)
    {
    case Torus:
      {
	SCP_DynProg<topology, EdgeCostsT> dynprog;
	dynprog.setEdgeCosts(*m_edges);

	toppath = dynprog.computeMinCostCircularPath(0);
	bestpath = toppath;

	bottompath.makeShiftedCopy(toppath,h);
      }
      break;

    case Cylinder:
      {
	SCP_DynProg<topology, EdgeCostsT> dynprog;
	dynprog.setEdgeCosts(*m_edges);

	toppath    = dynprog.computeMinCostCircularPath(0);
	bottompath = dynprog.computeMinCostCircularPath(h-1);

	bestpath = (toppath.cost < bottompath.cost) ? toppath : bottompath;
      }
      break;
    }

  if (isLoggingEnabled())
    {
      std::cout << "cost of 0-0 path: " << toppath.cost << "\n";
    }

  Visualizer* v = getVisualizer();
  if (v)
    {
      v->prepareVisualization();
      v->drawPath(toppath   ,0xFFFF00);
      v->drawPath(bottompath,0xFFFF00);
      v->showVisualization();
    }

  rangeRecurse(bestpath, toppath,bottompath);

  if (isLoggingEnabled())
    {
      std::cout << "---------------------------------------------------------\n";
      std::cout << "Best path is: "; ShortPrintPath(bestpath);
      std::cout << "cost= " << bestpath.cost << endl;
    }

  return bestpath;
}


template <Topology topology, class EdgeCostsT, class Cell>
void SCP_Maes<topology, EdgeCostsT,Cell>::printNodeInfo(const CellMatrix<Cell>& nodes,
							const Path& abovepath,
							const Path& belowpath) const
{
  int w = m_edges->getWidth();
  int h = m_edges->getHeight();

  for (int y=0;y<2*h;y++)
    {
      std::cout << setw(2) << y << "| ";
      for (int x=0;x<w+1;x++)
	{
	  if (y<abovepath.topRow(x) ||
	      y>belowpath.bottomRow(x))
	    std::cout << "               ";
	  else
	    {
	      std::cout << std::setw(4) << nodes(x,y).costSum << " "
			<< std::setw(2) << nodes(x,y).prevY << ";"
			<< std::setw(2) << nodes(x,y).prevX << "  ";
	    }
	}

      std::cout << endl;
    }
}
