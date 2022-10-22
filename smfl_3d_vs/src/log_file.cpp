#include "log_file.hpp"

logFile::logFile()
{
  //ctor
}

logFile::~logFile()
{
  //dtor
}

/*

namespace Log
{
static FILE *LogFile = NULL;

unsigned long Begin(wchar_t *DataFile)
{
WCHAR path[MAX_PATH];

if (DataFile != NULL)
  {
  _sntprintf(path, MAX_PATH, L"%s.modelout", DataFile);
  }
else
  {
  _sntprintf(path, MAX_PATH, L"freerun_%lu.modelout", GetTickCount());
  }

LogFile = _tfopen(path, L"wb");

if (LogFile == NULL)
  {
  Application::ErrorMsg(L"Start logging error!");
  LogFile = stdout; // мегафикс
  }

fprintf(LogFile,
          "Time\t"
          "AccelPure.x\tAccelPure.y\tAccelPure.z\t"
          "AccelOffset.x\tAccelOffset.y\tAccelOffset.z\t"
          "Accel.x\tAccel.y\tAccel.z\t"
          "GyroPure.x\tGyroPure.y\tGyroPure.z\t"
          "GyroOffset.x\tGyroOffset.y\tGyroOffset.z\t"
          "Gyro.x\tGyro.y\tGyro.z\t"
          "Velocity.x\tVelocity.y\tVelocity.z\t"
          "Translation.x\tTranslation.y\tTranslation.z\t"
          "\r\n");
return ERROR_SUCCESS;
}

unsigned long End(void)
{
if (LogFile != NULL)
  {
  fclose(LogFile);
  }

return ERROR_SUCCESS;
}

unsigned long Write(unsigned long MessageType, wchar_t *Message)
{
UNREFERENCED_PARAMETER(MessageType);
UNREFERENCED_PARAMETER(Message);
return ERROR_SUCCESS;
}

FILE *GetOutFile(void)
{
return (LogFile);
}
}

*/
