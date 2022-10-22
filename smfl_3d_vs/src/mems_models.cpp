#include "mems_models.hpp"

memsModels::memsModels()
{
  //ctor
}

memsModels::~memsModels()
{
  //dtor
}

/*
#include "Models.h"
#include "Common.h"
#include "Physics.h"
#include "CFC.h"
#include <limits>

template <typename T, int N> class CWindowFilter
  {
  private:
    int idx;
    T data[N];

  public:
    void Init(T value)
      {
      for (idx = 0; idx < N; ++idx)
        {
        data[idx] = value;
        }
      }

    T Step(T value)
      {
      ++idx;
      if (idx >= N)
        {
        idx = 0;
        }

      data[idx] = value;

      T Sum = data[0];
      for (int no = 1; no < N; ++no)
        {
        Sum += data[no];
        }
      return (Sum / N);
      }
  };
//--------------------------------------------------------------------------------------------------
C_MODEL_FLOAT FloatModel;
C_MODEL_INTEGER IntegerModel;
C_INPUT_POINTS InputPoints;
//--------------------------------------------------------------------------------------------------
MODEL_INPUT *C_INPUT_POINTS::Points;
unsigned int C_INPUT_POINTS::Number;
unsigned int C_INPUT_POINTS::Capacity;
double C_INPUT_POINTS::BeginTime;
double C_INPUT_POINTS::EndTime;
//--------------------------------------------------------------------------------------------------
C_INPUT_POINTS::C_INPUT_POINTS()
{
Points = NULL;
Number = 0;
Capacity = 0;
}
//--------------------------------------------------------------------------------------------------
C_INPUT_POINTS::~C_INPUT_POINTS()
{
CleanUp();
}
//--------------------------------------------------------------------------------------------------
void C_INPUT_POINTS::CleanUp()
{
SAFE_DELETE(Points);
Number = 0;
Capacity = 0;
}
//--------------------------------------------------------------------------------------------------
void C_INPUT_POINTS::AllocPoints(unsigned int PointsCount)
{
CleanUp();

PointsCount = std::min(3000000U, PointsCount);

Points = new MODEL_INPUT[PointsCount];
Capacity = PointsCount;
}
//--------------------------------------------------------------------------------------------------
void C_INPUT_POINTS::SetKeyTimes(double IdleTime, double IntegrateTime)
  {
  BeginTime = IdleTime;
  EndTime = BeginTime + IntegrateTime;
  }
//--------------------------------------------------------------------------------------------------
bool C_INPUT_POINTS::AddPoint(MODEL_INPUT const &Point)
{
if (Number < Capacity)
  {
  Points[Number] = Point;
  Number++;
  return (true);
  }

return (false);
}
//--------------------------------------------------------------------------------------------------
void C_MODEL::Init(char *newTitle)
{
Title = newTitle;
}

//--------------------------------------------------------------------------------------------------

float LimitA_Low = 1;
float LimitA_High =5;
float LimitS_Move = 0.15f; // 0.45 м/с2 -> 1 метр проедем

float integ_time;

CFCF_FILTER GravityFilterX;
CFCF_FILTER GravityFilterY;
CFCF_FILTER GravityFilterZ;

CFCF_FILTER SliceFilterX;
CFCF_FILTER SliceFilterY;
CFCF_FILTER SliceFilterZ;


const float Limit_Pitch = 1.0;

float velECEFOld, velECEFNew;
float velPolOld, velPolNew;
float courseOld, courseNew;

static float boost, retard, driftl, driftr;

// Static
//M	{x=-1.1447966202445652 y=0.21007133152173912 z=-9.6695132048233692 }	VECTOR3
//D	{x=0.17686719935291434 y=0.18314026572520606 z=0.18276737606256482 }	VECTOR3
//M2	9.7393108012992489	double

//const VECTOR3 StaticG     (-1.1447966202445652f, 0.21007133152173912f, -9.6695132048233692f);
//const VECTOR3 StaticNoise (0.17686719935291434f, 0.18314026572520606f, 0.18276737606256482f);

static unsigned int modelTickCounter;

static float max_x = 0;
static float max_y = 0;
static float max_z = 0;
static float max_m = 0;


//static ClassWindowFilter<VECTOR3, 64> FilterWindow;


enum _CALIB
  {
  calibstate_Ready = 0,
  calibstate_StaticPrepare = 0,
  calibstate_StaticCalib = 1,
  calibstate_DynamicPrepare = 2,
  calibstate_DynamicCalib = 3,
  };

enum _CALIB_STATUS
  {
  calibstatus_Uncomplete = 0,
  calibstatus_Success = 1,
  calibstatus_Fail = 2,
  };

enum _CALIB_STATUS calibStatus;
enum _CALIB        calibState;
VECTOR3            calibVector;
VECTOR3            calibVectorG;
VECTOR3            calibVectorV;
unsigned int       calibIndex;


CFCF_FILTER FilterAccelX;
CFCF_FILTER FilterAccelY;
CFCF_FILTER FilterAccelZ;
CFCF_FILTER FilterGyroX;
CFCF_FILTER FilterGyroY;
CFCF_FILTER FilterGyroZ;

MATRIX4x4 gyro_calib_matrix;
MATRIX4x4 accel_calib_matrix;

CWindowFilter<REAL, 64> fx;
CWindowFilter<REAL, 64> fy;
CWindowFilter<REAL, 64> fz;

//--------------------------------------------------------------------------------------------------
void C_MODEL::WritePoint(double PointTime)
{
static char str[1024];
unsigned int len = _snprintf(str, ARRAYSIZE(str),
  "%.15lg\t"
  "%.15lg\t%.15lg\t%.15lg\t"
  "%.15lg\t%.15lg\t%.15lg\t"
  "%.15lg\t%.15lg\t%.15lg\t"
  "%.15lg\t%.15lg\t%.15lg\t"
  "%.15lg\t%.15lg\t%.15lg\t"
  "%.15lg\t%.15lg\t%.15lg\t"
  "%.15lg\t%.15lg\t%.15lg\t"
  "%.15lg\t%.15lg\t%.15lg\t"
  "\r\n",
  PointTime,
  VectorAccelPure.x, VectorAccelPure.y, VectorAccelPure.z,
  VectorAccelOffset.x, VectorAccelOffset.y, VectorAccelOffset.z,
  VectorAccel.x, VectorAccel.y, VectorAccel.z,
  VectorGyroPure.x, VectorGyroPure.y, VectorGyroPure.z,
  VectorGyroOffset.x, VectorGyroOffset.y, VectorGyroOffset.z,
  VectorGyro.x, VectorGyro.y, VectorGyro.z,
  VectorVelocity.x, VectorVelocity.y, VectorVelocity.z,
  VectorTranslation.x, VectorTranslation.y, VectorTranslation.z);

fwrite(&str, len, 1, Log::GetOutFile());
}

//--------------------------------------------------------------------------------------------------
// Float model
void C_MODEL_FLOAT::StepFirst(MODEL_INPUT const &Input)
{
BeginTime = 10;
EndTime = BeginTime + 3600;

const int line_points = 10;

LineAccel = new GRAPHIC_LINE(line_points, D3DCOLOR_XRGB(0x3F, 0x3F, 0x3F), 1.0f);
LineAccelOffset = new GRAPHIC_LINE(line_points, 0xFF1F1F1F, 1.0f);
LineVelocity = new GRAPHIC_LINE(line_points, 0xFFAF0000, 1.0f);
LineTranslation = new GRAPHIC_LINE(line_points, 0xFF00AF00, 1.0f);

//VectorVelocity = VECTOR3(5.0, 0.0, 0.0);

//GravityFilterX.Init(0.025f, dT, points[0].VectorA.x);
//GravityFilterY.Init(0.025f, dT, points[0].VectorA.y);
//GravityFilterZ.Init(0.025f, dT, points[0].VectorA.z);

//FilterWindow.Reset(Input.VectorAccel);

//CFCF_Filter_Init(&SliceFilterX, 0.0015, 1, points[0].VectorA.x);
//CFCF_Filter_Init(&SliceFilterY, 0.0015, 1, points[0].VectorA.y);
//CFCF_Filter_Init(&SliceFilterZ, 0.0015, 1, points[0].VectorA.z);

//calibState = calibstate_StaticPrepare;

MATRIX4x4::identity(gyro_calib_matrix);
MATRIX4x4::identity(accel_calib_matrix);

FilterAccelX.Init(1.0, dT, Input.VectorAccel.x);
FilterAccelY.Init(1.0, dT, Input.VectorAccel.y);
FilterAccelZ.Init(1.0, dT, Input.VectorAccel.z);

FilterGyroX.Init(1.0, dT, Input.VectorGyro.x);
FilterGyroY.Init(1.0, dT, Input.VectorGyro.y);
FilterGyroZ.Init(1.0, dT, Input.VectorGyro.z);

WritePoint(std::numeric_limits<double>::quiet_NaN());

fx.Init(Input.VectorAccel.x);
fx.Init(Input.VectorAccel.y);
fx.Init(Input.VectorAccel.z);
}

//--------------------------------------------------------------------------------------------------

//static VECTOR2 lastpolarVel;
//static double lastECEFTime;
//static VECTOR2 gnss_accel;

//static VECTOR3 accel_average;
//static VECTOR3 accel_sum;
//static unsigned int accel_aver_number;

void C_MODEL_FLOAT::StepNext(const MODEL_INPUT &Input)
{
// Частотный анализ
//FFT_Push(FFT_x, Input->VectorAccel.x);
//FFT_Push(FFT_y, Input->VectorAccel.y);
//FFT_Push(FFT_z, Input->VectorAccel.z);

// Данные от акселерометра
Time = Input.Time;

//VectorAccelPure.x = fx.Step(Input.VectorAccel.x);
//VectorAccelPure.y = fy.Step(Input.VectorAccel.y);
//VectorAccelPure.z = fz.Step(Input.VectorAccel.z);
VectorAccelPure = Input.VectorAccel;

//VectorAccelPure = Input.VectorAccel;

//VectorGyroPure.x = FilterGyroX.Step(Input.VectorGyro.x);
//VectorGyroPure.y = FilterGyroY.Step(Input.VectorGyro.y);
//VectorGyroPure.z = FilterGyroZ.Step(Input.VectorGyro.z);

VectorGyroPure = Input.VectorGyro;

VectorVelocity = VectorGyroPure;

//VectorAccelOffset = VectorAccelPure;
//VectorAccelOffset.x = 0.0;
//VectorAccelOffset.y = 0.0;

VectorAccel = VectorAccelPure;// - VectorAccelOffset;

//QuaternionGyro;



MATRIX4x4 matrix_to_global;
MATRIX4x4::rotation(matrix_to_global, QuaternionGyro);

VECTOR4 angular_speed_local;
MATRIX4x4::transform(angular_speed_local, VECTOR4(VectorGyroPure, 1.0), gyro_calib_matrix);

VECTOR4 acceleration_local;
MATRIX4x4::transform(acceleration_local, VECTOR4(VectorAccelPure, 1.0), accel_calib_matrix);

static VECTOR3 VectorASum(0.0, 0.0, 0.0);
static VECTOR3 VectorGSum(0.0, 0.0, 0.0);
static int n = 0;
//FilterWindow.Step(VectorAccelPure);
// без сторопа включаю рукой 93 см-000.txt
const VECTOR3 accel_offset;//(0.15, 0.022, 0.0);

if (Time < BeginTime)
  {
  VectorASum += VectorAccelPure;
  VectorGSum += VectorGyroPure;
  n++;

  VectorAccelOffset = VectorASum / n;
  VectorGyroOffset = VectorGSum / n;
  }
else if (Time < EndTime)
  {
  VECTOR4 angular_speed_global;
  MATRIX4x4::transform(angular_speed_global, angular_speed_local, matrix_to_global);
  VectorGyro = angular_speed_global.tovec3() - VectorGyroOffset;
  // Учёт внешних факторов
  //global_angular_velocity.x = angular_speed_global.x - global_coriolis_force.x;
  //global_angular_velocity.y = angular_speed_global.y - global_coriolis_force.y;
  //global_angular_velocity.z = angular_speed_global.z - global_coriolis_force.z;
  //CFC_VECTOR_Step(&gyro_filters, &angular_velocity, &angular_velocity);
  //----------------------------------------------------------------------------
  VECTOR4 acceleration_global;
  MATRIX4x4::transform(acceleration_global, acceleration_local, matrix_to_global);
  VectorAccel = acceleration_global.tovec3() - VectorAccelOffset;
  // Учёт внешних факторов
  //global_acceleration.x = acceleration_global.x - global_gravity_force.x;
  //global_acceleration.y = acceleration_global.y - global_gravity_force.y;
  //global_acceleration.z = acceleration_global.z - global_gravity_force.z;
  //CFC_VECTOR_Step(&accel_filters, &acceleration, &acceleration);
  //--------------------------------------------------------------
  QUATERNION prime_rotation = QUATERNION::microrots(VectorGyro * dT);
  QUATERNION result_rotation = QuaternionGyro.mul(prime_rotation);
  QuaternionGyro = result_rotation.unit();
  //--------------------------------------------------------------
  VECTOR3 velInc = VectorAccel * dT;
  VectorTranslation += (VectorVelocity + (velInc * 0.5)) * dT;
  VectorVelocity += velInc;
  }

WritePoint(Time);

VectorAccelOffsetLen = sqrt(VectorAccelOffset.len2());
VectorAccelLen = sqrt(VectorAccel.len2());
VectorVelocityLen = sqrt(VectorVelocity.len2());
VectorTranslationLen = sqrt(VectorTranslation.len2());

LineAccel->SavePoint(VectorAccel);
LineAccelOffset->SavePoint(VectorAccelOffset);
LineVelocity->SavePoint(VectorVelocity);
LineTranslation->SavePoint(VectorTranslation);

// GPS данные
//VectorECEFPos = Input->ECEFPos;
//VectorECEFVel = Input->ECEFVel;
//VectorECEFPos = Input->ECEFPos;
//VectorPolarVel = Input->PolarVel;

// Статистика
VectorG.x = GravityFilterX.Step(VectorPure.x);
VectorG.y = GravityFilterY.Step(VectorPure.y);
VectorG.z = GravityFilterZ.Step(VectorPure.z);

VectorA = VectorPure-VectorG;

if (Input->PPS)
  {
  if (accel_aver_number > 0 )
    {
    accel_average = accel_sum / accel_aver_number;

    bool valid_speeds = (lastpolarVel.x > 1.0) && (Input->PolarVel.x > 1.0);

    if (valid_speeds)
      {
      gnss_accel = Input->PolarVel - lastpolarVel;

      gnss_accel.y = abs(gnss_accel.y);

      //if ((abs(gnss_accel.x) > 1.0) && (sqrt(accel_average.len2()) > 1.0))

      vec_accel = (gnss_accel.x >= 0)? (vec_accel+accel_average): (vec_accel-accel_average);
      }

    lastpolarVel = Input->PolarVel;
    }

    accel_aver_number = 0;
  accel_sum = VECTOR3(0, 0, 0);
  }

accel_sum += VectorA;
accel_aver_number++;

//loat accel_am = sqrt(VectorA.len2());

//float gnss_m = gnss_accel;

//FFT_Push(FFT_x, VectorA.z);
//FFT_Push(FFT_y, VectorA.x);
//FFT_Push(FFT_z, VectorA.y);
//FFT_Push(FFT_w, gnss_m);

// Калибровка
switch (calibState)
  {
  case calibstate_StaticPrepare:
    calibStatus = calibstatus_Uncomplete;
    calibState = calibstate_StaticCalib;

    calibIndex = 0;
    calibVectorG.x = 0;
    calibVectorG.y = 0;
    calibVectorG.z = 0;
    calibVector = calibVectorG;
    return;

  case calibstate_StaticCalib:
    calibVectorG += VectorPure;
    calibIndex++;
    calibVector += ((calibVectorG / calibIndex) - VectorPure).sqr();

    if (calibIndex >= 60*MEMS_SAMLES_PER_SEC)
      {
      calibVectorG /= calibIndex;

      calibVector = (calibVector / calibIndex).sqrt();
      calibVector *= 3.0f;

      VectorG = calibVectorG;

      calibState = calibstate_DynamicPrepare;
      calibIndex = 0;
      }
    return;

  case calibstate_DynamicPrepare:
    VectorA = VectorPure-calibVectorG;

    calibVectorV += VectorA * dT;

    calibVectorV.x = noise_crop(calibVectorV.x, calibVector.x*dT);
    calibVectorV.y = noise_crop(calibVectorV.y, calibVector.y*dT);
    calibVectorV.z = noise_crop(calibVectorV.z, calibVector.z*dT);

    calibIndex++;

    if (calibVectorV.len2() > 0.1f)
      {
      calibVector = calibVectorV;
      calibState = calibstate_DynamicCalib;
      }
    else if (calibIndex > 30*MEMS_SAMLES_PER_SEC)
      {
      calibState = calibstate_Ready;
      calibStatus = calibstatus_Fail;
      }

    VectorV = calibVectorV;
    return;

  case calibstate_DynamicCalib:
    VectorA = VectorPure-calibVectorG;

    {
    float res = VectorA.dot(calibVector);

    if (res > 0.0f)
      {
      calibVector += VectorA;
      }
    else if (res < 0.0f)
      {
      calibVector -= VectorA;
      }
    }

    calibIndex++;

    if (calibVector.len2() > sqr_macro(0.4f*60*MEMS_SAMLES_PER_SEC))
      {
      vnz = calibVectorG.unit();
      vnx = -calibVector;

      vny = vnz.mul(vnx);

      vny = vny.unit();
      vnx = vny.mul(vnz);

      calibState = calibstate_Ready;
      calibStatus = calibstatus_Success;
      }
    else if (calibIndex > 60*MEMS_SAMLES_PER_SEC)
      {
      calibState = calibstate_Ready;
      calibStatus = calibstatus_Fail;
      }

    VectorR = calibVector;
    return;

  default:
    return;
  }

//VectorA = VectorPure-StaticG;

//VectorA.x = noise_crop(VectorA.x, -1);
//VectorA.y = noise_crop(VectorA.y, -1);
//VectorA.z = noise_crop(VectorA.z, -1);

//VectorV += VectorA*dT;

//VectorV *= 0.999f;

//VectorV.x = noise_crop(VectorV.x, 0.0022);
//VectorV.y = noise_crop(VectorV.y, 0.0022);
//VectorV.z = noise_crop(VectorV.z, 0.0022);

//VectorV.x *= 1.001;

//    VectorR += VectorV*dT;

    //if (calib_timer > 2500)
      //{
      //}
    //if (VectorA.len2() <= 1.0)
      //{
     //}
    //else
    //  if (forward.len2() == 0)
    //  {
    //  forward = VectorA;
    //  }
    //else
      //{
      //float result = VectorA.dot(forward);

      //if (result < 0.0)
        //{
        //backward += VectorA;
        //VectorR = backward;
        //}
      //else
        //{
        //forward += VectorA;
        //VectorV = forward;
        //}
      //}

// Модель

VectorPure = Input->VectorA;

VectorA_M = sqrt(VectorPure.len2());

FilterWindow.Step(VectorPure);

VectorG.x = GravityFilterX.Step(VectorPure.x);
VectorG.y = GravityFilterY.Step(VectorPure.y);
VectorG.z = GravityFilterZ.Step(VectorPure.z);

VectorG_M = sqrt(VectorG.len2());

VectorA = VectorG - VectorPure;

VectorA.x = noise_crop(VectorA.x, 0.1f);
VectorA.y = noise_crop(VectorA.y, 0.1f);
VectorA.z = noise_crop(VectorA.z, 0.1f);

// threshold check

VectorA_M = sqrt(VectorA.len2());

if (VectorA_M > LimitA_Low)
  InterlockedIncrement(&(CounterALow));

if (VectorA_M > LimitA_High)
  InterlockedIncrement(&(CounterAHigh));

// move check

switch (modelTickCounter)
  {
  case 1:
    if (fabs(VectorR.x) > max_x)
      max_x = VectorR.x;

    if (fabs(VectorR.y) > max_y)
      max_y = VectorR.y;

    if (fabs(VectorR.z) > max_z)
      max_z = VectorR.z;

    if (VectorR_M > max_m)
      max_m = VectorR_M;

    VectorV.x = noise_crop(VectorV.x, 0.022f);
    VectorV.y = noise_crop(VectorV.y, 0.022f);
    VectorV.z = noise_crop(VectorV.z, 0.022f);

    VectorR.x = noise_crop(VectorR.x, (0.022f)/2.0f);
    VectorR.y = noise_crop(VectorR.y, (0.022f)/2.0f);
    VectorR.z = noise_crop(VectorR.z, (0.022f)/2.0f);

    VectorR_M = sqrt(VectorR.len2());

    if (VectorR_M > LimitS_Move)
      {
   //   zzz = (-StaticG).unit();
   //   xxx = (-forward).unit();

   //   yyy = zzz.mul(xxx);
   //   xxx = yyy.mul(zzz);

      InterlockedIncrement(&(CounterMove));
      }
    break;

  default:
    break;
  }

VectorV += VectorA * dT;
VectorR += VectorV * dT;

VectorR_M = sqrt(VectorR.len2());

// slope check

switch (modelTickCounter)
  {
  case 3:
    break;
  case 4:
    break;
  case 5:
  ///VectorGSlope.x = CFCF_Filter_Step(&SliceFilterX, VectorG.x);
  //VectorGSlope.y = CFCF_Filter_Step(&SliceFilterY, VectorG.y);
  //VectorGSlope.z = CFCF_Filter_Step(&SliceFilterZ, VectorG.z);
  //AngleSlope = VectorG.angle(VectorGSlope);
  //if (AngleSlope > Limit_Pitch)
    //InterlockedIncrement(&(CounterPitch));
    break;
  default:
    break;
  }

// end of Proc

modelTickCounter = LOOP_INC(modelTickCounter, MEMS_SAMLES_PER_SEC);
}

void C_MODEL_FLOAT::StepLast(void)
{
}

//==================================================================================================
static CFCI_FILTER FiltersForFastG[3] = // три фильтра Баттенворта y[n] = A0*(x[n]+2*x[n-1]+x[n-2])+B1*y[n-1]+B2*y[n-2]
  {
  {1586, 33089895, -16319023, 0, 0, 0, 0, 0},
  {1586, 33089895, -16319023, 0, 0, 0, 0, 0},
  {1586, 33089895, -16319023, 0, 0, 0, 0, 0},
  };

static CFCI_FILTER FiltersForSlowG[3] =
  {
  {1586, 33089895, -16319023, 0, 0, 0, 0, 0},
  {1586, 33089895, -16319023, 0, 0, 0, 0, 0},
  {1586, 33089895, -16319023, 0, 0, 0, 0, 0},
  };

struct VECTOR3_S16
  {
  short x;
  short y;
  short z;
  };

struct VECTOR3_S32
  {
  long x;
  long y;
  long z;
  };

struct VECTOR3_S64
  {
  long long x;
  long long y;
  long long z;
  };


static VECTOR3_S16 VectorA;
static VECTOR3_S16 VectorFastG;
static VECTOR3_S16 VectorSlowG;
static VECTOR3_S32 VectorV;
static VECTOR3_S64 VectorR;
static long VectorR2;

const float ACCEL_SCALE = 1.0f/(32767.0f/(24.0f*9.81f));

static long ThresholdA2_High = (long)sqrr(5 / ACCEL_SCALE);
static long ThresholdA2_Low = (long)sqrr(1 / ACCEL_SCALE);

static long ThresholdR2_Move = (long)sqrr(0.45f / ACCEL_SCALE); // 0.45 м/с2 -> 1 метр проедем, масштаб времени учтён в расчёте
static unsigned char ThresholdIdx_Pitch = 3;

static unsigned short IntegratorCounter;
static unsigned short SloperCounter;

static unsigned char AccelDowncounters[4];

#define INTEGRATE_TIME (3*MEMS_SAMLES_PER_SEC)

void VtoI(const VECTOR3_S16 *v16, VECTOR3 *vd)
{
vd->x = v16->x * ACCEL_SCALE;
vd->y = v16->y * ACCEL_SCALE;
vd->z = v16->z * ACCEL_SCALE;
}


static long Vec3_S16_Mul(const VECTOR3_S16 *vA, const VECTOR3_S16 *vB)
{
return ((long)(vA->x)*(vB->x))+((long)(vA->y)*(vB->y))+((long)(vA->z)*(vB->z));
}*/

