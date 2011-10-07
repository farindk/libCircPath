/*
  NOTES:
  - small problem: the edges connecting left and right border are just straight, there
  . exist no diagonal edges along the cut. It seems that this cannot be easily be solved
  . by adding a copy of the first column to the right side. Also not when the weights are set
  . to zero.
 */

//long boundtime; /// TMP HACK
//extern long boundtime;

#define TODO 0

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <libvideogfx.hh>
#include <assert.h>
#include <sys/time.h>
#include <time.h>

#include <boost/shared_ptr.hpp>

using namespace std;
using namespace videogfx;

#include "path.hh"
/*
#include "util_shapedbitmap.hh"
#include "scp_dijkstra_cyl.hh"
#include "scp_appleton_cyl.hh"
#include "scp_farin_heap.hh"
#include "scp_farinheap_cyl.hh"
#include "scp_farin_cyl.hh"
#include "scp_farin_cyl2.hh"
#include "scp_farin2.hh"
#include "scp_farinbb.hh"
#include "scp_farinbb_cyl.hh"
#include "scp_maes_cyl.hh"
#include "scp_maes_edges_tor.hh"
#include "scp_maesrot_edges_tor.hh"
#include "scp_skewtest.hh"
#include "scp_dynprog_edges_cyl.hh"
#include "scp_dynprog_edges_tor.hh"
#include "scp_farin3_cyl.hh"
#include "scp_dynprog_cyl.hh"
*/
#include "input.hh"

#include "scp_dynprog.hh"
#include "scp_maes.hh"
#include "scp_appleton.hh"
#include "scp_farin.hh"
//#include "scp_dijkstra.hh"
#include "visualizer_videogfx.hh"

#include "cmdline.h"

struct gengetopt_args_info args_info;

Visualizer_VideoGfx visual;


void ExtractSizeIncr(char* spec,int& wadd,int& hadd,int& maxw,int& maxh)
{
  wadd=hadd=0;
  maxw=maxh=0;

  char* p = spec;
  char op1 = *p++;  while (isdigit(*p)) { wadd*=10; wadd+=*p-'0'; p++; }
  char op2 = *p++;  while (isdigit(*p)) { hadd*=10; hadd+=*p-'0'; p++; }
  char delim1 = *p++;  while (isdigit(*p)) { maxw*=10; maxw+=*p-'0'; p++; }  assert(delim1 == ':');
  char delim2 = *p++;  while (isdigit(*p)) { maxh*=10; maxh+=*p-'0'; p++; }  assert(delim2 == 'x');

  assert(op1=='+');
  assert(op2=='+');

  // cout << wadd << " " << hadd << " " << maxw << " " << maxh << endl;
}


void test_show_input()
{
  if (args_info.visualize_given)
    {
      visual.prepareVisualization();
      visual.showVisualization();
    }
}


#if 000
ColumnPath test_skewtest(const Bitmap<Cost>& cost)
{
  int w=cost.AskWidth(), h=cost.AskHeight();

  SCP_SkewTest_E_SE_S_Cyclic skewtest;
  skewtest.SetCostMatrix(cost);

  if (args_info.visualize_given) skewtest.SetVisualizationOutput(&visual);
  if (args_info.logging_given)   skewtest.EnableLogging(true);

  //skewtest.Flood2();
  ColumnPath path = skewtest.ComputeMinCostCircularPath();
  //ColumnPath path = dijkstra.ComputeMinCostCircularPath();



  // just avoid breaking the program when no solution is computed ...

  if (0)
    {
      SCP_Appleton_E_SE_S_Cyclic appleton;
      appleton.SetCostMatrix(cost);
      ColumnPath path = appleton.ComputeMinCostCircularPath();
      return path;
    }

  if (0)
    {
      SCP_DynProg_E_SE_S_Cyclic dynprog;
      dynprog.SetCostMatrix(cost);
      ColumnPath path = dynprog.ComputeMinCostCircularPath_RealCyclic();
      return path;
    }

  //ColumnPath path;
  //path.Create(w);
  return path;
}
#endif


