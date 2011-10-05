
#ifndef SCP_DYNPROG_HH
#define SCP_DYNPROG_HH

#include "type_costtraits.hh"
#include "type_cell.hh"
#include "type_edges.hh"
#include "path.hh"
#include "flooding.hh"


template <Topology topology, class EdgeCostsT,
	  class Cell = Cell_Std<typename CostTraits<typename EdgeCostsT::CostType>::SumType> >
class SCP_DynProg // : public AlgorithmLogging
{
public:
  typedef typename EdgeCostsT::CostType Cost;
  //typedef typename Cell::IndexType Index;

  SCP_DynProg() { m_edges=0; }
  ~SCP_DynProg() { }

  // --- parameters ---

  void setEdgeCosts(const EdgeCostsT& c) { m_edges=&c; }

  // --- high-level interface ---

  Path computeMinCostCircularPath();

  // compute minimum-cost circular path from a specific start-row
  Path computeMinCostCircularPath(int startrow);

private:
  const EdgeCostsT* m_edges;
  CellMatrix<Cell> m_matrix;
};

#include "scp_dynprog.inl"

#endif
