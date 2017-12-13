#ifndef SVADVECTION_H
#define SVADVECTION_H

#include "Kernel.h"

// Forward Declarations
class SVAdvection;

template <>
InputParameters validParams<SVAdvection>();

class SVAdvection : public Kernel
{
public:
  SVAdvection(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;
  virtual Real computeQpOffDiagJacobian(unsigned int jvar) override;

  /// Coupled water height variable
  const VariableValue & _h;

  /// Coupled momentum variables
  const VariableValue & _q_x;
  const VariableValue & _q_y;

  /// Equation indices
  const unsigned int _h_ivar;
  const unsigned int _q_x_ivar;
  const unsigned int _q_y_ivar;

  /// Component of momentum to evaluate
  const unsigned int _comp;
};

#endif
