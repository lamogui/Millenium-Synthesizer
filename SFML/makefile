

SYSTEMOBJ= ./System/Clock.o  ./System/Time.o ./System/Err.o ./System/Lock.o ./System/Mutex.o ./System/Sleep.o ./System/String.o ./System/Thread.o ./System/ThreadLocal.o ./System/Unix/ClockImpl.o ./System/Unix/MutexImpl.o ./System/Unix/SleepImpl.o ./System/Unix/ThreadImpl.o ./System/Unix/ThreadLocalImpl.o


WINDOWOBJ= ./Window/Context.o ./Window/GlContext.o ./Window/GlResource.o ./Window/Joystick.o ./Window/JoystickManager.o ./Window/Keyboard.o ./Window/Mouse.o ./Window/VideoMode.o ./Window/Window.o ./Window/WindowImpl.o ./Window/Linux/Display.o ./Window/Linux/GlxContext.o ./Window/Linux/InputImpl.o ./Window/Linux/JoystickImpl.o ./Window/Linux/VideoModeImpl.o ./Window/Linux/WindowImplX11.o

GRAPHICSOBJ= ./Graphics/CircleShape.o ./Graphics/Color.o ./Graphics/ConvexShape.o ./Graphics/Font.o ./Graphics/GLCheck.o ./Graphics/Image.o ./Graphics/ImageLoader.o ./Graphics/RectangleShape.o ./Graphics/RenderStates.o ./Graphics/RenderTarget.o ./Graphics/RenderTexture.o ./Graphics/RenderTextureImpl.o ./Graphics/RenderTextureImplFBO.o ./Graphics/RenderWindow.o ./Graphics/Shader.o ./Graphics/Shape.o ./Graphics/Sprite.o ./Graphics/Text.o ./Graphics/Texture.o ./Graphics/TextureSaver.o ./Graphics/Transformable.o ./Graphics/Transform.o ./Graphics/VertexArray.o ./Graphics/Vertex.o ./Graphics/View.o ./Graphics/RenderTextureImplDefault.o


INCLUDEFLAGS=-I../include -I..

all: system window graphics clean

%.o: %.cpp $(HEADER)
	@echo Compilation C++ $< to $@
	g++ $(INCLUDEFLAGS) -c $< -o $@ $(CXXFLAGS)

system: $(SYSTEMOBJ)
#	g++ $(INCLUDEFLAGS) $(SYSTEMOBJ) -fPIC -shared -Wl,-soname,libsfml-system.so.2.1 -o libsfml-system.so.2.1
	ar rcs libsfml-system.a $(SYSTEMOBJ)
	
window: $(WINDOWOBJ)
#	g++ $(INCLUDEFLAGS) $(WINDOWOBJ) -fPIC -shared -Wl,-soname,libsfml-window.so.2.1 -o libsfml-window.so.2.1
	ar rcs libsfml-window.a $(WINDOWOBJ)
	
graphics: $(GRAPHICSOBJ)
#	g++ $(INCLUDEFLAGS) $(GRAPHICSOBJ) -fPIC -shared -Wl,-soname,libsfml-graphics.so.2.1 -o libsfml-graphics.so.2.1
	ar rcs libsfml-graphics.a $(GRAPHICSOBJ)
	
clean: 
	rm System/*.o
	rm System/Unix/*.o
	rm Window/*.o
	rm Window/Linux/*.o
	rm Graphics/*.o
	