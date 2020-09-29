default: all

-include cckiss/Makefile

CPPFLAGS=-DGLFW_INCLUDE_VULKAN=1 -DGLM_FORCE_RADIANS=1 -DGLM_FORCE_DEPTH_ZERO_TO_ONE=1
CXXFLAGS=-O2 -Wall -Wextra -std=c++17
LIBS=-lvulkan -lglfw

all: window validation image pipeline present recreate index bezier

testwindow: cckiss/window.cc.o cckiss/testwindow.cc.o
	$(CXX) cckiss/window.cc.o cckiss/testwindow.cc.o -o testwindow $(LIBS)

validation: cckiss/validation.cpp.o
	$(CXX) cckiss/validation.cpp.o -o validation $(LIBS)

image: cckiss/image.cpp.o
	$(CXX) cckiss/image.cpp.o -o image $(LIBS)

pipeline: cckiss/pipeline.cpp.o glsl-pipeline/vert.spv glsl-pipeline/frag.spv
	$(CXX) cckiss/pipeline.cpp.o -o pipeline $(LIBS)

present: cckiss/present.cpp.o glsl-pipeline/vert.spv glsl-pipeline/frag.spv
	$(CXX) cckiss/present.cpp.o -o present $(LIBS)

recreate: cckiss/recreate.cpp.o glsl-pipeline/vert.spv glsl-pipeline/frag.spv
	$(CXX) cckiss/recreate.cpp.o -o recreate $(LIBS)

index: cckiss/index.cpp.o glsl-index/vert.spv glsl-index/frag.spv
	$(CXX) cckiss/index.cpp.o -o index $(LIBS)

bezier: cckiss/bezier.cpp.o glsl-index/vert.spv glsl-index/frag.spv
	$(CXX) cckiss/bezier.cpp.o -o bezier $(LIBS)

depth: cckiss/depth.cpp.o glsl-depth/vert.spv glsl-depth/frag.spv
	$(CXX) cckiss/depth.cpp.o -o depth $(LIBS)

spinny/spinny-bin: cckiss/spinny/window.cc.o cckiss/spinny/render.cc.o cckiss/spinny/main.cc.o glsl-depth/vert.spv glsl-depth/frag.spv spinny/spinny-data/voxel.vert.spv spinny/spinny-data/voxel.frag.spv
	$(CXX) cckiss/spinny/window.cc.o cckiss/spinny/render.cc.o cckiss/spinny/main.cc.o -o spinny/spinny-bin $(LIBS)

glsl-pipeline/vert.spv: pipeline.vert
	glslangValidator pipeline.vert -V -o glsl-pipeline/vert.spv

glsl-pipeline/frag.spv: pipeline.frag
	glslangValidator pipeline.frag -V -o glsl-pipeline/frag.spv

glsl-index/vert.spv: index.vert
	glslangValidator index.vert -V -o glsl-index/vert.spv

glsl-index/frag.spv: index.frag
	glslangValidator index.frag -V -o glsl-index/frag.spv

glsl-depth/vert.spv: depth.vert
	glslangValidator depth.vert -V -o glsl-depth/vert.spv

glsl-depth/frag.spv: depth.frag
	glslangValidator depth.frag -V -o glsl-depth/frag.spv

spinny/spinny-data/voxel.vert.spv: spinny/voxel.vert
	glslangValidator spinny/voxel.vert -V -o spinny/spinny-data/voxel.vert.spv

spinny/spinny-data/voxel.frag.spv: spinny/voxel.frag
	glslangValidator spinny/voxel.frag -V -o spinny/spinny-data/voxel.frag.spv

