#ifndef SVADVECTION_H
#define SVADVECTION_H

#include "Kernel.h"

// Forward Declarations
class SVAdvection;
class Function;

template <>
InputParameters validParams<SVAdvection>();

class SVAdvection : public Kernel
{
public:
  SVAdvection(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;

  /// Coupled water height variable
  const VariableValue & _h;

  /// Other momentum component variable (used in 2D)
  const VariableValue & _qo;

  /// Component indices
  const unsigned int _comp_u;
  const unsigned int _comp_qo;

  /// Whether or not to use the other momentum component
  bool _use_qo;
};

#endif
