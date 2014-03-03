
#include <cstdio>
#include <cmath>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "signal.hpp"
#include "oscillator.hpp"
#include "nelead6.hpp"
#include "settings.hpp"
#include <SFML/Graphics.hpp>
#include <map>
#include "bass.h"

#if defined(SFML_SYSTEM_WINDOWS)
#define COMPILE_WINDOWS
#endif 

#if defined(COMPILE_WINDOWS)
#include <windows.h>
#include "bassasio.h"

// ASIO function
DWORD CALLBACK AsioProc(BOOL input, DWORD channel, void *buffer, DWORD length, void *user)
{
	DWORD c=BASS_ChannelGetData((DWORD)user,buffer,length);
	if (c==-1) c=0; // an error, no data
  if (c!= length)
  {
    std::cout << "wanted " << length << "  got " << c << std::endl;
  }
	return c;
}

std::string handleBassAsioInitError()
{
  switch (BASS_ASIO_ErrorGetCode())
  {
    case BASS_ERROR_DEVICE:	return "The device number specified is invalid.";
    case BASS_ERROR_ALREADY: return "A device has already been initialized. You must call BASS_ASIO_Free before you can initialize again.";
    case BASS_ERROR_DRIVER:	return "The driver could not be initialized.";
    default: return "";
  }
}

std::string handleBassAsioStartError()
{
  switch (BASS_ASIO_ErrorGetCode())
  {
    case BASS_ERROR_INIT:	return "BASS_ASIO_Init has not been successfully called.";
    case BASS_ERROR_ALREADY:	return "The device has already been started.";
    case BASS_ERROR_NOCHAN:	return "No channels have been enabled.";
    case BASS_ERROR_UNKNOWN:	return "Some other mystery problem!"; 
    default: return "";
  }
}

#endif



std::string handleBassInitError()
{
  switch (BASS_ErrorGetCode())
  {
    case BASS_ERROR_DX:	return "DirectX (or ALSA on Linux or OpenSL ES on Android) is not installed.";
    case BASS_ERROR_DEVICE:	return "device is invalid.";
    case BASS_ERROR_ALREADY:	return "The device has already been initialized. BASS_Free must be called before it can be initialized again.";
    case BASS_ERROR_DRIVER:	return "There is no available device driver. The device may already be in use.";
    case BASS_ERROR_FORMAT:	return "The specified format is not supported by the device. Try changing the freq and flags parameters.";
    case BASS_ERROR_MEM:	return "There is insufficient memory.";
    case BASS_ERROR_NO3D:	return "Could not initialize 3D support.";
    case BASS_ERROR_UNKNOWN:	return "Some other mystery problem!";
    default: return "";
  }
}

std::string handleBassStreamCreateError()
{
  switch (BASS_ErrorGetCode())
  {
    case BASS_ERROR_INIT:	return "BASS_Init has not been successfully called.";
    case BASS_ERROR_NOTAVAIL:	return "Only decoding channels (BASS_STREAM_DECODE) are allowed when using the 'no sound' device. The BASS_STREAM_AUTOFREE flag is also unavailable to decoding channels.";
    case BASS_ERROR_FORMAT:	return "The sample format is not supported by the device/drivers. If the stream is more than stereo or the BASS_SAMPLE_FLOAT flag is used, it could be that they are not supported.";
    case BASS_ERROR_SPEAKER:	return "The specified SPEAKER flags are invalid. The device/drivers do not support them, they are attempting to assign a stereo stream to a mono speaker or 3D functionality is enabled.";
    case BASS_ERROR_MEM:	return "There is insufficient memory.";
    case BASS_ERROR_NO3D:	return "Could not initialize 3D support.";
    case BASS_ERROR_UNKNOWN:	return "Some other mystery problem!";
    default: return "";
  }
}

std::string handleBassChannelPlayError()
{
  switch (BASS_ErrorGetCode())
  {
    case BASS_ERROR_HANDLE:	return "handle is not a valid channel.";
    case BASS_ERROR_START:	return "The output is paused/stopped, use BASS_Start to start it.";
    case BASS_ERROR_DECODE:	return "The channel is not playable; it is a 'decoding channel'.";
    case BASS_ERROR_BUFLOST:	return "Should not happen... check that a valid window handle was used with BASS_Init.";
    case BASS_ERROR_NOHW:	return "No hardware voices are available (HCHANNEL only). This only occurs if the sample was loaded/created with the BASS_SAMPLE_VAM flag and BASS_VAM_HARDWARE is set in the sample's VAM mode, and there are no hardware voices available to play it.";
    default: return "";
  }
}