void InitVisualLogging(AlgorithmLogging& algo)
{
  if (args_info.visualize_given) algo.setVisualizer(&visual);
  if (args_info.logging_given)   algo.enableLogging(true);
}


#if TODO
template <class CostProv, class CostSum>
ColumnPath test_dynprog_singlepath_tor(const CostProv& costProv)
{
  SCP_DynProg< true, CostProv, Cell_Torus_Std<CostSum> > dynprog;
  dynprog.SetCostMatrix(costProv);
  InitVisualLogging(dynprog);

  ColumnPath path = dynprog.ComputeMinCostCircularPath(0);

  return path;
}
#endif

// if (strcmp(algo, "dynprog" )==0) return test_algorithm<EdgeCostsT,SCP_DynProg <Torus,EdgeCostsT> >(costMatrix);

template <class EdgeCostsT, class Algo>
Path test_algorithm(const EdgeCostsT& costProv)
{
  Algo algo;
  algo.setEdgeCosts(costProv);
  InitVisualLogging(algo);

  Path path = algo.computeMinCostCircularPath();

  return path;
}


#if 0
template <class CostProv, class CostSum>
ColumnPath test_dynprog(const CostProv& costProv)
{
  return test_algorithm<CostProv, CostSum, SCP_DynProg_Torus< CostProv > > (costProv);
  //return test_algorithm<CostProv, CostSum, SCP_DynProg_Torus< CostProv, Cell_Torus_Std<CostSum> > > (costProv);
}
#endif


#if 000
ColumnPath test_dynprog_edges_cyl(const Bitmap<Cost> cost[3])
{
  int w=cost[1].AskWidth(), h=cost[1].AskHeight();

  SCP_DynProg_Edges_Cyl dynprog;
  dynprog.SetEdgeCosts(cost[0],cost[1],cost[2]);

  if (args_info.visualize_given) dynprog.SetVisualizationOutput(&visual);
  if (args_info.logging_given)   dynprog.EnableLogging(true);

  ColumnPath path = Path2ColumnPath(dynprog.ComputeMinCostCircularPath());

  return path;
}

ColumnPath test_dynprog_edges_tor(const Bitmap<Cost> cost[3])
{
  int w=cost[1].AskWidth(), h=cost[1].AskHeight();

  SCP_DynProg_Edges_Torus dynprog;

  dynprog.SetEdgeCosts(cost[0],cost[1],cost[2]);

  if (args_info.visualize_given) dynprog.SetVisualizationOutput(&visual);
  if (args_info.logging_given)   dynprog.EnableLogging(true);

  ColumnPath path = dynprog.ComputeMinCostCircularPath();

  return path;
}


ColumnPath test_maes_edges_tor(const Bitmap<Cost> cost[3])
{
  int w=cost[1].AskWidth(), h=cost[1].AskHeight();

  SCP_Maes_Edges_Torus maes;

  maes.SetEdgeCosts(cost[0],cost[1],cost[2]);

  if (args_info.visualize_given) maes.SetVisualizationOutput(&visual);
  if (args_info.logging_given)   maes.EnableLogging(true);

  ColumnPath path = maes.ComputeMinCostCircularPath();

  return path;
}


ColumnPath test_maesrot_edges_tor(const Bitmap<Cost> cost[3])
{
  int w=cost[1].AskWidth(), h=cost[1].AskHeight();

  SCP_MaesRot_Edges_Torus maes;

  maes.SetEdgeCosts(cost[0],cost[1],cost[2]);

  if (args_info.visualize_given) maes.SetVisualizationOutput(&visual);
  if (args_info.logging_given)   maes.EnableLogging(true);

  ColumnPath path = maes.ComputeMinCostCircularPath();

  return path;
}


ColumnPath test_cyl_dynprog_brute_force(const Bitmap<Cost>& cost)
{
  int w=cost.AskWidth(), h=cost.AskHeight();

  SCP_DynProg_NE_E_SE_Cyclic dynprog;
  dynprog.SetCostMatrix(cost);

  if (args_info.visualize_given) dynprog.SetVisualizationOutput(&visual);
  if (args_info.logging_given)   dynprog.EnableLogging(true);

  ColumnPath path = Path2ColumnPath(dynprog.ComputeMinCostCircularPath());

  return path;
}


