CXX = c++
CFLAGS = -g -fPIC `root-config --cflags`
LDFLAGS = 
LDLIBS = 

ROOTLIBS = `root-config --libs`

PNGWRITER_LIBS   = -lpngwriter -lpng -lz -lm -lc -lfreetype
OPENCV_LIBS = -lopencv_core
OPENCV_LIBS = $(wildcard ${OPENCV_LIBDIR}/libopencv*.so)

CFLAGS += -I$(PNGWRITER_INCDIR) -I$(OPENCV_INCDIR)
LDLIBS += $(ROOTLIBS)
LDLIBS += -L$(PNGWRITER_LIBDIR) $(PNGWRITER_LIBS)
LDLIBS += -L$(OPENCV_LIBDIR) $(OPENCV_LIBS)

all: root2yolopng

root2yolopng.o: root2yolopng.cc
	$(CXX) $(CFLAGS) -c root2yolopng.cc -o root2yolopng.o

root2yolopng: root2yolopng.o
	$(CXX) $(LDLIBS) -o root2yolopng root2yolopng.o $(LDLIBS)
	@rm root2yolopng.o

clean:
	@rm root2yolopng
