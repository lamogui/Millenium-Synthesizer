
TARGET_NAME=test
OBJ=main.o terrain.o player.o soccerball.o
CFLAGS= --std=c99 -W -Wall -I"include"
CXXFLAGS= -I"include" -W -Wall
FILECPP=main.cpp terrain.cpp player.cpp soccerball.cpp
HEADER=terrain.hpp player.hpp soccerball.hpp

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
	g++ -o $(TARGET_NAME).exe $(OBJ) $(CXXFLAGS) -static-libgcc -static -lstdc++ "./libwin32/bass.lib" "./libwin32/libsfml-graphics.a" "./libwin32/libsfml-window.a" "./libwin32/libsfml-system.a" 

linux32: $(HEADER) $(OBJ) $(FILECPP)
	g++ -o $(TARGET_NAME).x32 $(OBJ) $(CXXFLAGS) "./liblinux32/libbass.so" "./liblinux32/libsfml-graphics.so.2.1" "./liblinux32/libsfml-window.so.2.1" "./liblinux32/libsfml-system.so.2.1" 

linux64: $(HEADER) $(OBJ) $(FILECPP)
	g++ -o $(TARGET_NAME).x64 $(OBJ) $(CXXFLAGS) "./liblinux64/libsfml-graphics.a" "./liblinux64/libsfml-window.a" "./liblinux64/libsfml-system.a" -lX11 -lGL -lXrandr -ljpeg -lfreetype "./liblinux64/libGLEW.a"