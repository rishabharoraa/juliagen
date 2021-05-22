#include<stdio.h>
#include<stdlib.h>
#include<string.h>

void plot(float, float);
int iterate(float, float, float, float);
void color(int);

/* creating the resultant image file */
FILE *res;
/* for x cells to be mapped to [-2,2], partitions must be 4 / x */
int side;
int iter;

void
plot(float zi, float zr)
{
  float step = (4 / (float)side);
  for(float i = -2.0f; i < 2.0f; i+=step) {
    for(float r = -2.0f; r < 2.0f; r+=step) {
      color(iterate(zi, zr, i, r));
    }
  }
}

int
iterate(float ci, float cr, float zi, float zr)
{
  int n = 0;
  while((zr*zr)+(zi*zi) <= 4 && n < iter) {
    /*    tempi and tempr are introduced so that the calculation
     *    below does not use the new value for the second expression.
     *    Not using them introduced a nasty bug that took hours to debug
     */
    float tempi, tempr;
    /*
     *    z = z*z + c
     *    here, z*z = (a+bi)(a+bi) = a^2 - b^2 + 2abi = (a^2 - b^2) + (2ab)i
     *    => z*z + c = (a^2 - b^2 + cr) + (2ab + ci)i
     *    => hence,
     *    new real part = a*a - b*b + cr
     *    new imaginary part = 2*a*b + ci
     *    if abs(z) > 2 then it diverges
     */
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
   *    here, from [0,iter] to [255,0]
   *    new = b1 + ((old - a1)(b2 - b1)) / (a2 - a1)
   */
  int shade = 255 + (iterations*(-255) / iter);
  fprintf(res, "%d %d %d\n", shade, shade, shade);
}

int
main(int argc, char *argv[])
{
  /* "sane" defaults */
  float zi = 0.01f;
  float zr = 0.285f;
  side = 1024;
  iter = 15;
  char* color = "grayscale";

  for(int i = 1; i < argc; i++) {
    if(strcmp(argv[i],"-c") == 0) {
      zr = atof(argv[i+1]);
      zi = atof(argv[i+2]);
      i+=2;
    }
    if(strcmp(argv[i],"-i") == 0) {
      iter = atoi(argv[i+1]);
      i++;
    }
  }

  printf("Contructing julia set with parameters: \n");
  printf("c = %f%+fi\n", zr, zi);
  printf("Dimensions: %dx%dpx\n", side, side);
  printf("Number of iterations: %d\n", iter);
  res = fopen("result.ppm", "w");

  /* .ppm headers */
  fprintf(res, "%s\n", "P3");
  fprintf(res, "%d %d\n", side, side);
  fprintf(res, "%d\n", 255);

  plot(zi, zr);
  fclose(res);
  printf("Julia set generated as res.ppm\n");

  return 0;
}
