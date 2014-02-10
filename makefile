
TARGET_NAME=test
OBJ=main.o terrain.o player.o
CFLAGS= --std=c99 -W -Wall -I"include"
CXXFLAGS= -W -Wall -I"include" -pedantic
FILECPP=main.cpp terrain.cpp player.cpp
HEADER=terrain.hpp player.hpp

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

linux 64:
	g++ -o $(TARGET_NAME).x64 $(OBJ) $(CXXFLAGS) "./liblinux64/libbass.so" "./liblinux64/libsfml-graphics.so.2.1" "./liblinux64/libsfml-window.so.2.1" "./liblinux64/libsfml-system.so.2.1"