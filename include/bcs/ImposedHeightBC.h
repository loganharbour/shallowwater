#ifndef IMPOSEDHEIGHTBC_H
#define IMPOSEDHEIGHTBC_H

#include "IntegratedBC.h"

// Forward Declarations
class ImposedHeightBC;

template <>
InputParameters validParams<ImposedHeightBC>();

class ImposedHeightBC : public IntegratedBC
{
public:
  ImposedHeightBC(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual() override;

  /// Coupled momentum variables
  const VariableValue & _q_x;
  const VariableValue & _q_y;

  /// Coupled height variable
  const VariableValue & _h;

  /// Equation and component identifiers
  const unsigned int _eq;
  const unsigned int _comp;

  /// Imposed values
  const Real _h_imp;
  const Real _q_imp;

  /// Gravity constant
  const Real _g;
};

#endif // IMPOSEDHEIGHT_BC
