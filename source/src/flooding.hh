
#ifndef FLOODING_HH
#define FLOODING_HH

#include "type_edges.hh"
#include "type_cell.hh"
#include "type_costtraits.hh"
#include "path.hh"



/* Floods from startrow until startrow+height.
*/
template <class EdgeCostsT, class CellMatrixT> class Flooding
{
public:
  /* Flood the matrix with valid start nodes in the range [rowRangeStart;rowRangeEnd].
     End-nodes will be in the same range.
     Often, one will set the range to only a single node (rowRangeStart==rowRangeEnd).
     A range is used, e.g., for the Branch-and-bound algorithm.
   */

  static void FloodColumn(const EdgeCostsT& cost, CellMatrixT& nodes,
			  int c, int cModW, int firstFloodRow, int lastFloodRow,
			  int abovePath=0, int belowPath=0);

  static void Flood_Unrestricted(const EdgeCostsT& cost, CellMatrixT& nodes,
				 int firstStartRow, int lastStartRow,
				 int firstFloodRow, int lastFloodRow);

  /* Flood the whole area between (and including) the two paths.
     This function also fills the touched-path information.
   */
  static void Flood_Bounded(const EdgeCostsT&, CellMatrixT&,
			    int startrow,
			    const Path& abovepath,
			    const Path& belowpath);

  /* Flood between the two paths, starting at the 'startpoint', which must be in both paths.
     Flooding is in reverse direction (i.e., from right to left).
   */
  static void Flood_RightToLeft_Bounded(const EdgeCostsT&, CellMatrixT&,
					const Path& abovepath,
					const Path& belowpath,
					const Position& startpoint);
};


template <class Cell, class EdgeCostsT>
inline void fillNode(CellMatrix<Cell>& nodes, const EdgeCostsT& cost, int r, int c, int rModH, int cModW);

template <class Cell, class EdgeCostsT>
inline void fillNode_reverse(CellMatrix<Cell>& nodes, const EdgeCostsT& cost, int r, int c);

#include "flooding.inl"

#endif
