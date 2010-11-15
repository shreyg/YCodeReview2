CPPFLAGS = -I.
CXXFLAGS = -Wall -g

OBJECTS = Tissue.o main.o
LIBS = fwk/BaseCollection.o fwk/BaseNotifiee.o fwk/Exception.o

asgn1:	$(OBJECTS) $(LIBS)
	$(CXX) $(CXXFLAGS) -o asgn1 $(OBJECTS) $(LIBS)

clean:
	rm -f asgn1 $(OBJECTS) $(LIBS) *~

Tissue.o: Tissue.cpp Tissue.h
main.o: main.cpp
