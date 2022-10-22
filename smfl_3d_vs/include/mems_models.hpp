#ifndef MEMS_MODELS_HPP
#define MEMS_MODELS_HPP


class memsModels
{
  public:
    memsModels();
    virtual ~memsModels();

  protected:

  private:
};

/*
#ifndef __MODELS_H
#define __MODELS_H

#pragma once

#include "Common.h"

struct MODEL_INPUT
  {
  double Time;
  VECTOR3 VectorAccel;
  VECTOR3 VectorGyro;
  //VECTOR3 PositionECEF;
  //VECTOR3 VelocityECEF;
  //VECTOR3 PositionBLH;
  //VECTOR2 VelocityBLH;
  //bool PPS;
  };

class CSTAT
  {
  private:
    float n;
    VECTOR3 Sum;
    VECTOR3 SqSum;

  public:
    VECTOR3 Aver;

    CSTAT()
      {
      memset(this, 0, sizeof(CSTAT));
      }

    void Eval(const VECTOR3 &vec)
      {
      Sum += vec;
      n++;

      VECTOR3  Med = Sum / n;

      VECTOR3 delta = Med - vec;
      SqSum += delta.sqr();
      }

    void Commit(void)
      {
      Aver = Sum / n;
      //SQ = (SqSum / n).sqrt();

      Sum = VECTOR3(0, 0, 0);
      SqSum = VECTOR3(0, 0, 0);
      n = 0;
      }
  };

class C_MODEL
  {
  public:
    double Time;
    VECTOR3 VectorAccelPure;
    VECTOR3 VectorAccelOffset;
    VECTOR3 VectorAccel;
    double VectorAccelLen;
    double VectorAccelOffsetLen;

    VECTOR3 VectorGyroPure;
    VECTOR3 VectorGyroOffset;
    VECTOR3 VectorGyro;
    double VectorGyroLen;

    VECTOR3 VectorVelocity;
    double VectorVelocityLen;

    VECTOR3 VectorTranslation;
    double VectorTranslationLen;

    QUATERNION QuaternionGyro;

    GRAPHIC_LINE *LineAccel;
    GRAPHIC_LINE *LineAccelOffset;
    GRAPHIC_LINE *LineVelocity;
    GRAPHIC_LINE *LineTranslation;

    //VECTOR3 VectorGSlope;
    //float AngleSlope;
    //VECTOR3 VectorECEFPos;
    //VECTOR3 VectorECEFVel;
    //VECTOR2 VectorPolarVel;
    //VECTOR3 VectorECEFX;
    //VECTOR3 VectorECEFY;
    //VECTOR3 VectorECEFZ;


    float BoostAccel;
    float BoostPolar;
    float BoostECEF;

    float RetardAccel;
    float RetardPolar;
    float RetardECEF;

    float DriftAccel;
    float DriftPolar;

    volatile long CounterALow;
    volatile long CounterAHigh;
    volatile long CounterMove;
    volatile long CounterPitch;

    VECTOR3 vnx;
    VECTOR3 vny;
    VECTOR3 vnz;

    char *Title;
    double BeginTime;
    double EndTime;

    float FFT_x[FFT_N];
    float FFT_y[FFT_N];
    float FFT_z[FFT_N];
    float FFT_w[FFT_N];

    //CSTAT Stat;
    //VECTOR3 vec_accel;
    //VECTOR3 vec_brake;

    void Init(char *Title);
    void WritePoint(double PointTime);
    void Draw();
  };

class C_MODEL_INTEGER: public C_MODEL
  {
  public:
    void StepFirst(MODEL_INPUT const &Input);
    void StepNext(const MODEL_INPUT &Input);
    void StepLast(void);
  };

class C_MODEL_FLOAT: public C_MODEL
  {
  public:
    void StepFirst(MODEL_INPUT const &Input);
    void StepNext(const MODEL_INPUT &Input);
    void StepLast(void);
  };

class C_INPUT_POINTS
  {
  public:
    C_INPUT_POINTS();
    ~C_INPUT_POINTS();

    void AllocPoints(unsigned int PointsCount);
    bool AddPoint(MODEL_INPUT const &Point);

    void SetKeyTimes(double IdleTime, double IntegrateTime);

    unsigned int GetPointsNumber()
      {
      return (Number);
      }

    MODEL_INPUT const & operator [] (unsigned int Index) const
      {
      if (Index >= Number)
        {
        throw std::exception();//"Out of input points array!");
        }

      return (Points[Index]);
      }

  private:
    void CleanUp();

    static MODEL_INPUT *Points;
    static unsigned int Number;
    static unsigned int Capacity;
    static double BeginTime;
    static double EndTime;
  };

extern C_INPUT_POINTS InputPoints;
extern C_MODEL_FLOAT FloatModel;
extern C_MODEL_INTEGER IntegerModel;

#endif
*/


#endif // MEMS_MODELS_HPP
