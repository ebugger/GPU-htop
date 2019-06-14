objects =  gpu gpu.o
CFLAGS = -I/usr/include/nvidia/gdk
LDFLAGS = -L/usr/src/gdk/nvml/lib
LIBS = -lnvidia-ml -lncurses

gpu:gpu.o
	g++ $(LDFLAGS) $(LIBS) -o gpu gpu.o

gpu.o:gpu.cpp
	g++ $(CFLAGS) -c gpu.cpp 

clean:
	rm $(objects)
