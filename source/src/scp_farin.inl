
// TODO: dual-float (see scp_farin_cyl.cc) , also for Maes

#include "scp_dynprog.hh"

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <assert.h>

static Log l;


template <Topology topology, class EdgeCostsT>
Path SCP_Farin<topology,EdgeCostsT>::extractPath_FromTwoSides(int endrow, const Position& touching_point)
{
  l.enter("extractPath_FromTwoSides");

  int w = m_edges->getWidth();
  int h = m_edges->getHeight();

  if (DBG) cout << l.lhs() << "ExtractPath_FromTwoSides[" << endrow << "] left: " << endrow-m_rowOffset
		<< " right: " << endrow << " (height=" << h << ")" << endl;

  Path path_left  = ExtractPath_Left2Right(m_matrix2, endrow-m_rowOffset);
  Path path_right = ExtractPath           (m_matrix,  endrow);

  /* Compose path of path_left + path_right.
     Everything before the splitting point is taken from path_left, the remaining path from path_right.
     In the touching column, this means that the toprow is from the left part and the bottomrow from the right part.
   */
  Path path = path_left;
  path.bottomRow(touching_point.x) = path_right.bottomRow(touching_point.x);
  for (int c=touching_point.x+1;c<=w;c++)
    {
      path.topRow(c)    = path_right.topRow(c);
      path.bottomRow(c) = path_right.bottomRow(c);
    }

  /*
  if (DBG) cout << l.lhs() << "cost parts: +"
		<< path_left.cost  << " -" << nodep2[touching_point.x][touching_point.y].costsum << endl
		<< l.lhs() << "            +"
		<< path_right.cost << " -" << nodep [touching_point.x][touching_point.y].costsum << endl
		<< l.lhs() << "            +" << cp[touching_point.y%h][touching_point.x] << endl;
  */

  //assert(d_nodes2(touching_point.x,touching_point.y).costSum==0);

  path.cost = (+ path_left.cost  - m_matrix2(touching_point.x, touching_point.y).costSum
	       + path_right.cost - m_matrix (touching_point.x, touching_point.y).costSum);
  //+ (*d_cost)(touching_point.x,touching_point.y%h));

  //if (DBG) cout << l.lhs() << "cost: " << path.cost << " (check=" << PathCost(path, d_cost) << ")" << endl;
  if (DBG)
    {
      //assert(abs(path.cost-PathCost(path, d_cost)) < 0.001);
    }

  l.leave();
  return path;
}


template <Topology topology, class EdgeCostsT>
Path SCP_Farin<topology,EdgeCostsT>::findBestTwoPartPath(int startrow, int endrow,
							 const Position& touching_point)
{
  const int w = m_edges->getWidth();
  const int h = m_edges->getHeight();

  CostSum mincost;
  int bestpath=-1;

  for (int r=startrow;r<=endrow;r++)
    {
      //assert(d_nodes2(touching_point.x,touching_point.y).costSum==0);

      CostSum cost = (m_matrix2(0,r).costSum + m_matrix(w,r+m_rowOffset).costSum
		      -m_matrix (touching_point.x,touching_point.y).costSum
		      -m_matrix2(touching_point.x,touching_point.y).costSum);
      if (r==startrow || cost<mincost) { mincost=cost; bestpath=r; }

      if (isLoggingEnabled())
	{
	  cout << "row " << r << " cost " << cost << endl;

	  Path p = extractPath_FromTwoSides(r+m_rowOffset, touching_point);
	  cout << "y:" << r << " : " << cost << " "; ShortPrintPath(p); cout << "\n";
	}
    }

  return extractPath_FromTwoSides(bestpath+m_rowOffset, touching_point);
}


/*
  Process area between two non-touching paths.

  This function guarantees to add the mincost path for the range [toppath;bottompath) into 'computed_paths'.

  We maintain a list of paths computed so far. One of these paths will be the final solution.
  The path originate from two sources: the subdivision-paths when we cut the graph, and the
  concatenated paths when solving for a complete range.
 */
