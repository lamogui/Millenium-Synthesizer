
TARGET_NAME=test
OBJ=main.o signal.o audiostream.o oscillator.o note.o nelead6.o variant.o settings.o basserrorhandler.o bassdriver.o interface.o puresquare.o enveloppe.o
CFLAGS= --std=c99 -W -Wall -I"include"
CXXFLAGS= -I"include" -Os -s
FILECPP=signal.cpp oscillator.cpp audiostream.cpp main.cpp note.cpp nelead6.cpp variant.cpp settings.cpp config.hpp basserrorhandler.cpp bassdriver.cpp interface.cpp puresquare.cpp enveloppe.cpp
HEADER=signal.hpp oscillator.hpp audiostream.hpp note.hpp instrument.hpp nelead6.hpp variant.hpp settings.hpp basserrorhandler.hpp audiodriver.hpp bassdriver.hpp interface.hpp puresquare.hpp enveloppe.hpp

defaut:
	@echo you must choose :
	@echo   - make win32
	@echo   - make linux32
	@echo   - make linux64
	

%.o: %.c $(HEADER)
	@echo Compilation C $< to $@
	gcc -c $< -o $@ $(CFLAGS)

%.o: %.cpp $(HEADER)
	@echo Compilation C++ $< to $@
	g++ -c $< -o $@ $(CXXFLAGS)

#windows only
%.o: %.rc
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
