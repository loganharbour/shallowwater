#ifndef SVBATHYMETRYSOURCE_H
#define SVBATHYMETRYSOURCE_H

#include "Kernel.h"

// Forward Declarations
class SVBathymetrySource;
class Function;

template <>
InputParameters validParams<SVBathymetrySource>();

class SVBathymetrySource : public Kernel
{
public:
  SVBathymetrySource(const InputParameters & parameters);

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
