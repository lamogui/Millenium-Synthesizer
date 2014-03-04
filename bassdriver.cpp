

#include "bassdriver.hpp"
#include <iostream>

#if defined(COMPILE_WINDOWS)
#include <windows.h>
#endif

BassDriver::BassDriver() :
_stream(0),
_initialized(false),
_running(false)
{
  
}

BassDriver::~BassDriver()
{
  free();
}

bool BassDriver::init(unsigned int rate)
{
  if (_initialized)
  {
    std::cerr << "Error you must free BASS driver before initialize it again" << std::endl;
    return false;
  }

  if (HIWORD(BASS_GetVersion())!=BASSVERSION) {
		#if defined(COMPILE_WINDOWS)
    MessageBox(NULL,"An incorrect version of BASS was loaded", "BASS Load Error", MB_OK | MB_ICONERROR);
    #endif
    std::cerr << "Incorrect BASS version loaded excepted " << BASSVERSION << " got " << HIWORD(BASS_GetVersion()) << std::endl;
    return false;
	}
  
  if (BASS_Init(-1, rate, BASS_DEVICE_LATENCY,0,0)==FALSE)
  {
    std::string error = "Error at BASS initialisation: ";
    error += handleBassInitError();
    #if defined(COMPILE_WINDOWS)
    MessageBox(NULL,error.c_str(), "BASS Init Error", MB_OK | MB_ICONERROR);
    #endif
    std::cerr << error << std::endl;
    return false;
  }
  std::cout << "BASS Initialised (" << rate << "Hz)" << std::endl;
  
  BASS_INFO info;
  BASS_SetConfig(BASS_CONFIG_UPDATEPERIOD,10);
  BASS_GetInfo(&info);
	// default buffer size = update period + 'minbuf' + 1ms extra margin
	BASS_SetConfig(BASS_CONFIG_BUFFER,10+info.minbuf+1);
	std::cout << "BASS buffer " << BASS_GetConfig(BASS_CONFIG_BUFFER) << "ms" <<  std::endl;
  std::cout << "Device latency " << info.latency << "ms" << std::endl;
  
  _initialized=true;
  return true;
}


void BassDriver::free()
{
  if (_initialized)
  {
    stop();
    BASS_Free();
    _initialized=false;
  }
}

bool BassDriver::start(AudioStream* stream)
{
  if (!_initialized)
  {
    std::cerr << "Error you must initialize BASS driver before use it" << std::endl;
    return false;
  }

  if (_running)
  {
    std::cerr << "Error you must stop BASS driver before start it again" << std::endl;
    return false;
  }
  if (!stream)
  {
    std::cerr << "Error: No stream to play" << std::endl;
    return false;
  }
  
  _stream=BASS_StreamCreate(Signal::frequency,
                            Signal::channels,0,
                            _StreamProc,(void*) stream);                            
  if (_stream)
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
    return false;
  }
  
  if (BASS_ChannelPlay(_stream,TRUE))
  {
    std::cout << "BASS Play Started !" << std::endl;
  }
  else 
  {
      
      std::string error = "Failed to start channel: ";
      error += handleBassChannelPlayError();
      #if defined(COMPILE_WINDOWS)
      MessageBox(NULL,error.c_str(), "BASS Channel Error", MB_OK | MB_ICONERROR);
      #endif
      std::cerr << error << std::endl;
      BASS_StreamFree(_stream);
      return false;
  }
  _running=true;
  return true;
}

void BassDriver::stop()
{
  if (!_initialized)
  {
    std::cerr << "Error you must initialize BASS driver before use it" << std::endl;
    return;
  }
  if (_running)
  {
    BASS_StreamFree(_stream);
    _running=false;
  }
}

DWORD CALLBACK BassDriver::_StreamProc(HSTREAM handle,
                                       void *buffer,
                                       DWORD length,
                                       void *user)
{
  sf::Lock(*((AudioStream*) user));
  unsigned c = 2*(((AudioStream*) user)->read((unsigned short*)buffer,length/2));
  /*if (!c)
  {
    std::cout << "Critical here: BASS want " << length << " but there is no data in queue" << std::endl;
  }*/
  return c;
}

#if defined(COMPILE_WINDOWS)

BassAsioDriver::BassAsioDriver():
_initialized(false),
_running(false),
_right(0),
_left(1)
{

}

BassAsioDriver::~BassAsioDriver()
{
  free();
}

