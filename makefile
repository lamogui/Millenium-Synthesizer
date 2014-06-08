
TARGET_NAME=test
CFLAGS= --std=c99 -W -Wall -I"include" -g
CXXFLAGS= -I"include" -W -Wall -g

OBJ= build/signal.o \
	build/audiostream.o \
	build/oscillator.o \
	build/note.o \
	build/nelead6.o \
	build/variant.o \
	build/settings.o \
	build/basserrorhandler.o \
	build/bassdriver.o \
	build/interface.o \
	build/puresquare.o \
	build/enveloppe.o \
	build/careme.o \
	build/filter.o \
	build/scope.o \
	build/track.o \
	build/midi.o \
	build/fft.o \
	build/file.o \
	build/preset.o \
	build/window.o \
	build/sfmlaudiodriver.o

FILECPP=src/signal.cpp \
	src/oscillator.cpp \
	src/audiostream.cpp \
	src/main.cpp \
	src/note.cpp \
	src/nelead6.cpp \
	src/variant.cpp \
	src/settings.cpp \
	src/basserrorhandler.cpp \
	src/bassdriver.cpp \
	src/interface.cpp \
	src/puresquare.cpp \
	src/enveloppe.cpp \
	src/careme.cpp \
	src/filter.cpp \
	src/scope.cpp \
	src/track.cpp \
	src/midi.cpp \
	src/fft.cpp \
	src/file.cpp \
	src/preset.cpp \
	src/window.cpp \
	src/sfmlaudiodriver.cpp

HEADER=	include/signal.hpp \
	include/oscillator.hpp \
	include/audiostream.hpp \
	include/note.hpp \
	include/instrument.hpp \
	include/nelead6.hpp \
	include/variant.hpp \
	include/settings.hpp \
	include/basserrorhandler.hpp \
	include/audiodriver.hpp \
	include/bassdriver.hpp \
	include/interface.hpp \
	include/puresquare.hpp \
	include/enveloppe.hpp \
	include/config.hpp \
	include/careme.hpp \
	include/filter.hpp \
	include/scope.hpp \
	include/track.hpp \
	include/midi.hpp \
	include/fft.hpp \
	include/file.hpp \
	include/preset.hpp \
	include/window.hpp \
	include/sfmlaudiodriver.hpp

defaut:
	@echo you must choose :
	@echo   - make win32
	@echo   - make linux32
	@echo   - make linux64
	@echo   - make linux64enib ENIB only
	

build/%.o: src/%.c $(HEADER)
	@echo Compilation C $< to $@
	gcc -c $< -o $@ -std=c89 $(CFLAGS)

build/%.o: src/%.cpp $(HEADER)
	@echo Compilation C++ $< to $@
	g++  -c $< -o $@ $(CXXFLAGS)

#windows only
build/%.o: src/%.rc
	@echo RC compiling $< to $@
	windres $< $@ -v
	
win32: $(HEADER) $(OBJ) $(FILECPP) build/main.o
	g++ -o $(TARGET_NAME).exe build/main.o $(OBJ) $(CXXFLAGS) -static-libgcc -static -lstdc++ -lComdlg32 "./libwin32/bass.lib" "./libwin32/bassasio.lib" "./libwin32/libsfml-graphics.a" "./libwin32/libsfml-window.a" "./libwin32/libsfml-system.a" -lkernel32
	
linux32: $(HEADER) $(OBJ) $(FILECPP) build/main.o
	g++ -o $(TARGET_NAME).x32 $(OBJ) build/main.o $(CXXFLAGS) "./liblinux32/libbass.so" -lsfml-graphics -lsfml-window -lsfml-system -lX11 -lGL -lXrandr -ljpeg -lfreetype -lGLEW
	
linux64: $(HEADER) $(OBJ) $(FILECPP) build/main.o
	g++ -o $(TARGET_NAME).x64 $(OBJ) build/main.o $(CXXFLAGS) "./liblinux64/libbass.so" -ljpeg -lsfml-audio -lsfml-graphics -lsfml-window -lsfml-system -lX11 -lGL -lXrandr -lfreetype -lGLEW

linux64enib: $(HEADER) $(OBJ) $(FILECPP) build/main.o
	g++ -o $(TARGET_NAME).enib $(OBJ) build/main.o $(CXXFLAGS) -DNO_SFML_AUDIO "./liblinux64/libbass.so" "./liblinux64/ENIB/libsfml-graphics.a" "./liblinux64/ENIB/libsfml-window.a" "./liblinux64/ENIB/libsfml-system.a" -lX11 -lGL -lXrandr -ljpeg -lfreetype "./liblinux64/ENIB/libGLEW.a"
	
