
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
<<<<<<< Updated upstream
	build/filter.o \
	build/scope.o
=======
<<<<<<< HEAD
	build/test.o \
	build/filter.o
=======
	build/filter.o \
	build/scope.o
>>>>>>> 394b9c2d0487aee0348d47187540131f954d1e78
>>>>>>> Stashed changes
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
	src/record.cpp \
	src/careme.cpp \
<<<<<<< Updated upstream
	src/filter.cpp \
	src/scope.cpp
=======
<<<<<<< HEAD
	src/test.cpp \
	src/filter.cpp
=======
	src/filter.cpp \
	src/scope.cpp
>>>>>>> 394b9c2d0487aee0348d47187540131f954d1e78
>>>>>>> Stashed changes
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
<<<<<<< Updated upstream
	include/filter.hpp \
	include/scope.hpp
=======
<<<<<<< HEAD
	include/test.hpp \
	include/filter.hpp

=======
	include/filter.hpp \
	include/scope.hpp
>>>>>>> 394b9c2d0487aee0348d47187540131f954d1e78
>>>>>>> Stashed changes
defaut:
	@echo you must choose :
	@echo   - make win32
	@echo   - make linux64 (ENIB)
	

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

linux64: $(HEADER) $(OBJ) $(FILECPP)
	g++ -o $(TARGET_NAME).x64 $(OBJ) $(CXXFLAGS) "./liblinux64/libbass.so" "./liblinux64/libsfml-graphics.a" "./liblinux64/libsfml-window.a" "./liblinux64/libsfml-system.a" -lX11 -lGL -lXrandr -ljpeg -lfreetype "./liblinux64/libGLEW.a"
	
