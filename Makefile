# Flags and Compiler

CXX ?= clang++ 
CXXFLAGS ?= --std=c++17 -g -w 

executeBase: base.exe
	./base.exe
executeDynamic: dynamic.exe
	./dynamic.exe

dynamic.exe: DynamicMain.cpp AGCell.cpp AGTissue.cpp CellClass.cpp DynamicEdge.cpp DynamicTissue.cpp EdgeClass.cpp SharedConstants.cpp TissueClass.cpp VectClass.cpp VertexClass.cpp
	$(CXX) $(CXXFLAGS) AGCell.cpp AGTissue.cpp CellClass.cpp DynamicEdge.cpp DynamicMain.cpp DynamicTissue.cpp EdgeClass.cpp SharedConstants.cpp TissueClass.cpp VectClass.cpp VertexClass.cpp -o dynamic.exe
base.exe: MainTest.cpp AGCell.cpp AGTissue.cpp CellClass.cpp EdgeClass.cpp SharedConstants.cpp TissueClass.cpp VectClass.cpp VertexClass.cpp
	$(CXX) $(CXXFLAGS) AGCell.cpp AGTissue.cpp CellClass.cpp EdgeClass.cpp SharedConstants.cpp TissueClass.cpp VectClass.cpp VertexClass.cpp MainTest.cpp -o base.exe
	