/*=============================================================================================================*/
/*void C_MODEL_INTEGER::accel_integrate(const VECTOR3_S16 *Vector)
{
if (IntegratorCounter == 0) // Анализируем результаты интегрирования
  {
  VECTOR3_S32 temp;
  temp.x = Output->VectorR.x / (MEMS_SAMLES_PER_SEC*MEMS_SAMLES_PER_SEC);
  temp.y = Output->VectorR.y / (MEMS_SAMLES_PER_SEC*MEMS_SAMLES_PER_SEC);
  temp.z = Output->VectorR.z / (MEMS_SAMLES_PER_SEC*MEMS_SAMLES_PER_SEC);

  VectorR2 = (temp.x*temp.x)+(temp.y*temp.y)+(temp.z*temp.z);

  if (VectorR2 > ThresholdR2_Move)
    InterlockedIncrement(&(CounterMove));

  VectorV.x = Vector->x;
  VectorV.y = Vector->y;
  VectorV.z = Vector->z;

  VectorR.x = VectorV.x;
  VectorR.y = VectorV.y;
  VectorR.z = VectorV.z;

//  LineVLength = 0;
 // LineSLength = 0;

  IntegratorCounter = INTEGRATE_TIME;
  } else
  {
  VectorV.x += (long)(Vector->x); // интегрируем - скорость
  VectorV.y += (long)(Vector->y);
  VectorV.z += (long)(Vector->z);

  VectorR.x += (long long)(VectorV.x); // интегрируем - перемещение
  VectorR.y += (long long)(VectorV.y);
  VectorR.z += (long long)(VectorV.z);

  IntegratorCounter--;
  }

  VectorV.x = VectorV.x * (ACCEL_SCALE * dT);
  VectorV.y = VectorV.y * (ACCEL_SCALE * dT);
  VectorV.z = VectorV.z * (ACCEL_SCALE * dT);

  LineV->SavePoint(VectorV);

  VectorR.x = VectorR.x * (ACCEL_SCALE * dT * dT);
  VectorR.y = VectorR.y * (ACCEL_SCALE * dT * dT);
  VectorR.z = VectorR.z * (ACCEL_SCALE * dT * dT);

  //SavePoint(LineS, &(LineSLength), &(VectorR));

  VectorR_M = sqrt(VectorR.len2());
}*/
/*=============================================================================================================*/
/*static void accel_threshold_check(const VECTOR3_S16 *Vector, C_MODEL *Output)
{
long VectorA2 = Vec3_S16_Mul(&VectorA, &VectorA); // Квадрат модуля вектора ускорения

if (VectorA2 > ThresholdA2_High)
  InterlockedIncrement(&(CounterAHigh));

if (VectorA2 > ThresholdA2_Low)
  InterlockedIncrement(&(CounterALow));

}
// Таблица квадратов косинусов ((COS(i*0.25 degs))^2) << 15
#define COS_TABLE_SIZE  (81)
#define COS_TABLE_SHIFT (15)

static const unsigned short TableCos2[COS_TABLE_SIZE] = {
  32768, 32767, 32765, 32762, 32758, 32752, 32745, 32737, 32728, 32717, 32705, 32692, 32678, 32662,
  32645, 32627, 32608, 32588, 32566, 32543, 32519, 32493, 32466, 32439, 32409, 32379, 32348, 32315,
  32281, 32246, 32209, 32172, 32133, 32093, 32052, 32009, 31966, 31921, 31875, 31828, 31779, 31730,
  31679, 31627, 31574, 31520, 31465, 31409, 31351, 31292, 31232, 31171, 31109, 31046, 30982, 30916,
  30850, 30782, 30713, 30643, 30572, 30500, 30427, 30353, 30278, 30202, 30124, 30046, 29966, 29886,
  29804, 29722, 29638, 29554, 29468, 29382, 29294, 29206, 29116, 29026, 28934 };

static void accel_slope_check(const VECTOR3_S16 *Vector, C_MODEL *Output)
{
if (SloperCounter == 0)
  {
  VectorSlowG.x = CFCI_Filter_Step(&(FiltersForSlowG[0]), Vector->x); // отфильтровываем постоянную составляющую
  VectorSlowG.y = CFCI_Filter_Step(&(FiltersForSlowG[1]), Vector->y);
  VectorSlowG.z = CFCI_Filter_Step(&(FiltersForSlowG[2]), Vector->z);

  VtoI(&VectorSlowG, &(VectorGSlope));

  long Mod1 = Vec3_S16_Mul(&VectorSlowG, &VectorSlowG); //!!!!!!!!!!!!!
  long Mod2 = Vec3_S16_Mul(Vector, Vector);

  long long Denom = ((long long)Mod1*Mod2) >> COS_TABLE_SHIFT;

  unsigned int idx = 0;

  if (Denom)
    {
    long AlgMul = Vec3_S16_Mul(&VectorSlowG, Vector);

    long long Nom = ((long long)AlgMul*AlgMul);

    long Result = Nom / Denom;

    for (; idx < COS_TABLE_SIZE; idx++)
      if (Result >= TableCos2[idx])
        break;
    }

  AngleSlope = idx * 0.25f;

  if (idx > ThresholdIdx_Pitch)
    InterlockedIncrement(&(CounterPitch));

  SloperCounter = MEMS_SAMLES_PER_SEC;
  }

SloperCounter--;
}

#define WINDOW_SIZE (5)
static unsigned short WindowIdx;
static VECTOR3_S16 Window[WINDOW_SIZE];

static void WindowFilter(VECTOR3_S16 *Vector)
{
Window[WindowIdx] = *Vector;
WindowIdx = LOOP_INC(WindowIdx, WINDOW_SIZE);

VECTOR3_S32 Average;
Average.x = Window[0].x;
Average.y = Window[0].y;
Average.z = Window[0].z;

for (int idx = 1; idx < WINDOW_SIZE; idx++)
  {
  Average.x += Window[idx].x;
  Average.y += Window[idx].y;
  Average.z += Window[idx].z;
  }

Vector->x = Average.x / WINDOW_SIZE;
Vector->y = Average.y / WINDOW_SIZE;
Vector->z = Average.z / WINDOW_SIZE;
}
#undef WINDOW_SIZE

void C_MODEL_INTEGER::StepFirst(const MODEL_INPUT &Input)
{
CFCI_Filter_Reset(&(FiltersForFastG[0]), points[0].VectorA.x / ACCEL_SCALE);
CFCI_Filter_Reset(&(FiltersForFastG[1]), points[0].VectorA.y / ACCEL_SCALE);
CFCI_Filter_Reset(&(FiltersForFastG[2]), points[0].VectorA.z / ACCEL_SCALE);

CFCI_Filter_Reset(&(FiltersForSlowG[0]), points[0].VectorA.x / ACCEL_SCALE);
CFCI_Filter_Reset(&(FiltersForSlowG[1]), points[0].VectorA.y / ACCEL_SCALE);
CFCI_Filter_Reset(&(FiltersForSlowG[2]), points[0].VectorA.z / ACCEL_SCALE);
}

void C_MODEL_INTEGER::StepNext(const MODEL_INPUT &Input)
{
VectorECEFZ = Input->ECEFPos.unit();

VectorECEFVel = Input->ECEFVel;

VectorECEFX = Input->ECEFVel.unit();

VectorECEFY = VectorECEFZ.mul(VectorECEFX);

VECTOR3_S16 v;
v.x = Input->VectorA.x / ACCEL_SCALE;
v.y = Input->VectorA.y / ACCEL_SCALE;
v.z = Input->VectorA.z / ACCEL_SCALE;

WindowFilter(&v); // Если получили точку - фильтруем

VtoI(&v, &(VectorPure));

  VectorFastG.x = CFCI_Filter_Step(&(FiltersForFastG[0]), v.x); // отфильтровываем постоянную составляющую
  VectorFastG.y = CFCI_Filter_Step(&(FiltersForFastG[1]), v.y);
  VectorFastG.z = CFCI_Filter_Step(&(FiltersForFastG[2]), v.z);

  VtoI(&VectorFastG, &(VectorG));
  VectorG_M = sqrt(VectorG.len2());

  VectorA.x = VectorFastG.x - v.x; // убираем из показаний акселерометра постоянную составляющую
  VectorA.y = VectorFastG.y - v.y;
  VectorA.z = VectorFastG.z - v.z;

  LineA->SavePoint(VectorECEFVel);

  VtoI(&VectorA, &(VectorA));

  VectorA_M = sqrt(VectorA.len2());

//  if (device_is_started()) // датчик начинает работу после старта устройства
    {
    accel_threshold_check(&VectorA, Output); // Виртуальный датчик удара
    accel_integrate(&VectorA, Output);       // Виртуальный датчик движения
    accel_slope_check(&VectorFastG, Output); // Виртуальный датчик наклона
    }
}

void C_MODEL_INTEGER::StepLast(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////


Output->VectorG_M = sqrt(Output->VectorG.len2());

if (Output->VectorA.len2() > 1.0)
  {
  //SavePoint((Output->LineA), &(Output->LineALength), &(Output->VectorA));
  }

float Acc = -(Output->VectorA.y);

if (Acc > 0.0)
  {
  if (Acc > boost)
    boost = Acc;
  } else
  {
  Acc = -Acc;
  if (Acc > retard)
    retard = Acc;
  }

Acc = Output->VectorA.x;

if (Acc > 0.0)
  {
  if (Acc > driftr)
    driftr = Acc;
  } else
  {
  Acc = -Acc;
  if (Acc > driftl)
    driftl = Acc;
  }

if (Input->PPS)
  {
  Output->BoostAccel = boost;
  Output->RetardAccel = retard;

  boost = 0;
  retard = 0;

  Output->DriftAccel = max(driftl, driftr);

  driftl = 0;
  driftr = 0;

  float acc = velECEFNew-velECEFOld;

  if (acc > 0.0)
    {
    Output->BoostECEF = acc;
    Output->RetardECEF = 0;
    } else
    {
    Output->BoostECEF = 0;
    Output->RetardECEF = -acc;
    }

  velECEFOld = velECEFNew;
  velECEFNew = sqrt(Input->ECEFVel.len2());

 acc = velPolNew-velPolOld;

  if (acc > 0.0)
    {
    Output->BoostPolar = acc;
    Output->RetardPolar = 0;
    } else
    {
    Output->BoostPolar = 0;
    Output->RetardPolar = -acc;
    }

  velPolOld = velPolNew;
  velPolNew = Input->PolarVel.x;

 float drift = D3DXToDegree(courseNew-courseOld);

 Output->DriftPolar = drift;

 courseOld = courseNew;
 courseNew = Input->PolarVel.y;*/

  //Output->VectorECEFAcc.x = Input->ECEFVel.x - Oldvel.x;

  //Oldvel = Input->ECEFVel;

