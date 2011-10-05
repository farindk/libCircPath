

template <Topology topology, class EdgeCostsT, class Cell>
Path SCP_DynProg<topology, EdgeCostsT, Cell>::computeMinCostCircularPath()
{
  typedef typename Cell::IndexType Index;

  Index w=m_edges->getWidth(), h=m_edges->getHeight();

  Path bestpath;
  for (Index y=0;y<h;y++)
    {
      Path path = computeMinCostCircularPath(y);
      
      if (y==0 || path.cost<bestpath.cost)
	bestpath=path;

      // logging output: print result for each start-node and draw path in visualization

      if (isLoggingEnabled())
	{
	  cout << "y:" << y << " : " << path.cost << " ";
	  ShortPrintPath(path); cout << "\n";

	  Visualizer* v = getVisualizer();
	  if (v)
	    {
	      v->prepareVisualization();
	      v->drawPath(path, 0xffc000);
	      v->showVisualization(true);
	    }
	}
    }

  return bestpath;
}


#if 1
template <Topology topology, class EdgeCostsT, class Cell>
Path SCP_DynProg<topology, EdgeCostsT, Cell>::computeMinCostCircularPath(int startrow)
{
  int w=m_edges->getWidth(), h=m_edges->getHeight();

  switch (topology)
    {
    case Torus:
      {
	m_matrix.create(w+1, h*2,1);
	int floodStart = (EdgeCostsT::hasNE ? 0 : startrow);
	int floodEnd   = (startrow+h);

	Flooding<EdgeCostsT,CellMatrix<Cell> >::Flood_Unrestricted(*m_edges,m_matrix,
								   startrow,startrow, floodStart,floodEnd);
	return ExtractPath(m_matrix, startrow+h);
      }

    case Cylinder:
      m_matrix.create(w+1, h  ,1);
      Flooding<EdgeCostsT,CellMatrix<Cell> >::Flood_Unrestricted(*m_edges,m_matrix, startrow,startrow, 0,h-1);
      return ExtractPath(m_matrix, startrow);
    }
}
#endif


#if 0
// partial template specialization not supported in C++ ...

template <class EdgeCostsT, class Cell>
Path SCP_DynProg<Torus, EdgeCostsT, Cell>::computeMinCostCircularPath(int startrow)
{
  int w=m_edges->getWidth(), h=m_edges->getHeight();

  m_matrix.create(w+1, h*2,1);
  Flooding<EdgeCostsT,CellMatrix<Cell> >::Flood_Unrestricted(*m_edges,m_matrix, startrow,startrow, 0,h-1);
  return ExtractPath(m_matrix, startrow);
}


template <class EdgeCostsT, class Cell>
Path SCP_DynProg<Cylinder, EdgeCostsT, Cell>::computeMinCostCircularPath(int startrow)
{
  int w=m_edges->getWidth(), h=m_edges->getHeight();

  m_matrix.create(w+1, h  ,1);
  Flooding<EdgeCostsT,CellMatrix<Cell> >::Flood_Unrestricted(*m_edges,m_matrix, startrow,startrow, 0,h-1);
  return ExtractPath(m_matrix, startrow+h);
}
#endif