ColumnPath test_cyl_dynprog_singlepath(const Bitmap<Cost>& cost)
{
  int w=cost.AskWidth(), h=cost.AskHeight();

  SCP_DynProg_NE_E_SE_Cyclic dynprog;
  dynprog.SetCostMatrix(cost);

  if (args_info.visualize_given) dynprog.SetVisualizationOutput(&visual);
  if (args_info.logging_given)   dynprog.EnableLogging(true);

  ColumnPath path = Path2ColumnPath(dynprog.ComputeMinCostCircularPath(0));

  return path;
}


ColumnPath test_appleton(const Bitmap<Cost>& cost)
{
  int w=cost.AskWidth(), h=cost.AskHeight();

  SCP_Appleton_E_SE_S_Cyclic appleton;
  appleton.SetCostMatrix(cost);

  if (args_info.visualize_given) appleton.SetVisualizationOutput(&visual);
  if (args_info.logging_given)   appleton.EnableLogging(true);

  ColumnPath path = appleton.ComputeMinCostCircularPath();

  return path;
}


ColumnPath test_cyl_appleton(const Bitmap<Cost>& cost)
{
  int w=cost.AskWidth(), h=cost.AskHeight();

  SCP_Appleton_NE_E_SE_Cyclic appleton;
  appleton.SetCostMatrix(cost);

  if (args_info.visualize_given) appleton.SetVisualizationOutput(&visual);
  if (args_info.logging_given)   appleton.EnableLogging(true);

  ColumnPath path = Path2ColumnPath(appleton.ComputeMinCostCircularPath());

  return path;
}


ColumnPath test_farinbb(const Bitmap<Cost>& cost)
{
  int w=cost.AskWidth(), h=cost.AskHeight();

  SCP_FarinBB_E_SE_S_Cyclic farinbb;
  farinbb.SetCostMatrix(cost);

  if (args_info.visualize_given) farinbb.SetVisualizationOutput(&visual);
  if (args_info.logging_given)   farinbb.EnableLogging(true);

  /**/ if (strcmp(args_info.splitpoint_arg,"center"     )==0) { farinbb.SetSplitPointPosition(SCP_FarinBB_E_SE_S_Cyclic::SplitPoint_Center); }
  else if (strcmp(args_info.splitpoint_arg,"mincost"    )==0) { farinbb.SetSplitPointPosition(SCP_FarinBB_E_SE_S_Cyclic::SplitPoint_CutMinCost); }
  else if (strcmp(args_info.splitpoint_arg,"alternating")==0) { farinbb.SetSplitPointPosition(SCP_FarinBB_E_SE_S_Cyclic::SplitPoint_Alternating); }
  else { cerr << "unknown splitting method specified\n"; exit(5); }

  ColumnPath path = farinbb.ComputeMinCostCircularPath();

  return path;
}


ColumnPath test_cyl_farinbb(const Bitmap<Cost>& cost)
{
  int w=cost.AskWidth(), h=cost.AskHeight();

  SCP_FarinBB_NE_E_SE_Cyclic farinbb;
  farinbb.SetCostMatrix(cost);

  if (args_info.visualize_given) farinbb.SetVisualizationOutput(&visual);
  if (args_info.logging_given)   farinbb.EnableLogging(true);

  /**/ if (strcmp(args_info.splitpoint_arg,"center"     )==0) { farinbb.SetSplitPointPosition(SCP_FarinBB_NE_E_SE_Cyclic::SplitPoint_Center); }
  else if (strcmp(args_info.splitpoint_arg,"mincost"    )==0) { farinbb.SetSplitPointPosition(SCP_FarinBB_NE_E_SE_Cyclic::SplitPoint_CutMinCost); }
  else if (strcmp(args_info.splitpoint_arg,"alternating")==0) { farinbb.SetSplitPointPosition(SCP_FarinBB_NE_E_SE_Cyclic::SplitPoint_Alternating); }
  else { cerr << "unknown splitting method specified\n"; exit(5); }

  ColumnPath path = Path2ColumnPath(farinbb.ComputeMinCostCircularPath());

  return path;
}