/*if (Input->PPS)
  {

  Output->VectorECEFVel.y = Input->ECEFVel.y - Oldvel.y;
  Output->VectorECEFVel.z = Input->ECEFVel.z - Oldvel.z;


  }*/


/*  }






//if (IntegCounter == 0)
//  {
//  IntegCounter = 300.0 / dT;

  if (Output->VectorR_M > LimitS_Move)
    InterlockedIncrement(&(Output->CounterMove));

  // Для графики
//  Output->LineVLength = 0;
//  Output->LineSLength = 0;

  // Сбрасываем результат интегрирования
//  ZeroMemory(&(Output->VectorR), sizeof(VECTOR3));
//  ZeroMemory(&(Output->VectorV), sizeof(VECTOR3));
//  } else
//  {
//  IntegCounter--;
//  }

Output->VectorV.x += Output->VectorA.x * dT;
Output->VectorV.y += Output->VectorA.y * dT;
Output->VectorV.z += Output->VectorA.z * dT;

Output->VectorV.x = noise_crop(Output->VectorV.x, 0.4*dT);
Output->VectorV.y = noise_crop(Output->VectorV.y, 0.4*dT);
Output->VectorV.z = noise_crop(Output->VectorV.z, 0.4*dT);

Output->VectorR.x += Output->VectorV.x * dT;
Output->VectorR.y += Output->VectorV.y * dT;
Output->VectorR.z += Output->VectorV.z * dT;

Output->VectorR.x = noise_crop(Output->VectorR.x, 0.4*dT*dT);
Output->VectorR.y = noise_crop(Output->VectorR.y, 0.4*dT*dT);
Output->VectorR.z = noise_crop(Output->VectorR.z, 0.4*dT*dT);

Output->VectorR_M = sqrt(Output->VectorR.len2());

SavePoint(Output->LineV, &(Output->LineVLength), &(Output->VectorV));
SavePoint(Output->LineS, &(Output->LineSLength), &(Output->VectorR));

if (PitchCounter == 0)
  {
  Output->VectorGSlope.x = CFCF_Filter_Step(&SliceFilterX, Output->VectorG.x);
  Output->VectorGSlope.y = CFCF_Filter_Step(&SliceFilterY, Output->VectorG.y);
  Output->VectorGSlope.z = CFCF_Filter_Step(&SliceFilterZ, Output->VectorG.z);

  Output->AngleSlope = Output->VectorG.angle(Output->VectorGSlope);

  if (Output->AngleSlope > Limit_Pitch)
    InterlockedIncrement(&(Output->CounterPitch));

  PitchCounter = 1.0 / dT;
  } else
  {
  PitchCounter--;
  }
*/
