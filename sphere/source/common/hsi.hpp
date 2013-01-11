#ifndef HSI_HPP
#define HSI_HPP
void RGBtoHSI(const double r, const double g, const double b, double* h, double* s, double* i);
void HSItoRGB(const double h, const double saturation, const double intensity, double* r, double* g, double* b);
#endif