bool BassAsioDriver::init(unsigned int rate)
{
  if (_initialized)
  {
    std::cerr << "Error BASS ASIO already initialized !" << std::endl;
    return false;
  }
  int count;
  int selected_driver=-1;
  BASS_ASIO_DEVICEINFO device_info;
  BASS_ASIO_INFO info;
  /*if (GetSettingsFor("ASIO/AddAsio4AllAsDriver",false))
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
  }*/
  
  std::cout << "Looking for ASIO drivers available..." << std::endl;
  std::string driver_name = GetSettingsFor("ASIO/Driver",std::string(""));
  
  for (count=0; BASS_ASIO_GetDeviceInfo(count, &device_info); count++)
  {
    std::cout << "Device " << count << std::endl;
    std::cout << "  Name: " << device_info.name << std::endl;
    std::cout << "  Driver: " << device_info.driver << std::endl;
    
    if (driver_name.empty())
    {
      driver_name = device_info.name;
      SetSettings("ASIO/Driver",driver_name);
      selected_driver=count;
    }
    else if (driver_name == device_info.name)
    {
      selected_driver=count;
    }
  }
  std::cout << count <<  " Driver(s) available" << std::endl;
  
  if (count)
  {
    if (selected_driver == -1)
    {
      MessageBox(NULL,"The ASIO driver selected in the settings file doesn't exists, now load default driver", "Missing ASIO Driver", MB_OK | MB_ICONWARNING);
      std::cout << "The ASIO driver selected in the settings file doesn't exists, now load default driver" << std::endl;
      selected_driver=0;
    }
    // setup ASIO - first device
    if (!BASS_ASIO_Init(selected_driver,BASS_ASIO_THREAD))
    {
      std::string error="Error BASS ASIO initialisation failed: ";
      error+=handleBassAsioInitError();
      MessageBox(NULL,error.c_str(), "BASS ASIO Init Error", MB_OK | MB_ICONERROR);
      std::cerr << error << std::endl;
      return false;
    }
    
    // start device info for buffer size range, begin with default
    BASS_ASIO_GetInfo(&info);
    if (!BASS_ASIO_SetRate(rate))
    {
      //change global configuration 
      std::cout << "Warning: Cannot set ASIO sample rate to " << rate << "Hz, this will reconfigure alls signals frequency at " << BASS_ASIO_GetRate() << "Hz now" << std::endl;
      Signal::globalConfiguration(BASS_ASIO_GetRate(),Signal::refreshRate);
    }
    else
    {
      std::cout << "ASIO Rate set to " << Signal::frequency << std::endl;
    }
    _initialized=true;
    return true;
  }
  else
  {
    MessageBox(NULL,"Unable to find an ASIO driver, now start without", "No ASIO Driver Founds", MB_OK | MB_ICONWARNING);
    std::cout << "Unable to find an ASIO driver start without" << std::endl;
    return false;
  }
}

void BassAsioDriver::free()
{
  if (_initialized)
  {
    BASS_ASIO_Free();
    _initialized=false;
    _running=false;
  }
}

bool BassAsioDriver::start(AudioStream* stream)
{
  unsigned count;
  _right=GetSettingsFor("ASIO/ChannelRightID",0);
  _left=GetSettingsFor("ASIO/ChannelLeftID",1);
  BASS_ASIO_CHANNELINFO channel_info;
  std::cout << "Looking for Channels available..." << std::endl;
  for (count=0; BASS_ASIO_ChannelGetInfo(0,count, &channel_info); count++)
  {
    std::cout << "Device ID: " << count << std::endl;
    std::cout << "  Name: " << channel_info.name << std::endl;
    std::cout << "  Group: " << channel_info.group << std::endl;
  }
   std::cout << count << " Channels available..." << std::endl;
  
  if (count < Signal::channels)
  {
    MessageBox(NULL,"Error the driver not provide stereo channels", "Need 2 channels", MB_OK | MB_ICONERROR);
    return false;
  }
  if (_right >= count || _left >= count)
  {
    _right=0;
    _left=1;
  }
  
  if (!BASS_ASIO_ChannelEnable(0,_right,&_AsioProc,(void*)stream))// enable 1st output channel...
    std::cerr << "Cannot enable channel right (Device ID: " << _right << ")" << std::endl;
  if (!BASS_ASIO_ChannelJoin(0,_left,_right)) // and join the next channel to it (stereo)
    std::cerr << "Cannot enable channel left (Device ID: " << _left << ")" << std::endl;
  BASS_ASIO_ChannelSetFormat(0,_right,BASS_ASIO_FORMAT_16BIT); // set the source format (16-bit)
  // start the ASIO device
  if (!BASS_ASIO_Start(0,0))
  {
    std::string error = handleBassAsioStartError();
    MessageBox(NULL,error.c_str(), "Can't start ASIO device", MB_OK | MB_ICONERROR);
    std::cerr << "Can't start ASIO device: " << error << std::endl;
    return false;
  }
  else
  {
    std::cout << "ASIO Device Started !" << std::endl;
    _running=true;
    return true;
  }
}

void BassAsioDriver::stop()
{
  if (!_initialized)
  {
    std::cerr << "Error you must initialize ASIO driver before use it" << std::endl;
    return;
  }
  if (_running)
  {
    BASS_ASIO_Stop();
    std::cout << "ASIO Device Stopped !" << std::endl;
    BASS_ASIO_ChannelReset(FALSE, -1, BASS_ASIO_RESET_ENABLE|BASS_ASIO_RESET_JOIN);
    _running=false;
  }
}

DWORD CALLBACK BassAsioDriver::_AsioProc(BOOL input, 
                                         DWORD channel, 
                                         void *buffer, 
                                         DWORD length, 
                                         void *user)
{
  if (input) return 0;
  sf::Lock lock(*((AudioStream*) user));
  unsigned c = 2*(((AudioStream*) user)->read((unsigned short*)buffer,length/2));
  /*if (!c)
  {
    std::cout << "Critical here: ASIO want " << length << " but there is no data in queue" << std::endl;
  }*/
  return c;
}

#endif