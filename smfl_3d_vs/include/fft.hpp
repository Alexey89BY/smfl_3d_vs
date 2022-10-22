#ifndef FFT_H
#define FFT_H


class FFT
{
  public:
    static const int FFT_N = 1024;

    FFT();
    virtual ~FFT();

    void push(double (&Data)[FFT_N], double NewData);
    void transform(double *Rdat, double *Idat, int Log2N, bool Inverse);

  protected:

  private:

};

#endif // FFT_H
