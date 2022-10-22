#define _USE_MATH_DEFINES
#include <math.h>
#include "cfc_filter.hpp"
/*===============================================================================================*/
#define CFCI_FILTER_SHIFT (24) // сдвиг знаменателя коэффициентов
#define SAT_S16(x) (x)
#define SAT_S16(x) (x)
/*===============================================================================================*/
void CFCI_Filter_Reset(CFCI_FILTER *Filter, const short Value)
{
Filter->x[0] = Value;
Filter->x[1] = Value;
Filter->y[0] = Value;
Filter->y[1] = Value;
}
/*===============================================================================================*/
short CFCI_Filter_Step(CFCI_FILTER *Filter, const short X)
{
long long Y = Filter->rem;

Y += ((long)(Filter->a))*((long)X+((long)((Filter->x)[0]) << 1)+(long)((Filter->x)[1]));

(Filter->x)[1] = (Filter->x)[0];
(Filter->x)[0] = X;

Y += ((long long)(Filter->b1))*((Filter->y)[0]);
Y += ((long long)(Filter->b2))*((Filter->y)[1]);

Filter->rem = static_cast<unsigned long>(Y & ((1<<CFCI_FILTER_SHIFT)-1));
Y >>= CFCI_FILTER_SHIFT;

(Filter->y)[1] = (Filter->y)[0];
(Filter->y)[0] = static_cast<short>(SAT_S16(Y));

return ((Filter->y)[0]);
}
//==================================================================================================
void CFCI_Filter_Init(CFCI_FILTER *Filter, const double CFC, const double T, const short Value)
{
CFCF_FILTER filterF;

filterF.Init(CFC, T, 0.0);

Filter->a = (short)((filterF.a0)*(1<<CFCI_FILTER_SHIFT));
Filter->b1 = (long)((filterF.b1)*(1<<CFCI_FILTER_SHIFT));
Filter->b2 = (long)((filterF.b2)*(1<<CFCI_FILTER_SHIFT));

CFCI_Filter_Reset(Filter, Value);
}

//--------------------------------------------------------------------------------------------------
void CFCF_FILTER::Init(const double CFC, const double T, const double Value)
{
Reset(Value);

double w_d = 2.0 * M_PI * CFC * 2.0775;
double w_a = tan( w_d * T * 0.5);

double denom = 1.0 + M_SQRT2 * w_a + w_a * w_a;

a0 = (w_a * w_a) / denom;
a1 = 2.0 * a0;

b1 = (-2.0 * (w_a * w_a - 1.0)) / denom;
b2 = (-1.0 + M_SQRT2 * w_a - w_a * w_a) / denom;
}

//--------------------------------------------------------------------------------------------------
double CFCF_FILTER::Step(const double X)
{
double Y = a0*X+a1*x_old1+a0*x_old2+b1*y_old1+b2*y_old2;

x_old2 = x_old1;
x_old1 = X;

y_old2 = y_old1;
y_old1 = Y;

return (Y);
}

//--------------------------------------------------------------------------------------------------
double noise_crop(double value, double level)
{
if (value > level)
  {
  return (value-level);
  } 
else if (value < -level)
  {
  return (value+level);
  }
else
  {
  return (0.0);
  }
}