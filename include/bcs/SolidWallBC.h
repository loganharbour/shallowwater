#ifndef SOLIDWALLBC_H
#define SOLIDWALLBC_H

#include "IntegratedBC.h"

// Forward Declarations
class SolidWallBC;

template <>
InputParameters validParams<SolidWallBC>();

class SolidWallBC : public IntegratedBC
{
public:
  SolidWallBC(const InputParameters & parameters);
  virtual ~SolidWallBC(){}

protected:
  virtual Real computeQpResidual() override;

  /// Coupled height variable
  const VariableValue & _h;

  /// Equation and component identifiers
  const unsigned int _eq;
  const unsigned int _comp;

  /// Gravity constant
  const Real _g;
};

#endif // SOLIDWALLBC_H
