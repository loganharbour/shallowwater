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
  virtual Real computeQpOffDiagJacobian(unsigned int jvar) override;

  /// Coupled water height variable
  const VariableValue & _h;

  /// Couple water height variable index
  const unsigned int _h_ivar;

  /// Bathymetry function
  Function & _b;

  /// Component of b to evaluate
  const unsigned int _comp;

  /// Constant of gravity
  const Real _g;
};

#endif
