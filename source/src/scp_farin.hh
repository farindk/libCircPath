
#ifndef SCP_FARIN_HH
#define SCP_FARIN_HH

#include "type_costtraits.hh"
#include "type_cell.hh"
#include "type_edges.hh"
#include "path.hh"
#include "flooding.hh"
#include "logging.hh"


template <Topology topology, class EdgeCostsT>
class SCP_Farin : public AlgorithmLogging
{
public:
  typedef typename EdgeCostsT::CostType Cost;
  typedef typename CostTraits<Cost>::SumType CostSum;
  typedef Cell_Touched<CostSum> Cell;

  SCP_Farin() { }
  ~SCP_Farin() { }

  // --- parameters ---

  void setEdgeCosts(const EdgeCostsT& c) { m_edges=&c; }

  // --- high-level interface ---

  Path computeMinCostCircularPath();

private:
  const EdgeCostsT* m_edges;

  CellMatrix<Cell> m_matrix;
  CellMatrix<Cell> m_matrix2;


  int m_rowOffset;

  Path m_bestPath;
  std::vector<Path> m_candidatePaths;

  void clearBestPath() { m_bestPath.cost=-1.0; }
  void addCandidatePath(const Path& path) {
    if (m_bestPath.cost<0 || path.cost < m_bestPath.cost)
      { m_bestPath=path; }
  }


  // --- sub-routines ---

  Path extractPath_FromTwoSides(int endrow, const Position& touching_point);
  Path findBestTwoPartPath(int startrow, int endrow, const Position& touching_point);

  void rangeRecurse(const Path& toppath,
		    const Path& bottompath);

  // debugging

  void printNodeInfo(const CellMatrix<Cell>& nodes,
		     const Path& abovepath,
		     const Path& belowpath) const;
};

#include "scp_farin.inl"

#endif