int main()
{
  srand(time(NULL));
  Settings::getInstance().loadFile("settings.ini");
  unsigned int signalFrequency = GetSettingsFor("Audio/SignalFrequency",44100);
  unsigned int signalRefreshRate = GetSettingsFor("Audio/SignalRefreshRate",50);
  Signal::globalConfiguration(signalFrequency,signalRefreshRate);
  
  if (HIWORD(BASS_GetVersion())!=BASSVERSION) {
		#if defined(COMPILE_WINDOWS)
    MessageBox(NULL,"An incorrect version of BASS was loaded", "BASS Load Error", MB_OK | MB_ICONERROR);
    #endif
    std::cerr << "Incorrect BASS version loaded excepted " << BASSVERSION << " got " << HIWORD(BASS_GetVersion()) << std::endl;
    return 0xDEAD;
	}
  
  BOOL correct_bass_init = FALSE;
  int selected_driver=0;
  HSTREAM stream;
  #if defined(COMPILE_WINDOWS)
  bool useAsioDriver = GetSettingsFor("Audio/UseAsioDriver",true);
  BASS_ASIO_INFO info;
  if (useAsioDriver)
  {
    int count;
    BASS_ASIO_DEVICEINFO device_info;
    if (GetSettingsFor("Audio/AddAsio4AllAsDriver",false))
    {
      GUID driver_id;
      DWORD asio4all_id = BASS_ASIO_AddDevice(&driver_id,"asio4all.dll","Asio4All");
      if (asio4all_id == -1)
      {
        MessageBox(NULL,"Unable to load Asio4ALL driver (DLL not found)", "ASIO4ALL Load Error", MB_OK | MB_ICONERROR);
        std::cerr << "Unable to load Asio4ALL driver (DLL not found)" << std::endl;
      }
      else
      {
        std::cerr << "Asio4All loaded from asio4all.dll device id: " << asio4all_id << std::endl;
      }
    }
    
    std::cout << "Looking for ASIO drivers available..." << std::endl;
    for (count=0; BASS_ASIO_GetDeviceInfo(count, &device_info); count++)
    {
      std::cout << "Device " << count << std::endl;
      std::cout << "  Name: " << device_info.name << std::endl;
      std::cout << "  Driver: " << device_info.driver << std::endl;
    }
    std::cout << count <<  " Driver(s) available" << std::endl;
    
    selected_driver = GetSettingsFor("Audio/AsioDriverID",0);
    if (count)
    {
      if (selected_driver >= count)
      {
        MessageBox(NULL,"The ASIO driver selected in the settings file doesn't not exists, now load default driver", "Missing ASIO Driver", MB_OK | MB_ICONWARNING);
        std::cout << "The ASIO driver selected in the settings file doesn't not exists, now load default driver" << std::endl;
        selected_driver=0;
      }
      BASS_SetConfig(BASS_CONFIG_UPDATEPERIOD,0);
      //setup BASS - "no sound" device
      correct_bass_init = BASS_Init(0,48000,0,0,NULL);
      Signal::globalConfiguration(48000,Signal::refreshRate);
    }
    else
    {
      MessageBox(NULL,"Unable to find an ASIO driver, now start without", "No ASIO Driver Founds", MB_OK | MB_ICONWARNING);
      std::cout << "Unable to find an ASIO driver start without" << std::endl;
      useAsioDriver=false;
      correct_bass_init = BASS_Init(-1, Signal::frequency, 0,0,0);
    }
  }
  else {
  #endif
    correct_bass_init = BASS_Init(-1, Signal::frequency, 0,0,0);
  #if defined(COMPILE_WINDOWS)
  }
  #endif
  
  
  if (correct_bass_init==FALSE)
  {
    std::string error = "Error at BASS initialisation: ";
    error += handleBassInitError();
    #if defined(COMPILE_WINDOWS)
    MessageBox(NULL,error.c_str(), "BASS Init Error", MB_OK | MB_ICONERROR);
    #endif
    std::cerr << error << std::endl;
    return 0xDEAD;
  }
  else
  {
    std::cout << "BASS Initialised (" << Signal::frequency << "Hz)" << std::endl;
  }
  #if defined(COMPILE_WINDOWS)
  if (useAsioDriver)
  {
    // setup ASIO - first device
    if (!BASS_ASIO_Init(selected_driver,BASS_ASIO_THREAD))
    {
      std::string error="Error BASS ASIO initialisation failed: ";
      error+=handleBassAsioInitError();
      MessageBox(NULL,error.c_str(), "BASS ASIO Init Error", MB_OK | MB_ICONERROR);
      std::cerr << error << std::endl;
    }
    
    // start device info for buffer size range, begin with default
    BASS_ASIO_GetInfo(&info);
    if (!BASS_ASIO_SetRate(Signal::frequency))
    {
      //change global configuration 
      std::cout << "Warning: Cannot set Signal frequency to ASIO sample rate, this will reconfigure alls signals now" << std::endl;
      Signal::globalConfiguration(BASS_ASIO_GetRate(),Signal::refreshRate);
    }
    stream=BASS_StreamCreate(Signal::frequency,
                             Signal::channels, BASS_STREAM_DECODE,
                             STREAMPROC_PUSH,(void*) 0);
  }
  else
  {
  #endif
  stream=BASS_StreamCreate(Signal::frequency,
                           Signal::channels,0,
                           STREAMPROC_PUSH,(void*) 0);
  #if defined(COMPILE_WINDOWS)
  }
  #endif

  
                                   
  if (stream)
  {
    std::cout << "BASS Stream Created [" << Signal::frequency << "Hz, " << Signal::channels << " Channels]" << std::endl;
  }
  else
  {
    std::string error = "Error at BASS Stream Creation: ";
    error += handleBassStreamCreateError();
    #if defined(COMPILE_WINDOWS)
    MessageBox(NULL,error.c_str(), "BASS Stream Create Error", MB_OK | MB_ICONERROR);
    #endif
    std::cerr << error << std::endl;
    return 0xDEAD;
  }
  
  #if defined(COMPILE_WINDOWS)
  if (useAsioDriver)
  {
    unsigned right=GetSettingsFor("Audio/AsioDriverChannelRight",0),left=GetSettingsFor("Audio/AsioDriverChannelLeft",1);
    unsigned count;
    BASS_ASIO_CHANNELINFO channel_info;
    std::cout << "Looking for Channels available..." << std::endl;
    for (count=0; BASS_ASIO_ChannelGetInfo(0,count, &channel_info); count++)
    {
      std::cout << "Device " << count << std::endl;
      std::cout << "  Name: " << channel_info.name << std::endl;
      std::cout << "  Group: " << channel_info.group << std::endl;
    }
     std::cout << count << " Channels available..." << std::endl;
    
    if (count < 2)
    {
      MessageBox(NULL,"Error the driver not provide stereo channels", "Need 2 channels", MB_OK | MB_ICONERROR);
      return 0xdead;
    }
    if (right >= count || left >= count)
    {
      right=0;
      left=1;
    }
    
    if (!BASS_ASIO_ChannelEnable(0,right,&AsioProc,(void*)stream))// enable 1st output channel...
      std::cerr << "Cannot enable channel right (" << right << ")" << std::endl;
    if (!BASS_ASIO_ChannelJoin(0,left,right)) // and join the next channel to it (stereo)
      std::cerr << "Cannot enable channel left (" << left << ")" << std::endl;
    BASS_ASIO_ChannelSetFormat(0,0,BASS_ASIO_FORMAT_16BIT); // set the source format (16-bit)
    // start the ASIO device
    if (!BASS_ASIO_Start(0,0))
    {
      std::string error = handleBassAsioStartError();
      MessageBox(NULL,error.c_str(), "Can't start ASIO device", MB_OK | MB_ICONERROR);
      std::cerr << "Can't start ASIO device: " << error << std::endl;
      return 0xdead;
    }
  }
  #endif
  
	
  
                                    
  //reserve place for our buffer
  BASS_StreamPutData(stream,0,Signal::byteSize*2);
  
  #if defined(COMPILE_WINDOWS)
  if (!useAsioDriver)
  {
  #endif  
  if (BASS_ChannelPlay(stream,TRUE))
  {
    std::cout << "BASS Channel Started !" << std::endl;
  }
  else 
  {
      std::string error = "Failed to start channel: ";
      error += handleBassChannelPlayError();
      #if defined(COMPILE_WINDOWS)
      MessageBox(NULL,error.c_str(), "BASS Channel Error", MB_OK | MB_ICONERROR);
      #endif
      std::cerr << error << std::endl;
      return 0xdead;
  }
  #if defined(COMPILE_WINDOWS)
  }
  #endif
  
  sf::RenderWindow window(sf::VideoMode(800, 600), "Millenium Synth");
  
  float dt=0.02;
  unsigned int time=0;
  
  
  
  
  NELead6 lead;
  Signal output;
  std::map<sf::Keyboard::Key,Note*> notes;
  
  /*
  AudioStream s;
  HSTREAM stream = s.createCompatibleBassStream(use_callback);
  
  
  if (use_callback) 
  {
    window.setFramerateLimit(50); //Detruit tout ne pas activer
    // update period min 5 ms
    if (BASS_SetConfig(BASS_CONFIG_UPDATEPERIOD,10))
    {
      std::cout << "Bass period correctly set to " << BASS_GetConfig(BASS_CONFIG_UPDATEPERIOD) << " ms" << std::endl;
    }
    else
    {
      std::cerr << "Bass period error unable set it to " << BASS_GetConfig(BASS_CONFIG_UPDATEPERIOD) << " ms" << std::endl;
    }
    
    if (BASS_SetConfig(BASS_CONFIG_BUFFER,AudioStream::nBuffers*1000/Signal::refreshRate))
    {
      std::cout << "Bass buffer correctly set to " << AudioStream::nBuffers*1000/Signal::refreshRate << " ms" << std::endl;
    }
    else
    {
      std::cerr << "Bass buffer error unable set it to " << AudioStream::nBuffers*1000/Signal::refreshRate << " ms" << std::endl;
    }
  }
  */
  short* finalSamples = (short*) malloc(Signal::size*2);
  
  while (window.isOpen())
  {
  // Process events
    sf::Event event;
    while (window.pollEvent(event))
    {
      
      switch (event.type)
      {
        case sf::Event::Closed:
          window.close(); // Close window : exit
          break;
        case sf::Event::Resized:
          {
            sf::View newView(sf::FloatRect(0,0,event.size.width,event.size.height));
            window.setView(newView);
          }
          break;
        case sf::Event::MouseButtonPressed:
          if (event.mouseButton.button == sf::Mouse::Left)
          {
          }
          break;
          
        case sf::Event::KeyPressed:
          {
            float f=0;
            switch (event.key.code)
            {
              case sf::Keyboard::Q: f=261.63; break;
              case sf::Keyboard::Z: f=277.18; break;
              case sf::Keyboard::S: f=293.66; break;
              case sf::Keyboard::E: f=311.13; break;
              case sf::Keyboard::D: f=329.63; break;
              case sf::Keyboard::F: f=349.23; break;
              case sf::Keyboard::T: f=369.99; break;
              case sf::Keyboard::G: f=392.00; break;
              case sf::Keyboard::Y: f=415.30; break;
              case sf::Keyboard::H: f=440.00; break;
              case sf::Keyboard::U: f=466.16; break;
              case sf::Keyboard::J: f=493.88; break;
              default: break;
            }
            if (f)
            {
              if (notes.find(event.key.code) == notes.end())
              {
                notes[event.key.code] = new Note(time,f,1.0);
                lead.playNote(*notes[event.key.code]);
              }  
            }
          }
          break;
        case sf::Event::KeyReleased:
          if (notes.find(event.key.code) != notes.end())
          {
            delete notes[event.key.code];
            notes.erase(event.key.code);
          }
          break;
        default:
          break;
      }
    }
    
    DWORD buffed = BASS_StreamPutData(stream,0,0); 
    if (buffed < Signal::size*2)
    {
      lead.step(&output);
      time++;
      for (unsigned int i=0; i < Signal::size;i++)
      {
        finalSamples[i]=output.samples[i]*32000;
      }
      BASS_StreamPutData(stream,(void*)finalSamples,Signal::size*2); 
    }
    
    /*if (s.prepareNextBuffer())
    {
      Signal* signal = s.getPreparedBuffer();
      if (signal)
      {
        lead.step(signal);
        time++;
      }
      
    }
    sf::sleep(sf::microseconds(1000));
    
    if (!use_callback && s.pushInBass())
    {
    }*/
    
    
    
    // Update the window
    window.display();
    
    //std::cout << "CPU usage : " << BASS_ASIO_GetCPU() << std::endl;
  }
  #if defined(COMPILE_WINDOWS)
  if (useAsioDriver)
    BASS_ASIO_Free();
  #endif
  BASS_Free();
  
  return 0xdead;
}