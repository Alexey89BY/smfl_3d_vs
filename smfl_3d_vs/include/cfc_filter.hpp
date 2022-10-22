#ifndef __CFC_H
#define __CFC_H

#pragma once

#include <string>

/*===============================================================================================*/
typedef struct _CFCI_FILTER
  {
  short a;
  long b1;
  long b2;
  unsigned long rem;
  short x[2];
  short y[2];
  } CFCI_FILTER;

void CFCI_Filter_Reset(CFCI_FILTER *Filter, const short Value);
short CFCI_Filter_Step(CFCI_FILTER *Filter, const short X);
void CFCI_Filter_Init(CFCI_FILTER *Filter, const double CFC, const double T, const short Value);

//--------------------------------------------------------------------------------------------------
class CFCF_FILTER
  {
  public:
    CFCF_FILTER()
      {
      memset(this, 0, sizeof(*this));
      }

    void Reset(const double Value)
      {
      x_old2 = Value;
      x_old1 = Value;
      y_old2 = Value;
      y_old1 = Value;
      }

    void Init(const double CFC, const double T, const double Value);

    double Step(const double X);

  public:
    double x_old1, x_old2;
    double y_old1, y_old2;
    double a0, a1;
    double b1, b2;
  };

//--------------------------------------------------------------------------------------------------
template<class Type, unsigned int Size> class ClassWindowFilter
  {
  public:
    ClassWindowFilter()
      {
      WindowIdx = Size-1;
      }

    void Reset(Type const & Point)
    {
    for (WindowIdx = 1; WindowIdx < Size; WindowIdx++)
      {
      Window[WindowIdx] = Point;
      }
    }

    void Step(Type& Point)
    {
    WindowIdx = (WindowIdx < (Size-1))? (WindowIdx+1): (0);
    Window[WindowIdx] = Point;

    Type Average = Window[0];
    for (unsigned int idx = 1; idx<Size; idx++)
      {
      Average += Window[idx];
      }

    Point = Average / Size;
    }

  private:
    unsigned int WindowIdx;
    Type Window[Size];
  };

//--------------------------------------------------------------------------------------------------
double noise_crop(double value, double level);

#endif // __CFC_H
