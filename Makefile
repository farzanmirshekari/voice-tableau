INCLUDE_DIRECTORIES = -I/opt/homebrew/include/opencv4/ \
                      -I/opt/homebrew/include/portaudio/ \
                      -I/opt/homebrew/include/

CXXFLAGS = -O3 -std=c++11

LIBS = -L/opt/homebrew/lib/ \
       -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_imgcodecs \
       -lportaudio -lfftw3

OBJS = main.o utilities.o Spectrogram.o

all: voice_tableau

voice_tableau: $(OBJS)
	g++ -o $@ $(OBJS) $(LIBS)
	rm -rf *.o

%.o: %.cpp
	g++ -c -pipe $(CXXFLAGS) $(INCLUDE_DIRECTORIES) $< -o $@

clean:
	rm -rf *.o voice_tableau