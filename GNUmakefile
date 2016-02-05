CXX = c++
CFLAGS = -g -fPIC `root-config --cflags`
LDFLAGS = 
LDLIBS = 

ROOTLIBS = `root-config --libs`

PNGWRITER_LIBDIR = /Users/twongjirad/software/pngwriter/lib
PNGWRITER_INCDIR = /Users/twongjirad/software/pngwriter/include
PNGWRITER_LIBS   = -lpngwriter -lpng -lz -lm -lc -lfreetype

FREETYPE_INCDIR = /opt/X11/include/freetype2

CFLAGS += -I$(PNGWRITER_INCDIR) -I$(FREETYPE_INCDIR)
LDLIBS += $(ROOTLIBS)
LDLIBS += -L$(PNGWRITER_LIBDIR) $(PNGWRITER_LIBS)

all: root2yolopng

root2yolopng.o: root2yolopng.cc
	$(CXX) $(CFLAGS) -c root2yolopng.cc -o root2yolopng.o

root2yolopng: root2yolopng.o
	$(CXX) $(LDLIBS) -o root2yolopng root2yolopng.o $(LDLIBS)
	@rm root2yolopng.o

clean:
	@rm root2yolopng
