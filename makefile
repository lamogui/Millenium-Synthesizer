
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
	build/record.o \
	build/careme.o \
	build/filter.o \
	build/scope.o

CFLAGS= --std=c99 -W -Wall -I"include" -g
CXXFLAGS= -I"include" -g
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
	src/record.cpp \
	src/careme.cpp \
	src/filter.cpp \
	src/scope.cpp

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
	include/config.hpp \
	include/careme.hpp \
	include/filter.hpp \
	include/scope.hpp 

defaut:
	@echo you must choose :
	@echo   - make win32
	@echo   - make linux32
	@echo   - make linux64
	@echo   - make linux64enib (special ENIB)
	

build/%.o: src/%.c $(HEADER)
	@echo Compilation C $< to $@
	gcc -c $< -o $@ $(CFLAGS)

build/%.o: src/%.cpp $(HEADER)
	@echo Compilation C++ $< to $@
	g++ -std=c++11 -c $< -o $@ $(CXXFLAGS)

#windows only
build/%.o: src/%.rc
	@echo RC compiling $< to $@
	windres $< $@ -v

win32: $(HEADER) $(OBJ) $(FILECPP)
	g++ -o $(TARGET_NAME).exe $(OBJ) $(CXXFLAGS) -static-libgcc -static -lstdc++ "./libwin32/bass.lib" "./libwin32/bassasio.lib" "./libwin32/libsfml-graphics.a" "./libwin32/libsfml-window.a" "./libwin32/libsfml-system.a" 
	
linux32: $(HEADER) $(OBJ) $(FILECPP)
	g++ -o $(TARGET_NAME).x32 $(OBJ) $(CXXFLAGS) "./liblinux32/libbass.so" -lsfml-graphics -lsfml-window -lsfml-system -lX11 -lGL -lXrandr -ljpeg -lfreetype -lGLEW
	
linux64: $(HEADER) $(OBJ) $(FILECPP)
	g++ -o $(TARGET_NAME).x64 $(OBJ) $(CXXFLAGS) "./liblinux64/libbass.so" -lsfml-graphics -lsfml-window -lsfml-system -lX11 -lGL -lXrandr -ljpeg -lfreetype -lGLEW

linux64enib: $(HEADER) $(OBJ) $(FILECPP)
	g++ -o $(TARGET_NAME).enib $(OBJ) $(CXXFLAGS) "./liblinux64/libbass.so" "./liblinux64/libsfml-graphics.a" "./liblinux64/libsfml-window.a" "./liblinux64/libsfml-system.a" -lX11 -lGL -lXrandr -ljpeg -lfreetype "./liblinux64/libGLEW.a"
	