ColumnPath test_cyl_farin3(const Bitmap<Cost>& cost)
{
  int w=cost.AskWidth(), h=cost.AskHeight();

  SCP_Farin3_Cylinder farin3;
  farin3.SetCostMatrix(cost);

  if (args_info.visualize_given) farin3.SetVisualizationOutput(&visual);
  if (args_info.logging_given)   farin3.EnableLogging(true);

  ColumnPath path = Path2ColumnPath(farin3.ComputeMinCostCircularPath());

  return path;
}


ColumnPath test_farin(const Bitmap<Cost>& cost)
{
  SCP_Farin_E_SE_S_Cyclic algo_farin;

  if (args_info.visualize_given) algo_farin.SetVisualizationOutput(&visual);
  if (args_info.logging_given)   algo_farin.EnableLogging(true);

  algo_farin.SetCostMatrix(cost);
  ColumnPath path = algo_farin.ComputeMinCostCircularPath();

  return path;
}


ColumnPath test_farin_heap(const Bitmap<Cost>& cost)
{
  SCP_FarinHeap_E_SE_S_Cyclic algo_farin;

  if (args_info.visualize_given) algo_farin.SetVisualizationOutput(&visual);
  if (args_info.logging_given)   algo_farin.EnableLogging(true);

  algo_farin.SetCostMatrix(cost);
  ColumnPath path = algo_farin.ComputeMinCostCircularPath();

  return path;
}


ColumnPath test_cyl_farin_heap(const Bitmap<Cost>& cost)
{
  SCP_FarinHeap_NE_E_SE_Cyclic algo_farin;

  if (args_info.visualize_given) algo_farin.SetVisualizationOutput(&visual);
  if (args_info.logging_given)   algo_farin.EnableLogging(true);

  algo_farin.SetCostMatrix(cost);
  ColumnPath path = Path2ColumnPath(algo_farin.ComputeMinCostCircularPath());

  return path;
}


ColumnPath test_cyl_farin(const Bitmap<Cost>& cost)
{
  SCP_Farin_NE_E_SE_Cyclic algo_farin;

  if (args_info.visualize_given) algo_farin.SetVisualizationOutput(&visual);
  if (args_info.logging_given)   algo_farin.EnableLogging(true);

  algo_farin.SetCostMatrix(cost);
  ColumnPath path = Path2ColumnPath(algo_farin.ComputeMinCostCircularPath());

  return path;
}

ColumnPath test_cyl_rot_farin(const Bitmap<Cost>& cost)
{
  //SCP_Farin_NE_E_SE_Cyclic algo_farin;
  SCP_Farin2_NE_E_SE_Cyclic algo_farin;

  if (args_info.visualize_given) algo_farin.SetVisualizationOutput(&visual);
  if (args_info.logging_given)   algo_farin.EnableLogging(true);

  algo_farin.SetCostMatrix(cost);
  ColumnPath path = Path2ColumnPath(algo_farin.ComputeMinCostCircularPath());

  return path;
}


ColumnPath test_farin2(const Bitmap<Cost>& cost)
{
  SCP_Farin2_E_SE_S_Cyclic algo_farin2;

  if (args_info.visualize_given) algo_farin2.SetVisualizationOutput(&visual);
  if (args_info.logging_given)   algo_farin2.EnableLogging(true);

  algo_farin2.SetCostMatrix(cost);
  ColumnPath path = algo_farin2.ComputeMinCostCircularPath();

  return path;
}


ColumnPath test_maes(const Bitmap<Cost>& cost)
{
  SCP_Maes_E_SE_S_Cyclic algo_maes;

  if (args_info.visualize_given) algo_maes.SetVisualizationOutput(&visual);
  if (args_info.logging_given)   algo_maes.EnableLogging(true);

  algo_maes.SetCostMatrix(cost);
  ColumnPath path = algo_maes.ComputeMinCostCircularPath();

  return path;
}

