
#ifndef SCP_MAES_HH
#define SCP_MAES_HH

#include "type_costtraits.hh"
#include "type_cell.hh"
#include "type_edges.hh"
#include "path.hh"
#include "flooding.hh"
#include "logging.hh"


/*
template <class EdgeCostsT, class NodeMatrix>
void Flood_Torus_Bounded(const CostMatrix&, NodeMatrix&, int startrow,
			 const ColumnPath& abovepath,
			 const ColumnPath& belowpath);
*/


template <Topology topology, class EdgeCostsT,
	  class Cell = Cell_Std<typename CostTraits<typename EdgeCostsT::CostType>::SumType> >
class SCP_Maes : public AlgorithmLogging
{
public:
  typedef typename EdgeCostsT::CostType Cost;
  //typedef typename Cell::IndexType Index;

  SCP_Maes() { }
  ~SCP_Maes() { }

  // --- parameters ---

  void setEdgeCosts(const EdgeCostsT& c) { m_edges=&c; }

  // --- low-level interface ---

  // --- high-level interface ---

  Path computeMinCostCircularPath();

private:
  const EdgeCostsT* m_edges;
  CellMatrix<Cell> m_matrix;

  // --- sub-routines ---

  void rangeRecurse(Path& computed_path,
		    const Path& toppath,
		    const Path& bottompath);

  // debugging

  void printNodeInfo(const CellMatrix<Cell>& nodes,
		     const Path& abovepath,
		     const Path& belowpath) const;
};

#include "scp_maes.inl"

#endif
