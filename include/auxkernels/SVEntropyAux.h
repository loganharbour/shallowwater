#ifndef SVENTROPYAUX_H
#define SVENTROPYAUX_H

#include "AuxKernel.h"

// Forward Declarations
class SVEntropyAux;

template <>
InputParameters validParams<SVEntropyAux>();

class SVEntropyAux : public AuxKernel
{
public:
  SVEntropyAux(const InputParameters & parameters);

protected:
  virtual Real computeValue() override;

  /// Coupled water height variable
  const VariableValue & _h;

  /// Coupled momentum variables
  const VariableValue & _q_x;
  const VariableValue & _q_y;

  /// Constant of gravity
  const Real _g;
};

#endif