ColumnPath test_cyl_maes(const Bitmap<Cost>& cost)
{
  SCP_Maes_NE_E_SE_Cyclic algo_maes;

  if (args_info.visualize_given) algo_maes.SetVisualizationOutput(&visual);
  if (args_info.logging_given)   algo_maes.EnableLogging(true);

  algo_maes.SetCostMatrix(cost);
  ColumnPath path = Path2ColumnPath(algo_maes.ComputeMinCostCircularPath());

  return path;
}


ColumnPath show_all_paths(const Bitmap<Cost>& cost, bool segmentation_graph)
{
  int w=cost.AskWidth(), h=cost.AskHeight();

  SCP_DynProg_E_SE_S_Cyclic dynprog;
  SCP_DynProg_NE_E_SE_Cyclic dynprog_seg;

  dynprog.SetCostMatrix(cost);
  dynprog_seg.SetCostMatrix(cost);

  if (args_info.visualize_given) dynprog.SetVisualizationOutput(&visual);
  if (args_info.logging_given)   dynprog.EnableLogging(true);
  if (args_info.visualize_given) dynprog_seg.SetVisualizationOutput(&visual);
  if (args_info.logging_given)   dynprog_seg.EnableLogging(true);

  Visualizer vis;
  vis.Initialize(cost, segmentation_graph, args_info.fourquadrants_given);
  vis.PrepareVisualization();

  ColumnPath path;

  for (int r=0;r<h;r++)
    {
      if (segmentation_graph)
	path = Path2ColumnPath(dynprog_seg.ComputeMinCostCircularPath(r));
      else
	path = dynprog.ComputeMinCostCircularPath(r);

      DrawThickPath(vis.d_disp, path, Color<Pixel>(255,200,0),1);
    }

  vis.ShowVisualization();

  UnifiedImageWriter writer;
  writer.SetOutput("allpaths.png");
  writer.WriteImage(vis.d_disp);

  return path;
}




ColumnPath show_all_setpaths(const Bitmap<Cost>& cost, bool segmentation_graph)
{
  int w=cost.AskWidth(), h=cost.AskHeight();

  SCP_DynProg_E_SE_S_Cyclic dynprog;
  SCP_DynProg_NE_E_SE_Cyclic dynprog_seg;

  dynprog.SetCostMatrix(cost);
  dynprog_seg.SetCostMatrix(cost);

  if (args_info.visualize_given) dynprog.SetVisualizationOutput(&visual);
  if (args_info.logging_given)   dynprog.EnableLogging(true);
  if (args_info.visualize_given) dynprog_seg.SetVisualizationOutput(&visual);
  if (args_info.logging_given)   dynprog_seg.EnableLogging(true);

  Visualizer vis;
  vis.Initialize(cost, segmentation_graph, args_info.fourquadrants_given);
  vis.PrepareVisualization();

  ColumnPath path;

  if (segmentation_graph)
    dynprog_seg.Flood(0, true);
  else
    assert(0); // todo: not implemented yet

  for (int r=0;r<h;r++)
    {
      if (segmentation_graph)
	path = Path2ColumnPath(dynprog_seg.ExtractPath(r));
      else
	{
	  assert(0); // todo: not implemented yet
	  path = dynprog.ComputeMinCostCircularPath(r);
	}

      DrawThickPath(vis.d_disp, path, Color<Pixel>(255,200,0),1);
    }

  for (int r=0;r<h;r++)
    {
      if (segmentation_graph)
	path = Path2ColumnPath(dynprog_seg.ExtractPath(r));
      else
	{
	  assert(0); // todo: not implemented yet
	  path = dynprog.ComputeMinCostCircularPath(r);
	}

      if (path.StartRow() == path.EndRow())
	DrawThickPath(vis.d_disp, path, Color<Pixel>(255,0,0),1);
    }

  vis.ShowVisualization();

  UnifiedImageWriter writer;
  writer.SetOutput("allsetpaths.png");
  writer.WriteImage(vis.d_disp);

  return path;
}
#endif


