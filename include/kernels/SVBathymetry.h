#ifndef SVBATHYMETRY_H
#define SVBATHYMETRY_H

#include "Kernel.h"

// Forward Declarations
class SVBathymetry;

template <>
InputParameters validParams<SVBathymetry>();

class SVBathymetry : public Kernel
{
public:
  SVBathymetry(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpOffDiagJacobian(unsigned int jvar) override;

  /// Coupled water height variable
  const VariableValue & _h;

  /// h equation index
  const unsigned int _h_ivar;

  /// Coupled bathymetry variable
  const VariableGradient & _grad_b;

  /// Component of b to evaluate
  const unsigned int _comp;

  /// Constant of gravity
  const Real _g;
};

#endif
