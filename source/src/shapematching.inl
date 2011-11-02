#include "shapematching.hh"
#include "type_edges.hh"
#include "scp_farin.hh"

#define SQRT05 0.7071067811865475244008443621


/***  VERTEX_BASED  *********************************************************/
template <class CostType>
void ShapeMatching<CostType, VERTEX_BASED>::setDissimilarityMatrix(
        int n, CostType *dissimilarity) {
  NodeCosts_Torus<double> cost;
  cost.create(n,n);
  int nodeI = 0;
  for (int r=0; r<n; r++) {
    for (int c=0; c<n; c++,nodeI++) {
      cost.setCost(c,r, dissimilarity[nodeI]);
    }
  }
  
  SCP_Farin<Torus, NodeCosts_Torus<CostType> > iccv07;
  iccv07.setEdgeCosts(cost);
  matching = iccv07.computeMinCostCircularPath();
}



/***  EDGE_BASED  ***********************************************************/
template <class CostType>
void ShapeMatching<CostType, EDGE_BASED>::setDissimilarityMatrix(
        int n, CostType *dissimilarity) {
  EdgeCosts_Torus<double> cost;
  cost.create(n,n);
  int num   = n*n;
  int nodeI = 0;
  for (int r=0; r<n; r++) {
    for (int c=0; c<n; c++,nodeI++) {
      int nodeH = ((c+1)%n)*n+( r     );
      int nodeD = ((c+1)%n)*n+((r+1)%n);
      int nodeV = ( c     )*n+((r+1)%n);
      CostType H = 0.5*   (dissimilarity[nodeI]+dissimilarity[nodeH%num]);
      CostType D = SQRT05*(dissimilarity[nodeI]+dissimilarity[nodeD%num]);
      CostType V = 0.5*   (dissimilarity[nodeI]+dissimilarity[nodeV%num]);
      cost.setCostE (c,r, H);
      cost.setCostSE(c,r, D);
      cost.setCostS (c,r, V);
    }
  }
  
  SCP_Farin<Torus, EdgeCosts_Torus<CostType> > iccv07;
  iccv07.setEdgeCosts(cost);
  matching = iccv07.computeMinCostCircularPath();
}

