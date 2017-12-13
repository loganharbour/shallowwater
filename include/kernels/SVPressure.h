#ifndef SVPRESSURE_H
#define SVPRESSURE_H

#include "Kernel.h"

// Forward Declarations
class SVPressure;

template <>
InputParameters validParams<SVPressure>();

class SVPressure : public Kernel
{
public:
  SVPressure(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpOffDiagJacobian(unsigned int jvar) override;

  /// Coupled water height variable
  const VariableValue & _h;

  /// h equation index
  const unsigned int _h_ivar;

  /// Component to evaluate
  const unsigned int _comp;

  /// Constant of gravity
  const Real _g;
};

#endif
