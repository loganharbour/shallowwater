#ifndef SVENTROPYFLUXAUX_H
#define SVENTROPYFLUXAUX_H

#include "AuxKernel.h"

// Forward Declarations
class SVEntropyFluxAux;

template <>
InputParameters validParams<SVEntropyFluxAux>();

class SVEntropyFluxAux : public AuxKernel
{
public:
  SVEntropyFluxAux(const InputParameters & parameters);

protected:
  virtual Real computeValue() override;

  /// Coupled water height variable
  const VariableValue & _h;

  /// Coupled momentum variables
  const VariableValue & _q_x;
  const VariableValue & _q_y;

  /// Coupled bathymetry variable
  const VariableValue & _b;

  /// Component of the momentum flux to evaluate
  const unsigned int _comp;

  /// Momentum component
  const VariableValue & _momentum_comp;

  /// Constant of gravity
  const Real _g;
};

#endif
