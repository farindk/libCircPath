
OBJS = test.o main.o path.o visualizer_videogfx.o

#util_path.o visualizer.o \
#  input.o integral_invariant.o \
#  cmdline.o shortest_circular_path.o
#  test.o

#  util_columnpath.o scp_farin2.o \
#  scp_dijkstra.o scp_farin.o scp_dynprog.o scp_appleton.o scp_maes.o \
#  cmdline.o integral_invariant.o input.o \
#  scp_farinbb.o scp_dynprog_cyl.o scp_dijkstra_cyl.o scp_maes_cyl.o scp_appleton_cyl.o scp_farin_cyl.o scp_farinbb_cyl.o \
#  scp_farin_cyl2.o scp_farin_heap.o scp_farinheap_cyl.o scp_skewtest.o \
#  scp_dynprog_edges_cyl.o scp_dynprog_edges_tor.o \
#  scp_maes_edges_tor.o scp_maesrot_edges_tor.o scp_farin3_cyl.o \
#  shortest_circular_path.o

# SEGMOBJS = util_path.o scp_farinbb_cyl.o scp_farin_cyl.o scp_dynprog_cyl.o segmenter.o visualizer.o

#RANDOMPATHOBJS =  input.o util_path.o integral_invariant.o util_columnpath.o scp_dynprog_cyl.o randompaths.o visualizer.o

#  scp_farinbb_no_cutaway.o

CXXFLAGS = -O0 -g `libvideogfx-config --cflags` # -mtune=athlon64 `gsl-config --cflags`
CXX=g++

shcp: $(OBJS)
	g++ -O0 -g -o $@ $(OBJS) `libvideogfx-config --libs` # `gsl-config --libs`

#segmenter: $(SEGMOBJS)
#	g++ -O99 -o $@ $(SEGMOBJS) `libvideogfx-config --libs`

#randompaths: $(RANDOMPATHOBJS)
#	g++ -O99 -o $@ $(RANDOMPATHOBJS) `libvideogfx-config --libs` `gsl-config --libs`

#shortest_circular_path.o: cmdline.c

#cmdline.c: cmdline.ggo
#	gengetopt -u -i cmdline.ggo

# pull in dependency info for *existing* .o files
-include $(OBJS:.o=.d)

# compile and generate dependency info
%.o: %.cc
	g++ -c $(CXXFLAGS) $*.cc -o $*.o
	@g++ -MM $(CXXFLAGS) $*.cc > $*.d

#cmdline.c: cmdline.ggo
#	gengetopt -u -i cmdline.ggo

clean:
	-rm $(OBJS) $(SEGMOBJS) *~
	-rm $(patsubst %.o,%.d,$(OBJS) $(SEGMOBJS))
	-rm shcp
