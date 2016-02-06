# TITAN MACHINE
export PNGWRITER_LIBDIR=/home/taritree/software/pngwriter/lib
export PNGWRITER_INCDIR=/home/taritree/software/pngwriter/include

export FREETYPE_INCDIR=/usr/include/freetype2

export OPENCV_INCDIR=/usr/local/include
export OPENCV_LIBDIR=/usr/local/lib

# TARITIREE LAPTOP

export DYLD_LIBRARY_PATH=${PNGWRITER_LIBDIR}:${OPENCV_LIBDIR}:${DYLD_LIBRARY_PATH}
export LD_LIBRARY_PATH=${PNGWRITER_LIBDIR}:${OPENCV_LIBDIR}:${LD_LIBRARY_PATH}
