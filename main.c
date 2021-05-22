/* author - Rishabh Arora (github.com/RishabhAroraa) */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

typedef enum {false, true} bool;

void plot(float, float, bool);
int iterateDiscrete(float, float, float, float);
float iterateSmooth(float, float, float, float);
void color(float);

/* creating the resultant image file */
FILE *res;
int side;
int iter;
bool smooth;
bool mandelbrot;

void
plot(float zi, float zr, bool smooth)
{
  /* for x cells to be mapped to [-2,2], partitions must be 4 / x */
  float step = (4 / (float)side);

  for(float i = -2.0f; i < 2.0f; i+=step) {
    for(float r = -2.0f; r < 2.0f; r+=step) {
      if(smooth)  color(iterateSmooth(zi, zr, i, r));
      else color(iterateDiscrete(zi, zr, i, r));
    }
  }
}

int
iterateDiscrete(float ci, float cr, float zi, float zr)
{
  int n = 0;
  /*    tempi and tempr are introduced so that the calculation
   *    below (in the while loop) does not use the new value for the
   *    second expression. Not using them introduced a nasty bug that
   *    took HOURS to debug.
   */
  float tempi, tempr;

  /*
   *    checks if -mandelbrot flag is set and if so,
   *    generates a mandelbrot set instead of julia set
  */
  if(mandelbrot) {
    tempr = cr;
    tempi = ci;
    cr = zr;
    ci = zi;
    zr = tempr;
    zi = tempi;
  }

  while((zr*zr)+(zi*zi) <= 4 && n < iter) {
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
  return n+1;
}

float
iterateSmooth(float ci, float cr, float zi, float zr)
{
  /*    pretty much the same function as interateDiscrete,
   *    but with a smooth coloring function in the return statement
   *    that returns a float in the range [0,iter) rather than an integer
   */
  float n = 0;
  float tempi, tempr;

  if(mandelbrot) {
    tempr = cr;
    tempi = ci;
    cr = zr;
    ci = zi;
    zr = tempr;
    zi = tempi;
  }

  while((zr*zr)+(zi*zi) <= 4 && n < iter) {
    tempr = (zr*zr) - (zi*zi) + cr;
    tempi = 2*zr*zi + ci;
    zr = tempr;
    zi = tempi;
    n++;
  }

  /*
   *    the smooth factor 1 - log(log2(abs(z))) converts the
   *    number of iterations returned into a smooth function,
   *    which the color() function uses as a parameter for coloring
   *    the julia set
   */
  return n + 1 - logf(log2f(sqrtf((zi*zi)+(zr*zr))));
}

void
color(float iterations)
{
  /*
   *    for mapping a number from [a1,a2] to [b1,b2],
   *    here, from [0,iter] to [255,0]
   *    new = b1 + ((old - a1)(b2 - b1)) / (a2 - a1)
   */
  float shade = (255 + iterations*(-255) / (float)(iter));
  fprintf(res, "%d %d %d\n", (int)shade, (int) shade, (int) shade);
}

int
main(int argc, char *argv[])
{
  /* "sane" defaults */
  float zr = -0.8f;
  float zi = 0.156f;
  side = 1024;
  iter = 100;
  smooth = false;
  mandelbrot = false;

  for(int i = 1; i < argc; i++) {
    if(strcmp(argv[i],"-c") == 0) {
      if(!(mandelbrot)) {
        zr = atof(argv[i+1]);
        zi = atof(argv[i+2]);
        i+=2;
      }
    }
    if(strcmp(argv[i],"-i") == 0) {
      iter = atoi(argv[i+1]);
      i++;
    }
    if(strcmp(argv[i],"-s") == 0) {
      smooth = true;
    }
    if(strcmp(argv[i],"-m") == 0) {
      mandelbrot = true;
      zr = 0.0f;
      zi = 0.0f;
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

  plot(zi, zr, smooth);
  fclose(res);
  printf("Julia set generated as res.ppm\n");

  return 0;
}
