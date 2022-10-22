#include "fft.hpp"

//--------------------------------------------------------------------------------------------------
FFT::FFT()
{
  //ctor
}

//--------------------------------------------------------------------------------------------------
FFT::~FFT()
{
  //dtor
}

//--------------------------------------------------------------------------------------------------
void FFT::push(double (&Data)[FFT_N], double NewData)
{
const unsigned int FFT_STEP = 1;
unsigned int idx;

for (idx = 0; idx < (FFT_N-FFT_STEP); idx++)
  {
  Data[idx] = Data[idx+FFT_STEP];
  }

for (; idx < (FFT_N); idx++)
  {
  Data[idx] = NewData;
  }
}
//--------------------------------------------------------------------------------------------------
void FFT::transform(double *Rdat, double *Idat, int Log2N, bool Inverse)
{
int N = (1<<Log2N);

int  i, j, n, k, io, ie, in, nn;
double     ru, iu, rtp, itp, rtq, itq, rw, iw, sr;

static const double Rcoef[14] =
  {  -1.0000000000000000,  0.0000000000000000,  0.7071067811865475,
      0.9238795325112867,  0.9807852804032304,  0.9951847266721969,
      0.9987954562051724,  0.9996988186962042,  0.9999247018391445,
      0.9999811752826011,  0.9999952938095761,  0.9999988234517018,
      0.9999997058628822,  0.9999999264657178
  };
static const double Icoef[14] =
  {   0.0000000000000000, -1.0000000000000000, -0.7071067811865474,
     -0.3826834323650897, -0.1950903220161282, -0.0980171403295606,
     -0.0490676743274180, -0.0245412285229122, -0.0122715382857199,
     -0.0061358846491544, -0.0030679567629659, -0.0015339801862847,
     -0.0007669903187427, -0.0003834951875714
  };

nn = N >> 1;
ie = N;

for(n=1; n<=Log2N; n++)
  {
  rw = Rcoef[Log2N - n];
  iw = Icoef[Log2N - n];

  if(Inverse)
    iw = -iw;

  in = ie >> 1;
  ru = 1.0;
  iu = 0.0;

  for(j=0; j<in; j++)
    {
    for(i=j; i<N; i+=ie)
      {
      io       = i + in;
      rtp      = Rdat[i]  + Rdat[io];
      itp      = Idat[i]  + Idat[io];
      rtq      = Rdat[i]  - Rdat[io];
      itq      = Idat[i]  - Idat[io];
      Rdat[io] = rtq * ru - itq * iu;
      Idat[io] = itq * ru + rtq * iu;
      Rdat[i]  = rtp;
      Idat[i]  = itp;
      }

    sr = ru;
    ru = ru * rw - iu * iw;
    iu = iu * rw + sr * iw;
    }

  ie >>= 1;
  }

for(j=i=1; i<N; i++)
  {
  if(i < j)
    {
    io       = i - 1;
    in       = j - 1;
    rtp      = Rdat[in];
    itp      = Idat[in];
    Rdat[in] = Rdat[io];
    Idat[in] = Idat[io];
    Rdat[io] = rtp;
    Idat[io] = itp;
    }

  k = nn;

  while(k < j)
    {
    j   = j - k;
    k >>= 1;
    }

  j = j + k;
  }

rw = 1.0f / N;

for(i=0; i<N; i++)
  {
  Rdat[i] *= rw;
  Idat[i] *= rw;
  }
}
