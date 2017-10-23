#ifndef SWBATHYMETRYSOURCE_H
#define SWBATHYMETRYSOURCE_H

#include "Kernel.h"

// Forward Declarations
class SWBathymetrySource;
class Function;

template <>
InputParameters validParams<SWBathymetrySource>();

class SWBathymetrySource : public Kernel
{
public:
  SWBathymetrySource(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual() override;

  /// Coupled water height variable
  const VariableValue & _h;

  /// Bathymetry function
  Function & _b;

  /// Component of b to evaluate
  const unsigned int _component;

  /// Constant of gravity
  const Real _g;
};

#endif