template <class EdgeCostsT>
Path run_algorithm(const char* algo, const EdgeCostsT& cost, Topology topology)
{
  if (strcmp(algo, "show"    )==0) { test_show_input(); return Path(); }

#if 0
  if (strcmp(algo, "show-all-paths")==0)
    {
      return show_all_paths(cost[1], segmentation_graph);
    }
  else if (strcmp(algo, "show-all-setpaths")==0)
    {
      return show_all_setpaths(cost[1], segmentation_graph);
    }
  else {}
#endif

  if (topology==Torus)
    {
      if (strcmp(algo, "dynprog" )==0) return test_algorithm<EdgeCostsT,SCP_DynProg <Torus,EdgeCostsT> >(cost);
      if (strcmp(algo, "appleton")==0) return test_algorithm<EdgeCostsT,SCP_Appleton<Torus,EdgeCostsT> >(cost);
      if (strcmp(algo, "maes"    )==0) return test_algorithm<EdgeCostsT,SCP_Maes    <Torus,EdgeCostsT> >(cost);
      if (strcmp(algo, "farin"   )==0) return test_algorithm<EdgeCostsT,SCP_Farin   <Torus,EdgeCostsT> >(cost);
    }
  else if (topology==Cylinder)
    {
      if (strcmp(algo, "dynprog" )==0) return test_algorithm<EdgeCostsT,SCP_DynProg <Cylinder,EdgeCostsT> >(cost);
      if (strcmp(algo, "appleton")==0) return test_algorithm<EdgeCostsT,SCP_Appleton<Cylinder,EdgeCostsT> >(cost);
      if (strcmp(algo, "maes"    )==0) return test_algorithm<EdgeCostsT,SCP_Maes    <Cylinder,EdgeCostsT> >(cost);
      if (strcmp(algo, "farin"   )==0) return test_algorithm<EdgeCostsT,SCP_Farin   <Cylinder,EdgeCostsT> >(cost);
    }

}

int TimeDifference(const timeval& start, const timeval& end)
{
  int diff_sec  = end.tv_sec  - start.tv_sec;
  int diff_usec = end.tv_usec - start.tv_usec;
  return diff_sec*1000000 + diff_usec;
}

void PrintPath(const Path& solution,int w,int h)
{
  bool first=true;

  for (int x=0;x<solution.getWidth();x++)
    for (int y=solution.topRow(x); y<=solution.bottomRow(x); y++)
      {
	if (first) { first=false; } else cout << " ";
	cout << x << "," << y%h;
      }
}

/*
template <class SrcCost, class DstCost>
void ConvertCost(const Bitmap<SrcCost>& src, Bitmap<DstCost>& dst )
{
  dst.Create( src.AskHeight(), src.AskWidth() );
  ConvertBitmap( dst, src );
}
*/

template <class Cost>
boost::shared_ptr<EdgeCosts> ReadCosts_intern(InputSource* src, string& name)
{
  WeightFactory_Torus<Cost>*    srcTor   = dynamic_cast<WeightFactory_Torus<Cost>*>(src);
  WeightFactory_Cylinder<Cost>* srcCyl   = dynamic_cast<WeightFactory_Cylinder<Cost>*>(src);
  WeightFactory_Nodes<Cost>*    srcNodes = dynamic_cast<WeightFactory_Nodes<Cost>*>(src);

  //Bitmap<SrcCost> inputCost[3];

  if (srcTor)
    {
      EdgeCosts_Torus<Cost>* cost = new EdgeCosts_Torus<Cost>;

      name=srcTor->GenerateEdges_Tor(*cost);
      //for (int i=0;i<3;i++) ConvertCost(inputCost[i], cost[i]);
      return boost::shared_ptr<EdgeCosts>(cost);
    }
  else if (srcCyl)
    {
      EdgeCosts_Cylinder<Cost>* cost = new EdgeCosts_Cylinder<Cost>;

      name=srcCyl->GenerateEdges_Cyl(*cost);
      //for (int i=0;i<3;i++) ConvertCost(inputCost[i], cost[i]);
      return boost::shared_ptr<EdgeCosts>(cost);
    }
  else if (srcNodes)
    {
      // TODO: depending on program argument, generate either torus or cylinder

      Bitmap<Cost> bm;
      name = srcNodes->GenerateNodes(bm);

      int h=bm.AskHeight();
      int w=bm.AskWidth();
      const Cost*const* p = bm.AskFrame();

      NodeCosts_Torus<Cost>* cost = new NodeCosts_Torus<Cost>;
      cost->create(w,h);

      for (int y=0;y<h;y++)
	for (int x=0;x<w;x++)
	  cost->setCost(x,y, p[y][x]);

      //ConvertCost(inputCost[0], cost[0]);
      //cost[1]=cost[2]=cost[0];
      return boost::shared_ptr<EdgeCosts>(cost);
    }

  return boost::shared_ptr<EdgeCosts>();
}