template <Topology topology, class EdgeCostsT>
void SCP_Farin<topology,EdgeCostsT>::rangeRecurse(const Path& top_path,
						  const Path& bottom_path)
{
  l.enter("RangeRecurse");

#if DBG
  char prefix[100];
  sprintf(prefix, "%sRangeRecurse[%d:%d] ",l.lhs(), top_path.StartRow(), bottom_path.StartRow());
  sprintf(prefix,l.lhs());
#else
  const char* prefix="";
#endif

  const int w=m_edges->getWidth();
  const int h=m_edges->getHeight();

  // bounding paths should end in the same node as they start (be circular)
  assert(top_path   .startRow() == top_path   .endRow()-m_rowOffset);
  assert(bottom_path.startRow() == bottom_path.endRow()-m_rowOffset);

#if DBG
  assert( !Touching(top_path, bottom_path) );
#endif

  /* If the two bounding paths are tight, we cannot subdivide.
     But both paths should already be in the list of computed paths anyway.
  */
  if (bottom_path.startRow() <= top_path.startRow()+1)
    {
      //computed_paths.Append(top_path);
      l.leave();
      return;
    }


  /* Top and bottom paths do not touch.
     We flood the area inbetween, starting from the center row.

     TODO: check: could also be toppath.toprow[0]+... , but this would probably
     result in a similar path as the previous toppath. hence, using bottomrow[0]
     could be a bit better.
  */
  int center_row = (top_path.startRow()+bottom_path.startRow())/2;

  /* TODO: search for the column with the largest distance between toprow and bottomrow.
     Then shift the matrix by this amount. This should reduce the problem into two
     parts with parts as small as possible.
   */

  if (isLoggingEnabled())
    {
      std::cout << "handle (non-touching) area between\n"
	   << "  the top path    "; ShortPrintPath(top_path);
      std::cout << std::endl;
      std::cout << "  the bottom path "; ShortPrintPath(bottom_path); std::cout << std::endl;
      std::cout << "-> the center path starts at " << center_row << std::endl;
    }

  // Make a dyn-programming flooding between (including) toppath and bottompath, starting at the center row.

#if DBG
  cout << prefix << "Flooding[->] between ";
  ShortPrintPath(top_path); cout << " and ";
  ShortPrintPath(bottom_path); cout << endl;
#endif

  /* note: the min() in the next line is required for the special case that toppath.toprow[w-1] is above the center path.
     Since the first argument is limiting the area that is filled in the DP-array, ...
     Wait, I do it the other way round...
   */
  Flooding<EdgeCostsT,CellMatrix<Cell> >::Flood_Bounded(*m_edges, m_matrix, center_row, top_path, bottom_path);

  const int center_endrow = center_row + m_rowOffset;
  assert(center_endrow >= top_path.bottomRow(w));   // <<<< --- maybe this is not really required, but should still be true, because of the computation of the center-path above
  assert(center_endrow <= bottom_path.bottomRow(w));
  Path center_path = ExtractPath(m_matrix, center_endrow);

  // append just computed center-path to the list of computer circular paths
  addCandidatePath(center_path);

#if DBG
  cout << prefix << "extracted center path "; ShortPrintPath(center_path);
  cout << " +" << h << "x" << n_wraparounds << endl;
#endif

  //if (d_enable_logging) PrintNodeInfo(d_nodes, toppath, bottompath);

  /* If we have three tightly packed paths (consecutive rows),
     simply add the center path to the list of paths.
     TODO: check: I think that this case is not really needed, because above, it is
     checked for two tight bounding paths anyway.
  */

  /*
  if (top_path.StartRow() == bottom_path.StartRow()-2 &&
      top_path.EndRow()   == bottom_path.EndRow()  -2)
    {
      assert(center_path.StartRow() == top_path.StartRow()+1);
      assert(center_path.EndRow()   == top_path.EndRow()  +1);

#if DBG
      cout << prefix << "top,center,bottom are tightly packed. Do not recurse.\n";
#endif

      //computed_paths.Append(bottompath);
      l.leave();
      return;
    }
  */

  /* Determine the row-ranges for which the paths touch either the
     top or bottom bounding-paths. For these ranges, the solution can
     be computed directly.

     Note:
     The while-loops should terminate because toppath does not touch bottompath.
     However, since there might be several paths with the same cost, there might
     be a path similar to bottompath that also touches toppath. For this reason,
     we have to limit the search to a maximum row.

     PS: forget it, we have to end the search at the center row, because this
     defines the maximum range that we can solve directly.

     In the 'while's below, we have to start searching at toppath.toprow, and
     bottompath.bottomrow, because toppath.bottomrow might already be on the
     other side of the center-path. Likewise for bottompath.
  */

  const int topPath_endrow    = top_path.endRow();
  const int bottomPath_endrow = bottom_path.endRow();

  int topTouch_endrow;    // last row on the right side for which the path touches toppath
  int bottomTouch_endrow; // first row on the right side for which the path touches bottompath

  topTouch_endrow = top_path.endRow(); // REWORK, WAS: toprow[w];
  while (m_matrix(w, topTouch_endrow+1).touchedAbove && topTouch_endrow < center_endrow) topTouch_endrow++;

  bottomTouch_endrow = bottom_path.endRow(); // REWORK, WAS: bottomrow[w];
  while (m_matrix(w, bottomTouch_endrow-1).touchedBelow && bottomTouch_endrow>center_endrow) bottomTouch_endrow--;

  Path topTouch_path    = ExtractPath(m_matrix,topTouch_endrow);
  Path bottomTouch_path = ExtractPath(m_matrix,bottomTouch_endrow);

#if DBG
  cout << prefix << " topTouch_path:    "; ShortPrintPath(topTouch_path);    cout << endl;
  cout << prefix << " bottomTouch_path: "; ShortPrintPath(bottomTouch_path); cout << endl;
#endif

  if (isLoggingEnabled())
    {
      cout << "center_endrow: " << center_endrow
	   << " topTouch_endrow: " << topTouch_endrow
	   << " bottomTouch_endrow: " << bottomTouch_endrow << endl;
      cout << "topTouch_path    "; ShortPrintPath(topTouch_path); cout << endl;
      cout << "bottomTouch_path "; ShortPrintPath(bottomTouch_path); cout << endl;

      cout << "touching ranges: "
	   << topPath_endrow << "-----" << topTouch_endrow << "  [" << center_endrow << "]  "
	   << bottomTouch_endrow << "-----" << bottomPath_endrow << endl;
    }


  // computed_paths.Append(center_path);

  Visualizer* v = getVisualizer();
  if (v)
    {
      v->prepareVisualization();
      v->drawArea(top_path, bottom_path, 0x00FF00); //, torus ? center_row : -1);
      v->drawPath(top_path, 0xFFFF00);
      v->drawPath(bottom_path,0xFFFF00);
      v->drawPath(topTouch_path, 0x0000FF);
      v->drawPath(bottomTouch_path, 0x0000FF);
      v->drawPath(center_path, 0xFF0000);
      v->showVisualization(true);
    }


  /* Now compute the new bounding paths at the top and the bottom side, and also
     the minimum-cost path in the handled region.

     We first process both areas before we recurse in order to be able to reuse the node matrix.
  */

  Path newTopBounding_path;
  Path newBottomBounding_path;

  // if the following is set to NULL, we do not recurse
  const Path* recurse_topBoundingPath    = NULL;
  const Path* recurse_bottomBoundingPath = NULL;


  // --- process areas that can be solved directly with a single backwards flooding ---

  const float ratioThreshold = 0.4;

  float topRatio = (topTouch_endrow-topPath_endrow)/float(center_endrow-topPath_endrow);
  //std::cout << topRatio <<"\n";

  /* Check if there is a top-range which could be solved directly with a backwards flow.
     If this is not the case (range only consists of already known top-path, only recurse in the other part.
   */
  if ( /*topTouch_endrow==topPath_endrow ||*/ topRatio <= ratioThreshold)
    {
      if (DBG) cout << prefix << "top solved-area is small (or empty), do not process it\n";
      recurse_topBoundingPath = &top_path;
    }
  else
    {
      const Position touching_point = TouchPoint(top_path,topTouch_path);

      if (DBG)
	{
	  cout << prefix << "top-bound path and last-top path touch at "
	       << touching_point.x << ";" << touching_point.y << endl;
	  cout << prefix << "flood[<-] between ";
	  ShortPrintPath(top_path); cout << " and "; ShortPrintPath(topTouch_path);
	  cout << endl;
	}

      Flooding<EdgeCostsT,CellMatrix<Cell> >::Flood_RightToLeft_Bounded(*m_edges, m_matrix2,
									top_path, topTouch_path, touching_point);

      if (DBG) cout << prefix << "combine left and right part of new top-bounding-path\n";

      newTopBounding_path = extractPath_FromTwoSides(topTouch_endrow, touching_point);
      if (isLoggingEnabled())
	{ cout << "new top bounding path: "; ShortPrintPath(newTopBounding_path); cout << endl; }

      assert(newTopBounding_path.topRow(0)%h == topTouch_endrow%h);
      assert(newTopBounding_path.bottomRow(w) == topTouch_endrow);

      if (DBG) cout << prefix << "get min-cost path\n";
      const Path bestpath = findBestTwoPartPath(top_path.startRow(),newTopBounding_path.startRow(),
						touching_point);

      addCandidatePath(bestpath);

      if (isLoggingEnabled() || DBG) cout << l.lhs() << "best path (range " << top_path.bottomRow(0)
					  << "---" << newTopBounding_path.bottomRow(0) << "): "
					  << bestpath.topRow(0) << "->" << bestpath.bottomRow(w)
					  << " cost=" << bestpath.cost << endl;

      if (PathsAreTouching(newTopBounding_path, center_path))
	{
	  if (center_path.startRow() == newTopBounding_path.startRow())
	    {
	      if (DBG)
		cout << l.lhs() << "center-path and new-top-bounding path are similar. Do not process further.\n";
	    }
	  else
	    {
	      if (DBG)
		cout << l.lhs() << "new-top-bounding path and center-path touch, we can find the solution for "
		     << newTopBounding_path.startRow() << "-" << center_path.startRow() << " immediately\n";

	      Position tp = TouchPoint(newTopBounding_path, center_path);

	      m_matrix2(tp.x,tp.y).prevY = -1;
	      m_matrix2(tp.x,tp.y).prevX = -1;

	      const Path bestpath2 = findBestTwoPartPath(newTopBounding_path.startRow(), center_path.startRow(), tp);
	      addCandidatePath(bestpath2);
	    }
	}
      else
	{
	  assert( topTouch_endrow == newTopBounding_path.endRow() );

	  recurse_topBoundingPath = &newTopBounding_path;
	}
    }


  float bottomRatio = (bottomTouch_endrow-bottomPath_endrow)/float(center_endrow-bottomPath_endrow);
  //std::cout << topRatio <<"\n";

  if (/*bottomTouch_endrow==bottomPath_endrow ||*/ bottomRatio <= ratioThreshold)
    {
      if (DBG) cout << prefix << "bottom solved-area is empty, do not process it\n";
      recurse_bottomBoundingPath = &bottom_path;
    }
  else
    {
      const Position touching_point = TouchPoint(bottomTouch_path, bottom_path);

      Flooding<EdgeCostsT,CellMatrix<Cell> >::Flood_RightToLeft_Bounded(*m_edges, m_matrix2,
									bottomTouch_path,
									bottom_path, touching_point);

      //if (d_enable_logging) PrintNodeInfo(d_nodes2, top_path, bottom_path);

      if (DBG) cout << prefix << "combine left and right part of new bottom-bounding-path\n";

      newBottomBounding_path = extractPath_FromTwoSides(bottomTouch_endrow, touching_point);
      assert(newBottomBounding_path.topRow(0)%h == bottomTouch_endrow%h);

      if (isLoggingEnabled())
	{
	  cout << prefix << "new bottom bounding path: ";
	  ShortPrintPath(newBottomBounding_path); cout << endl;
	}

      const Path bestpath = findBestTwoPartPath(newBottomBounding_path.startRow(),
						bottom_path.topRow(0), touching_point);
      addCandidatePath(bestpath);

      if (isLoggingEnabled() || DBG) cout << prefix
					  << "best path (range " << newBottomBounding_path.topRow(0) << "---"
					  << bottom_path.bottomRow(0) << "): "
					  << bestpath.topRow(0) << "->" << bestpath.bottomRow(w-1)
					  << " cost=" << bestpath.cost << endl;

      if (PathsAreTouching(center_path, newBottomBounding_path))
	{
	  if (center_path.startRow() == newBottomBounding_path.startRow())
	    {
	      if (DBG) cout << l.lhs() << "center-path and new-bottom-bounding path are similar. Do not process further.\n";
	    }
	  else
	    {
	      if (DBG)
		cout << l.lhs() << "center-path and new-bottom-bounding path touch, we can find the solution for "
		     << center_path.startRow() << "-" << newBottomBounding_path.startRow() << " immediately\n";

	      Position tp = TouchPoint(center_path, newBottomBounding_path);
	      if (DBG) cout << l.lhs() << "touch-point=" << tp.x << ";" << tp.y << endl;

	      m_matrix2(tp.x,tp.y).prevY = -1;
	      m_matrix2(tp.x,tp.y).prevX = -1;

	      const Path bestpath2 = findBestTwoPartPath(center_path.startRow(),
							 newBottomBounding_path.startRow(), tp);
	      addCandidatePath(bestpath2);
	    }
	}
      else
	{
	  assert( bottomTouch_endrow == newBottomBounding_path.endRow() );

	  recurse_bottomBoundingPath = &newBottomBounding_path;
	}
    }


  if (recurse_topBoundingPath)
    {
      if (isLoggingEnabled() || DBG) cout << prefix << "RECURSE in top area...\n";
      rangeRecurse(*recurse_topBoundingPath, center_path);
    }

  if (recurse_bottomBoundingPath)
    {
      if (isLoggingEnabled() || DBG) cout << prefix << "RECURSE in bottom area...\n";
      rangeRecurse(center_path, *recurse_bottomBoundingPath);
    }

  l.leave();
}


