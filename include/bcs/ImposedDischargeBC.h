#ifndef IMPOSEDDISCHARGEBC_H
#define IMPOSEDDISCHARGEBC_H

#include "IntegratedBC.h"

// Forward Declarations
class ImposedDischargeBC;

template <>
InputParameters validParams<ImposedDischargeBC>();

class ImposedDischargeBC : public IntegratedBC
{
public:
  ImposedDischargeBC(const InputParameters & parameters);

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
  const Real _h_imposed;
  const Real _q_imposed;

  /// Gravity constant
  const Real _g;

  /// Parameters for Newton iteration
  const unsigned int _newton_max;
  const Real _newton_abs_tol;
};

#endif // IMPOSEDDISCHARGEBC_H
