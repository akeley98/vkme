default: all

-include cckiss/Makefile

CXXFLAGS=-O2 -Wall -std=c++17
LIBS=-lvulkan -lglfw

all: window validation image pipeline present recreate index bezier

window: cckiss/00_base_code.cpp.o
	$(CXX) cckiss/00_base_code.cpp.o -o window $(LIBS)

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
