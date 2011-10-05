

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

#if 0
      if (d_enable_logging)
	{
	  cout << "y:" << y << " : " << path.cost << endl;

	  if (d_visual)
	    {
	      d_visual->PrepareVisualization();
	      DrawPath(d_visual->d_disp, path, Color<Pixel>(255,100,100));
	      d_visual->ShowVisualization(true);
	    }
	}
#endif
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
      m_matrix.create(w+1, h*2,1);
      Flooding<EdgeCostsT,CellMatrix<Cell> >::Flood_Unrestricted(*m_edges,m_matrix, startrow,startrow, 0,h-1);
      return ExtractPath(m_matrix, startrow);

    case Cylinder:
      m_matrix.create(w+1, h  ,1);
      Flooding<EdgeCostsT,CellMatrix<Cell> >::Flood_Unrestricted(*m_edges,m_matrix, startrow,startrow, 0,h-1);
      return ExtractPath(m_matrix, startrow+h);
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