boost::shared_ptr<EdgeCosts> ReadCosts(InputSource* src, string& name)
{
  boost::shared_ptr<EdgeCosts> costs;

  if (costs=ReadCosts_intern<float>        (src, name)) return costs;
  if (costs=ReadCosts_intern<unsigned char>(src, name)) return costs;
  if (costs=ReadCosts_intern<int>          (src, name)) return costs;

  assert(0);
  return costs;
}

template <class Cost>
void main_program()
{
  InputSource* input = NULL;

  if (args_info.ascii_given)           input = new InputSource_ASCII_Matrix<float>;
  else if (args_info.shapes_given)
    {
      InputSource_Shapes* shapeinput = new InputSource_Shapes(args_info.shape_skip_arg);
      input = shapeinput;
      shapeinput->EnableAllPairs(args_info.shape_allpairs_given);
      shapeinput->EnableRotation(args_info.shape_rotation_given);
    }
  else if (args_info.noise_given)      input = new InputSource_UniformNoise(args_info.noise_arg);
  else if (args_info.artificial_given) input = new InputSource_Synthetic;
  else if (args_info.gaussian_given)   input = new InputSource_GaussianShape(args_info.gaussian_arg,1);
  else if (args_info.dna_given)        input = new InputSource_DNA;
  else
    input = new InputSource_Image_Matrix;

  Topology topology = Torus;
  if (strcmp("cylinder", args_info.graph_arg)==0) topology=Cylinder;

  int start_w=0,start_h=0;
  int end_w  =0,end_h  =0;
  int incr_w =0,incr_h =0;
  if (args_info.size_given)       ExtractSize    (args_info.size_arg,       start_w,start_h);
  if (args_info.sizechange_given) ExtractSizeIncr(args_info.sizechange_arg, incr_w, incr_h, end_w,end_h);

  assert(input);
  input->Init(args_info.inputs, args_info.inputs_num);

  // --- initialize outputs ---

  uint64 totaltime_usec=0;

  int size_w = start_w;
  int size_h = start_h;

  //Bitmap<Cost> cost[3];

  boost::shared_ptr<EdgeCosts> cost;

  for (;;) // iterate through all sizes
    {
      uint64 sizetime_usec=0;
      int    sizetime_n=0;
      input->Reset();

      for (;;) // iterate through all data-sets
	{
	  /*
	    cout << start_w << " " << start_h << " "
	    << end_w   << " " << end_h   << " "
	    << incr_w  << " " << incr_h  << " -> " << size_w << " " << size_h << endl;
	  */

	  if (size_w) input->SetSize(size_w, size_h);

	  string name;
	  cost = ReadCosts(input, name);

	  if (args_info.visualize_given)
	    {
	      NodeCosts_Torus<Cost>* cost1 = dynamic_cast<NodeCosts_Torus<Cost>*>(cost.get());
	      assert(cost1); // TODO: implement other types

	      Bitmap<Cost> bm;
	      bm.Create(cost1->getWidth(), cost1->getHeight());
	      for (int y=0;y<cost1->getHeight();y++)
		for (int x=0;x<cost1->getWidth();x++)
		  bm.AskFrame()[y][x] = cost1->costE(x,y);  // TODO: which edge should we choose?

	      Visualizer::GraphType type;
	      switch (cost->getGraphTopology())
		{
		case Torus:    type=Visualizer::Torus;    break;
		case Cylinder: type=Visualizer::Cylinder; break;
		}

	      visual.initialize(bm, type);
	    }

	  timeval starttime,endtime;
	  gettimeofday(&starttime,NULL);

	  Path solution;

	  for (int i=0;i<args_info.repetitions_arg;i++)
	    {
	      {
		EdgeCosts_General<Cost>* c = dynamic_cast<EdgeCosts_General<Cost>*>(cost.get());
		if (c) { solution = run_algorithm(args_info.algo_arg, *c, cost->getGraphTopology()); }
	      }

	      {
		EdgeCosts_Torus<Cost>* c = dynamic_cast<EdgeCosts_Torus<Cost>*>(cost.get());
		if (c) { solution = run_algorithm(args_info.algo_arg, *c, cost->getGraphTopology()); }
	      }

	      {
		NodeCosts_Torus<Cost>* c = dynamic_cast<NodeCosts_Torus<Cost>*>(cost.get());
		if (c) { solution = run_algorithm(args_info.algo_arg, *c, cost->getGraphTopology()); }
	      }

	      {
		EdgeCosts_Cylinder<Cost>* c = dynamic_cast<EdgeCosts_Cylinder<Cost>*>(cost.get());
		if (c) { solution = run_algorithm(args_info.algo_arg, *c, cost->getGraphTopology()); }
	      }

	      {
		NodeCosts_Cylinder<Cost>* c = dynamic_cast<NodeCosts_Cylinder<Cost>*>(cost.get());
		if (c) { solution = run_algorithm(args_info.algo_arg, *c, cost->getGraphTopology()); }
	      }
	    }

	  gettimeofday(&endtime,NULL);

	  int usec = TimeDifference(starttime, endtime) / args_info.repetitions_arg;
	  totaltime_usec += usec;

	  sizetime_usec+=usec;
	  sizetime_n++;

	  cout << name << " @" << cost->getWidth() << "x" << cost->getHeight() << " " << usec/1000000.0
	       << " cost= " << solution.cost << " (" << solution.startRow() << ")" << endl;

	  if (args_info.print_path_given)
	    {
	      cout << "path="; PrintPath(solution,cost->getWidth(), cost->getHeight()); cout << endl;

	      if (args_info.dna_given)
		dynamic_cast<InputSource_DNA*>(input)->ShowMatching(solution);
	    }

	  // visualization

	  if (solution.getWidth())
	    {
	      if (args_info.visualize_given || args_info.save_solution_given)
		{
		  visual.prepareVisualization();
		  visual.drawPath(solution, 0xffc000);
		}

	      if (args_info.visualize_given    ) visual.showVisualization();
	      if (args_info.save_solution_given) WriteImage_PNG(visual.getImage(), "solution.png");
	    }

	  if ( !input->Next() ) break;
	}

      cout << "total time for size " << size_w << " x " << size_h << " : " << sizetime_usec/sizetime_n/1000000.0 << endl;

      if (!incr_w) break; // we just have one size
      size_w += incr_w; size_h += incr_h;
      if (size_w > end_w || size_h > end_h) break;
    }

  cout << "total time: " << totaltime_usec/1000000.0 << endl;
  //cout << "bound time: " << boundtime/1000000.0 << endl;
}


void control(int argc,char** argv)
{
  //srand(time(NULL));

  try
    {
      cmdline_parser(argc,argv,&args_info);

      if (strcmp(args_info.algo_arg, "list")==0)
	{
	  cerr << "farin    - improved Maes algorithm\n"
	    //<< "farin-b  - The new algorithm.\n"
	    //	       << "farin2   - Combination of the old farin-Algorithm with branch & bound ideas.\n"
	    //<< "dijkstra - All-pair dijkstra algorithm.\n"
	       << "dynprog  - All-pair dynamic-programming.\n"
	       << "appleton - Basic branch&bound algorithm.\n"
	    //	       << "farinbb  - Farin's B&B algorithm with guaranteed (n^2 log n) runtime\n"
	       << "maes     - divide and conquer.\n"
	    //<< "dynprog-single - Perform exactly one dynamic-programming flooding (just for speed comparisons).\n"
	       << "show           - do now carry out scp-search. Only show the input data.\n";
	  exit(0);
	}

      main_program<float>();
    }
  catch(const Excpt_Base& e)
    {
      MessageDisplay::Show(e);
    }
}
