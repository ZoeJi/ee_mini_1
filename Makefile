CXX = g++
CXXFLAGS = -Wall -std=c++0x
TARGET = parser
OBJS = main.o gate.o cell.o gateList.o gatetype.o cellList.o
$(TARGET) : $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.O : %.CPP
	$(CXX) $(CXXFLAGS) -c -o $@ $<
.PHONY :clean
clean:
	rm $(OBJS)

main.o: main.cpp gate.h gate.cpp gatetype.h gatetype.cpp gateList.h \
gateList.cpp cell.h cell.cpp cellList.h cellList.cpp
gateList.o: gateList.h gate.h gate.cpp gateList.cpp
gatetype.o: gatetype.h gatetype.cpp
gate.o: gate.h gate.cpp
cellList.o: cellList.h cell.h cellList.cpp cell.cpp