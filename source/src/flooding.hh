
#ifndef FLOODING_HH
#define FLOODING_HH

#include "type_cost.hh"
#include "type_cell.hh"
#include "util_path.hh"



/* Floods from startrow until startrow+height.
*/
template <Topology topology, class CostMatrixT, class NodeMatrixT> class Flooding
{
public:
  /* Flood the matrix with valid start nodes in the range [rowRangeStart;rowRangeEnd].
     End-nodes will be in the same range.
     Often, one will set the range to only a single node (rowRangeStart==rowRangeEnd).
     A range is used, e.g., for the Branch-and-bound algorithm.
   */
  static void Flood_Unrestricted(const CostMatrixT& cost, NodeMatrixT& nodes,
				 int rowRangeStart, int rowRangeEnd);


  // NEW ->
  static void Flood_Unrestricted(const EdgeCostsT& cost, CellMatrixT& nodes,
				 int firstStartRow, int lastStartRow,
				 int firstFloodRow, int lastFloodRow);
  /*
    if (cost.hasNE()) computeRangeFirst--;
    if (cost.hasSE() || cost.hasS()) computeRangeLast++;
   */


  /* Flood the whole area between (and including) the two paths.
     This function also fills the touched-path information.
   */
  static void Flood_Bounded(const CostMatrixT&, NodeMatrixT&,
			    int startrow,
			    const ColumnPath& abovepath,
			    const ColumnPath& belowpath);

  /* Flood between the two paths, starting at the 'startpoint', which must be in both paths.
     Flooding is in reverse direction (i.e., from right to left).
   */
  static void Flood_RightToLeft_Bounded(const CostMatrixT&, NodeMatrixT&,
					const ColumnPath& abovepath,
					const ColumnPath& belowpath,
					const Position& startpoint);
};

//  template <class CostMatrix, class NodeMatrix>
//  void Flood_Torus_Unrestricted(const CostMatrix&, NodeMatrix&, int startrow, int endrow=-1);

/* cNorm = c%w; cNorm1 = (c-1)%w
 */

template <class Cell, class CostMatrix>
inline void fillNode(NodeMatrix<Cell>& nodes, const CostMatrix& cost, int r, int c, int cNorm, int cNorm1);

/* (not exported)
template <class Cell, class CostMatrix>
inline void fillNode_Torus(NodeMatrix<Cell>& nodep, const CostMatrix& cost, int r, int c, int cNorm, int cNorm1);

template <class Cell, class CostMatrix>
inline void fillNode_Cylinder(NodeMatrix<Cell>& nodep, const CostMatrix& cost, int r, int c, int cNorm1);
*/

#include "flooding.inl"

#endif
