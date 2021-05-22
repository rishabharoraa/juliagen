#include<stdio.h>

#define SIDE 1024
#define ITER 100

/* for x cells to be mapped to [-2,2], partitions must be 4 / x */
float step = (4 / (float) SIDE );

void plot(float, float);
int iterate(float, float, float, float);
void color(int);

/* creating the resultant image file */
FILE *res = fopen("result.ppm", "w");

void
plot(float zi, float zr)
{
  int iterations;
  for(float i = -2.0f; i < 2.0f; i+=step) {
    for(float r = -2.0f; r < 2.0f; r+=step) {
      color(iterate(i, r, zi, zr));
    }
  }
}

int
iterate(float ci, float cr, float zi, float zr)
{
  int n = 0;
  while((zr*zr)+(zi*zi) <= 4 && n < ITER) {
    /*
     *    z = z*z + c
     *    here, z*z = (a+bi)(a+bi) = a^2 - b^2 + 2abi = (a^2 - b^2) + (2ab)i
     *    => z*z + c = (a^2 - b^2 + cr) + (2ab + ci)i
     *    => hence,
     *    new real part = a*a - b*b + cr
     *    new imaginary part = 2*a*b + ci
     *    if abs(z) > 2 then it diverges
     */
    float tempi, tempr;
    tempr = (zr*zr) - (zi*zi) + cr;
    tempi = 2*zr*zi + ci;
    zr = tempr;
    zi = tempi;
    n++;
  }
  return n;
}

void
color(int iterations)
{
  /*
   *    for mapping a number from [a1,a2] to [b1,b2],
   *    new = b1 + ((old - a1)(b2 - b1)) / (a2 - a1)
   */
  int shade = 255 + (iterations*(-255) / ITER);
  fprintf(res, "%d %d %d\n", shade, shade, shade);
}

int
main()
{
  float zi = 0.0f;
  float zr = 0.0f;

  /* .ppm headers */
  fprintf(res, "%s\n", "P3");
  fprintf(res, "%d %d\n", SIDE, SIDE);
  fprintf(res, "%d\n", 255);

  plot(zi, zr);
  fclose(res);

  return 0;
}
