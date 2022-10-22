#include "mems_data.hpp"

memsData::memsData()
{
  //ctor
}

memsData::~memsData()
{
  //dtor
}

/*
#include "models.h"
#include "ModelData.h"
#include <stdio.h>
#include <stdlib.h>
//--------------------------------------------------------------------------------------------------
#define INPUTS_DIN_COUNT (4)

#pragma pack(push)
#pragma pack(1)

struct _MEMS_SENSORS_LOG_ITEM
  {
  unsigned char StartByte; // 'S'
  unsigned char CheckSum; // 13
  unsigned short HRTime;
  short LAS[3];
  short ASS[3];
  };

struct _MEMS_LOG_ITEM_OLD
  {
  unsigned char StartByte;
  unsigned char CheckSum;
  unsigned char Reserved[6];
  double HRTime;
  double Time;
  double PosPolar[3];
  double Course;
  double VelPolar;
  double Position[3];
  double Velocity[3];
  double LAS[3];
  double filteredLAS[3];
  double ASS[3];
  };

struct _MEMS_LOG_ITEM_NEW
  {
  unsigned char StartByte;
  unsigned char CheckSum;
  unsigned char PPS;
  unsigned char Inputs[INPUTS_DIN_COUNT];
  unsigned char Reserved[8-3-INPUTS_DIN_COUNT];
  double HRTime;
  double HRTimeG;
  double Time;
  double PosPolar[3];
  double Course;
  double VelPolar;
  double Position[3];
  double Velocity[3];
  double LAS[3];
  //double reserved[3];
  double ASS[3];
  };
#pragma pack(pop)

typedef struct _MEMS_LOG_ITEM_NEW DATA_ITEM;

////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char XorSum(unsigned char PrevSum, unsigned char *Data, unsigned int Length)
{
while (Length--)
  PrevSum ^= *(Data++);

return (PrevSum);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
double get_rand(double scale = 1.0)
{
return ((rand() - RAND_MAX*0.5) * (scale/RAND_MAX));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void UploadFakePoints(HWND hDlg, HWND hProgressBar)
{
UNREFERENCED_PARAMETER(hDlg);
UNREFERENCED_PARAMETER(hProgressBar);

srand(GetTickCount());

MODEL_INPUT p = {0};
InputPoints.AllocPoints(static_cast<unsigned int>(3600.0 / dT) + 1);

InputPoints.SetKeyTimes(10.0, 3600.0);

do
  {
  p.Time = InputPoints.GetPointsNumber()*dT;
  //p.PPS = (InputPoints.GetPointsNumber() % MEMS_SAMLES_PER_SEC) == 0;

  p.VectorAccel = VECTOR3(cos(5*p.Time), cos(p.Time), cos(10*p.Time)*sin(10*p.Time));
  p.VectorGyro = VECTOR3(0, 0, 0);

  static VECTOR3 _gyro_offset;
  static VECTOR3 _accel_offset;//(0.0f, 0.0f, 9.81f);

  static VECTOR3 _gyro_velocity;
  static VECTOR3 _gyro_acceleration;
  static VECTOR3 _gyro_angle;

  static REAL t;

  p.VectorGyro = _gyro_offset;// + VECTOR3(get_rand(0.01), get_rand(0.01), get_rand(0.01));
  p.VectorAccel = _accel_offset;// + VECTOR3(get_rand(0.01), get_rand(0.01), get_rand(0.01));

  if (p.Time > 20.0)
    {
    VECTOR3 radius(0.0, 5.0, 0.0);

    _gyro_velocity += _gyro_acceleration * dT;
    _gyro_acceleration = VECTOR3(0.0, 0.0, -0.005 * (1.0 - _gyro_velocity.len2() * 5.0));

    VECTOR3 _velocity;
    _velocity = _gyro_velocity.cross(radius);

    VECTOR3 _accel_Y;
    _accel_Y = _gyro_velocity.cross(_velocity);

    VECTOR3 _accel_X;
    _accel_X = _gyro_acceleration.cross(radius);

    p.VectorGyro += _gyro_velocity;
    p.VectorAccel += _accel_X + _accel_Y + VECTOR3(0.0, 0.0, 0.05*cos(t * 0.1));

    t += dT;
    }

  //points[i].VectorAccel = VECTOR3(-2.0, 1.0, -0.5);
  //points[i].VectorGyro = VECTOR3(1, 0.5, 0.25);
  //VECTOR3 noise(get_rand(), get_rand(), get_rand());

  //VECTOR3 noise(get_rand(0.01), get_rand(0.01), get_rand(0.01));
  //p.VectorAccel = VECTOR3(0.0, 9.8065, 0.0) + noise;

  //points[i].VectorAccel = points[i].VectorAccel.unit();
  //p.VectorGyro = VECTOR3(0.0, M_PI / 200.0, 0.0)+noise;

  if (i < 500)
    {
      points[i].v.x = 0;
      points[i].v.z = 0;
      points[i].v.y = -10;
    }
  else if (i < 10500)
    {
    float alpha = ((i-500.0)/10000.0)*(M_PI_2);


      points[i].v.x = 0;
      points[i].v.z = 10*sin(alpha);
      points[i].v.y = -10*cos(alpha);
    }
  else
    {
      points[i].v.x = 0;
      points[i].v.z = 10;
      points[i].v.y = 0;
    }
  }
while (InputPoints.AddPoint(p));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
static char const *GetDoubleValue(char const *in_str, double* out_value)
{
char const *out_str = NULL;

if (in_str == NULL)
  {
  if (out_value != NULL)
    {
    *out_value = 0.0;
    }
  }
else
  {
  out_str = strchr(in_str, '\t');
  out_str = (out_str != NULL)? (out_str+1): NULL;

  if (out_value != NULL)
    {
    char *err_ch;
    *out_value = strtod(in_str, &err_ch);

    if (err_ch == in_str)
      {
      out_str = NULL;
      }
    }
  }

return (out_str);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void UploadFilePoints(FILE *fileData, HWND hDlg, HWND hProgressBar)
{
UNREFERENCED_PARAMETER(hDlg);
UNREFERENCED_PARAMETER(hProgressBar);

enum _FILE_TYPE
  {
  file_unknown = 0,
  file_xsens,
  file_inemo,
  file_custom,
  } file_type;
char line[256];

//if (fgets(line, ARRAYSIZE(line), fileData) != NULL)
  {
  //if (memcmp(line, "// Start Time:", 14) == 0)
  //  {
  //  file_type = file_xsens;
  //  }
  //else if (memcmp(line, "Timestamp", 9) == 0)
  //  {
  //  file_type = file_inemo;
  //  }
  file_type = file_custom;

  InputPoints.AllocPoints(UINT_MAX);


      static struct _FILE_DATA
			{
			unsigned __int32 no;
      VECTOR3 a;
      VECTOR3 g;
			} point;

  while (fread(&point, sizeof(point), 1, fileData) != 0)//fgets(line, ARRAYSIZE(line), fileData) != NULL)
    {
    MODEL_INPUT p = {0};
    char const* str = line;

    p.Time = InputPoints.GetPointsNumber() * dT;

    switch (file_type)
      {
      case file_xsens:
        str = GetDoubleValue(str, NULL); //p->counter
        str = GetDoubleValue(str, NULL); //p->timer
        str = GetDoubleValue(str, &p.VectorAccel.x);
        str = GetDoubleValue(str, &p.VectorAccel.y);
        str = GetDoubleValue(str, &p.VectorAccel.z);
        str = GetDoubleValue(str, NULL);//p->freeacc.x = strtod(str, &str);
        str = GetDoubleValue(str, NULL);//p->freeacc.y = strtod(str, &str);
        str = GetDoubleValue(str, NULL);//p->freeacc.z = strtod(str, &str);
        str = GetDoubleValue(str, &p.VectorGyro.x);
        str = GetDoubleValue(str, &p.VectorGyro.y);
        str = GetDoubleValue(str, &p.VectorGyro.z);
        str = GetDoubleValue(str, NULL);//p->velinc.x = strtod(str, &str);
        str = GetDoubleValue(str, NULL);//p->velinc.y = strtod(str, &str);
        str = GetDoubleValue(str, NULL);//p->velinc.z = strtod(str, &str);
        str = GetDoubleValue(str, NULL);//p->oriinc.w = strtod(str, &str);
        str = GetDoubleValue(str, NULL);//p->oriinc.x = strtod(str, &str);
        str = GetDoubleValue(str, NULL);//p->oriinc.y = strtod(str, &str);
        //str = GetDoubleValue(str, NULL);//p->oriinc.z = strtod(str, &str);
        break;

      case file_inemo:
        str = GetDoubleValue(str, NULL); //p->counter = strtod(str, &str);
        str = GetDoubleValue(str, &p.VectorAccel.x);
        str = GetDoubleValue(str, &p.VectorAccel.y);
        str = GetDoubleValue(str, &p.VectorAccel.z);
        str = GetDoubleValue(str, &p.VectorGyro.x);
        str = GetDoubleValue(str, &p.VectorGyro.y);
        str = GetDoubleValue(str, &p.VectorGyro.z);

        p.VectorAccel *= 9.80665 / 1000.0;
        p.VectorGyro *= M_PI / 180.0;
        break;

      case file_custom:
        p.Time = point.no * 0.0001;
        p.VectorAccel = point.a;
        p.VectorGyro = point.g;
        break;

      default:
        str = NULL;
        break;
      }

    if (str != NULL)
      {
      if (! InputPoints.AddPoint(p))
        {
        Application::ErrorMsg(TEXT("Overflow!\r\n"));
        break;
        }
      }
    }
  }

DATA_ITEM buf;
double offset = 0;
double old_time = 0.0, new_time;
//const double SCALE_LA = ((9.81f*24.0f)/32768.0f);

fseek(fileData, 0, SEEK_END);

points_count = _ftelli64(fileData) / sizeof(DATA_ITEM);
points = new MODEL_INPUT[points_count];

fseek(fileData, 0, SEEK_SET);
for (points_point_idx = 0; ; )
  {
  if (fread(&buf, sizeof(DATA_ITEM), 1, fileData) == 0)
    {
    break;
    }

  if (
      (buf.StartByte != '@')
   || (XorSum(sizeof(DATA_ITEM), (unsigned char *)&buf, sizeof(DATA_ITEM)))
     )
    {
    MessageBox(hDlg, L"Error - invalid record!", NULL, MB_ICONSTOP);
    break;
    }

  new_time = buf.HRTime;

  if (old_time > new_time)
    {
    offset += 1.0f;
    }

  old_time = new_time;

  double point_time = offset+new_time;
  const double begin_time = 0;//400;

  if (point_time >= begin_time)
    {
    MODEL_INPUT* point = points+points_point_idx;

    point->Time = point_time-begin_time;

    //point->PPS = buf.PPS != 0;

    point->VectorAccel.x = buf.LAS[0];
    point->VectorAccel.y = buf.LAS[1];
    point->VectorAccel.z = buf.LAS[2];

    point->VectorGyro.x = buf.ASS[0];
    point->VectorGyro.y = buf.ASS[1];
    point->VectorGyro.z = buf.ASS[2];

    point->PositionECEF.x = buf.Position[0];
    point->PositionECEF.y = buf.Position[1];
    point->PositionECEF.z = buf.Position[2];

    point->VelocityECEF.x = buf.Velocity[0];
    point->VelocityECEF.y = buf.Velocity[1];
    point->VelocityECEF.z = buf.Velocity[2];

    point->PositionBLH.x = buf.PosPolar[0];
    point->PositionBLH.y = buf.PosPolar[1];
    point->PositionBLH.z = buf.PosPolar[2];

    point->VelocityBLH.x = buf.VelPolar;
    point->VelocityBLH.y = D3DXToRadian(buf.Course);

    points_point_idx++;
    }

//  if ((points_point_idx & 65535) == 65535)
//    {
//    SendMessage(hProgressBar, PBM_DELTAPOS, 65536, 0);
//    }
  }

points_count = points_point_idx;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void PointsAnalyze(void)
{
static VECTOR3 Accel_M, Gyro_M;
static VECTOR3 Accel_D, Gyro_D;

unsigned int PointCount = InputPoints.GetPointsNumber();

for (unsigned int idx = 0; idx < PointCount; idx++)
  {
  Accel_M += InputPoints[idx].VectorAccel;
  Gyro_M += InputPoints[idx].VectorGyro;
  }
Accel_M /= PointCount;
Gyro_M /= PointCount;

for (unsigned int idx = 0; idx < PointCount; idx++)
  {
  VECTOR3 AccelDelta = InputPoints[idx].VectorAccel - Accel_M;
  Accel_D += AccelDelta.sqr();
  VECTOR3 GyroDelta = InputPoints[idx].VectorGyro - Gyro_M;
  Gyro_D += GyroDelta.sqr();
  }
Accel_D /= PointCount;
Accel_D = Accel_D.sqroot();
Gyro_D /= PointCount;
Gyro_D = Gyro_D.sqroot();

static int HystoCounters[100] = {0};

for (unsigned int idx = 0; idx < InputPoints.GetPointsNumber(); idx++)
  {
  VECTOR3 delta = InputPoints[idx].VectorAccel - M;

  int index = (delta.z * 900) + ARRAYSIZE(HystoCounters)/2;

  if (index < 0)
    {
    index = 0;
    }
  else if (index > (ARRAYSIZE(HystoCounters)-1))
    {
    index = ARRAYSIZE(HystoCounters)-1;
    }

  ++(HystoCounters[index]);
  }
}
*/
