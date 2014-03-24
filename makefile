
TARGET_NAME=test
OBJ=	build/main.o \
	build/signal.o \
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
	build/record.o
CFLAGS= --std=c99 -W -Wall -I"include"
CXXFLAGS= -I"include" -Os -s
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
	src/record.cpp
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
	include/record.hpp \
	include/config.hpp 

defaut:
	@echo you must choose :
	@echo   - make win32
	@echo   - make linux32
	@echo   - make linux64
	

build/%.o: src/%.c $(HEADER)
	@echo Compilation C $< to $@
	gcc -c $< -o $@ $(CFLAGS)

build/%.o: src/%.cpp $(HEADER)
	@echo Compilation C++ $< to $@
	g++ -c $< -o $@ $(CXXFLAGS)

#windows only
build/%.o: src/%.rc
	@echo RC compiling $< to $@
	windres $< $@ -v

win32: $(HEADER) $(OBJ) $(FILECPP)
	g++ -o $(TARGET_NAME).exe $(OBJ) $(CXXFLAGS) -static-libgcc -static -lstdc++ "./libwin32/bass.lib" "./libwin32/bassasio.lib" "./libwin32/libsfml-graphics.a" "./libwin32/libsfml-window.a" "./libwin32/libsfml-system.a" 

linux32: $(HEADER) $(OBJ) $(FILECPP)
	g++ -o $(TARGET_NAME).x32 $(OBJ) $(CXXFLAGS) "./liblinux32/libbass.so" "./liblinux32/libsfml-graphics.so.2.1" "./liblinux32/libsfml-window.so.2.1" "./liblinux32/libsfml-system.so.2.1" 

linux64: $(HEADER) $(OBJ) $(FILECPP)
	g++ -o $(TARGET_NAME).x64 $(OBJ) $(CXXFLAGS) "./liblinux64/libbass.so" "./liblinux64/libsfml-graphics.a" "./liblinux64/libsfml-window.a" "./liblinux64/libsfml-system.a" -lX11 -lGL -lXrandr -ljpeg -lfreetype "./liblinux64/libGLEW.a"
	
synth_exemple: synth.c
	gcc -o synth.exe  synth.c  -I"include" -std=c99 "./libwin32/bass.lib"
