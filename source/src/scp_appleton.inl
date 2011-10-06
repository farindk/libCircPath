
#include <iostream>


template <Topology topology, class EdgeCostsT, class Cell>
inline bool SCP_Appleton<topology,EdgeCostsT,Cell>::source_set::operator<(const source_set& b) const
{
  return best_path.cost > b.best_path.cost;
}


template <Topology topology, class EdgeCostsT, class Cell>
typename SCP_Appleton<topology, EdgeCostsT, Cell>::source_set
SCP_Appleton<topology, EdgeCostsT, Cell>::createSet(int start,int end)
{
  typedef typename Cell::CostSumType CostSum;

  const int w=m_cost->getWidth(), h=m_cost->getHeight();
  const int endOffset = (topology==Torus ? h : 0);

  if (isLoggingEnabled()) std::cout << "compute path for start set [" << start << ";" << end << "]\n";

  /**/ if (topology==Torus)    m_matrix.create(w+1, h*2,1);
  else if (topology==Cylinder) m_matrix.create(w+1, h+1,1);

  Flooding<EdgeCostsT,CellMatrix<Cell> >::Flood_Unrestricted(*m_cost, m_matrix, start,end, 0,h+endOffset-1);

  CostSum   mincost = m_matrix(w, start+endOffset).costSum;
  int       bestendrow = start+endOffset;

  for (int r=start+1;r<=end;r++)
    {
      CostSum c = m_matrix(w, r+endOffset).costSum;
      if (c<=mincost) { mincost=c; bestendrow=r+endOffset; }
    }

  source_set sset;
  sset.startrow  = start;
  sset.endrow    = end;
  sset.best_path = ExtractPath(m_matrix, bestendrow);
  sset.circular  = (sset.best_path.startRow() == sset.best_path.endRow()-endOffset);

  if (isLoggingEnabled()) std::cout << "best path = "
				    << sset.best_path.startRow() << "-"
				    << sset.best_path.endRow() << " with cost=" << sset.best_path.cost
				    << (sset.circular ? " CIRCULAR\n" : "\n");

  Visualizer* v = getVisualizer();
  if (v)
    {
      int boundarycolor     = 0xFF0000;
      int pathcolor_circ    = 0x00FF00;
      int pathcolor_noncirc = 0xFFFF00;

      v->prepareVisualization();
      v->markRange(start,end, endOffset, boundarycolor);
      v->drawPath(sset.best_path, sset.circular ? pathcolor_circ : pathcolor_noncirc);
      v->showVisualization();
    }

  return sset;
}


template <Topology topology, class EdgeCostsT, class Cell>
Path SCP_Appleton<topology, EdgeCostsT, Cell>::computeMinCostCircularPath()
{
  int w=m_cost->getWidth(), h=m_cost->getHeight();

  std::vector<source_set> heap;

  source_set initset;
  initset.startrow = 0;
  initset.endrow = h-1;
  initset.circular = false;
  initset.best_path.cost = -1; // just some non-sense value
  heap.push_back(initset);

  for (;;)
    {
      source_set sset = heap.front();
      std::pop_heap(heap.begin(),heap.end());
      heap.pop_back();

      if (isLoggingEnabled())
	std::cout << "retrieve node [" << sset.startrow << ";" << sset.endrow << "] with bound "
		  << sset.best_path.cost << "\n";

      if (sset.circular)
	return sset.best_path;

      int centerrow = (sset.startrow+sset.endrow)/2;

      heap.push_back(createSet(sset.startrow, centerrow));   std::push_heap(heap.begin(),heap.end());
      heap.push_back(createSet(centerrow+1,   sset.endrow)); std::push_heap(heap.begin(),heap.end());
    }

  assert(0);
}
