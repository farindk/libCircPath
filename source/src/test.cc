
#include <iostream>
#include <libvideogfx.hh>

using namespace std;
using namespace videogfx;


//#include "scp_dynprog.hh"

#include "type_matrix.hh"
#include "type_cell.hh"
#include "type_edges.hh"
#include "path.hh"
#include "flooding.hh"
#include "scp_dynprog.hh"
#include "scp_maes.hh"
#include "scp_appleton.hh"
#include "scp_farin.hh"
#include "visualizer_videogfx.hh"


void test_tinyTorus()
{
  static const float edgeTab[2][2][3] = {
    { { 8,12,3 }, { 7,4,5  } },
    { { 2,9,10 }, { 11,1,6 } }
  };

  Cell_Std<float> cell;

  int w = 2;
  int h = 2;

  CellMatrix< Cell_Std<float> > matrix;
  matrix.create(w+1,h*2,1);

  EdgeCosts_Torus<float> edges;
  edges.create(w,h);
  for (int x=0;x<edges.getWidth();x++)
    for (int y=0;y<edges.getHeight();y++)
      {
	std::cout << "x=" << x << " y=" << y << " -> "
		  << edgeTab[y][x][0] << ";"
		  << edgeTab[y][x][1] << ";"
		  << edgeTab[y][x][2] << "\n";

	edges.setCostE (x,y, edgeTab[y][x][0]);
	edges.setCostSE(x,y, edgeTab[y][x][1]);
	edges.setCostS (x,y, edgeTab[y][x][2]);
      }

  typedef Flooding< EdgeCosts_Torus<float>, CellMatrix< Cell_Std<float> > > MyFlooding;

  MyFlooding::Flood_Unrestricted(edges,matrix, 0,h/2, 0,2*h-1);

  std::cout << "B.R.corner cost= " << matrix(w,h).costSum << "\n";
}

#include <stdlib.h>
void test_largeTorus()
{
  typedef unsigned char Cost;
  typedef CostTraits<Cost>::SumType CostSum;

  int w = 20000;
  int h = 20000;

  CellMatrix< Cell_Std<CostSum> > matrix;
  matrix.create(w+1,h*2,1);

  NodeCosts_Torus<Cost> edges;
  edges.create(w,h);
  for (int x=0;x<edges.getWidth();x++)
    for (int y=0;y<edges.getHeight();y++)
      {
	int r = rand()%256;
	edges.setCost (x,y, r);
	//edges.setCostSE(x,y, r);
	//edges.setCostS (x,y, r);
      }

  std::cout << "flood...\n";

  typedef Flooding< NodeCosts_Torus<Cost>, CellMatrix< Cell_Std<CostSum> > > MyFlooding;

  MyFlooding::Flood_Unrestricted(edges,matrix, 0,0, 0,2*h-1);

  std::cout << "B.R.corner cost= " << matrix(w,h).costSum << "\n";
}



void test_dynProg_largeTorus()
{
  typedef unsigned char Cost;
  typedef CostTraits<Cost>::SumType CostSum;
  typedef NodeCosts_Torus<Cost> Edges;

  int w = 1000;
  int h = 1000;
  int s=0;

  for (;s<100;s++)
    {
      //s=352;
      srand(s);

      CellMatrix< Cell_Std<CostSum> > matrix;
      matrix.create(w+1,h*2,1);

      Edges edges;
      edges.create(w,h);
      for (int x=0;x<edges.getWidth();x++)
	for (int y=0;y<edges.getHeight();y++)
	  {
	    int r = rand()%256;
	    edges.setCost (x,y, r);
	    //std::cout << x << ";" << y << " NODEc = " << r << "\n";
	  }

      std::cout << "search...\n";

      //typedef Flooding< NodeCosts_Torus<unsigned char>, CellMatrix< Cell_Std<float> > > MyFlooding;

      Bitmap<Pixel> costbm(w,h);
      for (int y=0;y<h;y++)
	for (int x=0;x<w;x++)
	  costbm.AskFrame()[y][x]=edges.costE(x,y);


      Visualizer_VideoGfx visualizer;
      //visualizer.initialize(costbm, Visualizer::Torus);


      //SCP_DynProg<Torus, Edges> scp2;
      SCP_Maes<Torus, Edges> scp2;
      //SCP_Appleton<Torus, Edges> scp;
      SCP_Farin<Torus, Edges> scp;
      scp.setEdgeCosts(edges);
      //scp.setVisualizer(&visualizer);
      //scp.enableLogging();
      Path p = scp.computeMinCostCircularPath();
      std::cout << "total cost = " << p.cost << "\n";

      scp2.setEdgeCosts(edges);
      //scp2.enableLogging();
      Path p2; // = scp2.computeMinCostCircularPath();
      std::cout << "ref cost = " << p2.cost << "\n";

      //if (p.cost != p2.cost) { std::cout << "SEED=" << s << "\n"; exit(0); }
    }
}
