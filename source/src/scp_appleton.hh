
#ifndef SCP_APPLETON_HH
#define SCP_APPLETON_HH

#include "type_costtraits.hh"
#include "type_cell.hh"
#include "type_edges.hh"
#include "path.hh"
#include "flooding.hh"
#include "logging.hh"


struct source_set
{
  int startrow, endrow;
  bool circular;
  Path best_path;
};

bool operator<(const source_set& a,
	       const source_set& b);
  

template <Topology topology, class EdgeCostsT,
	  class Cell = Cell_Std<typename CostTraits<typename EdgeCostsT::CostType>::SumType> >
class SCP_Appleton : public AlgorithmLogging
{
public:
  SCP_Appleton() { }
  ~SCP_Appleton() { }

  // --- parameters ---

  void setEdgeCosts(const EdgeCostsT& c) { m_cost=&c; }

  // --- high-level interface ---

  Path computeMinCostCircularPath();

private:
  const EdgeCostsT* m_cost;
  CellMatrix<Cell> m_matrix;

  source_set createSet(int start,int end);
};

#include "scp_appleton.inl"

#endif