template <Topology topology, class EdgeCostsT>
Path SCP_Farin<topology,EdgeCostsT>::computeMinCostCircularPath()
{
  typedef typename EdgeCostsT::CostType Cost;

  int w=m_edges->getWidth();
  int h=m_edges->getHeight();


  clearBestPath();

  // === compute a first set of bounding paths (at the very top and very bottom) ===

  Path toppath;
  Path bottompath;

  switch (topology)
    {
    case Torus:
      {
	SCP_DynProg<topology, EdgeCostsT> dynprog;
	dynprog.setEdgeCosts(*m_edges);

	toppath = dynprog.computeMinCostCircularPath(0);
	addCandidatePath(toppath);

	/* Make a lower limiting path by just be copying the upper bounding path
	   down, shifted by 'h'. The area between these two paths define the
	   further working area. Note, that if 'toppath' is already the mincost
	   path, we have a second solution, 'bottompath'. However, it is easier
	   to exclude this solution later than modifying 'bottompath' here.

	   => since 'bottompath' will not be inserted into the min-cost path set,
	   it is not considered as solution.
	*/

	bottompath.makeShiftedCopy(toppath,h);

	m_matrix .create(w+1,2*h, 2);
	m_matrix2.create(w+1,2*h, 2);
	m_rowOffset=h;
      }
      break;

    case Cylinder:
      {
	// Compute two independent paths. One at the top, the other at the bottom.

	// TODO: here, we could gain a little bit by computing both paths at once.
	// We would start at the first and last row at once. If they do not touch,
	// we have saved one pass. If they do touch, then we can still use one of
	// the paths and can recompute the other.

	SCP_DynProg<topology, EdgeCostsT> dynprog;
	dynprog.setEdgeCosts(*m_edges);

	toppath    = dynprog.computeMinCostCircularPath(0);
	bottompath = dynprog.computeMinCostCircularPath(h-1);

	addCandidatePath(toppath);
	addCandidatePath(bottompath);

	m_matrix .create(w+1,h+1, 1);
	m_matrix2.create(w+1,h+1, 1);
	m_rowOffset=0;
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


  // recursively compute paths and collect candidate paths

  rangeRecurse(toppath, bottompath);


  if (isLoggingEnabled())
    {
      std::cout << "---------------------------------------------------------\n";
      std::cout << "Best path is: "; ShortPrintPath(m_bestPath); std::cout << endl;
      std::cout << "cost= " << m_bestPath.cost << endl;
    }

  return m_bestPath;
}


template <Topology topology, class EdgeCostsT>
void SCP_Farin<topology,EdgeCostsT>::printNodeInfo(const CellMatrix<Cell>& nodes,
						   const Path& abovepath,
						   const Path& belowpath) const
{
#if 000
  int w = d_cost->AskWidth();
  int h = d_cost->AskHeight();

  int nodeArrayHeight = (torus ? 2*h : h);

  for (int y=0;y<nodeArrayHeight;y++)
    {
      cout << setw(2) << y << "| ";
      for (int x=0;x<w;x++)
	{
	  if (y<abovepath.toprow[x] ||
	      y>belowpath.bottomrow[x])
	    cout << "               ";
	  else
	    {
	      cout << setw(4) << nodes(x,y).costSum << " "
		   << setw(2) << nodes(x,y).prevY << ";"
		   << setw(2) << nodes(x,y).prevX << " "
		   << (nodes(x,y).touchedAbove ? 'A':'a')
		   << (nodes(x,y).touchedBelow ? 'B':'b') << "  ";
	    }
	}

      cout << endl;
    }
#endif
}
