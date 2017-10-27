#ifndef SVCONTINUITY_H
#define SVCONTINUITY_H

#include "Kernel.h"

// Forward Declarations
class SVContinuity;

template <>
InputParameters validParams<SVContinuity>();

class SVContinuity : public Kernel
{
public:
  SVContinuity(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpOffDiagJacobian(unsigned int jvar) override;

  /// Coupled momentum variables
  const VariableValue & _q_x;
  const VariableValue & _q_y;

  /// Equation indices
  const unsigned int _q_x_ivar;
  const unsigned int _q_y_ivar;
};

#endif
