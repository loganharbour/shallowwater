#ifndef SVPRESSUREAUX_H
#define SVPRESSUREAUX_H

#include "AuxKernel.h"

// Forward Declarations
class SVPressureAux;

template <>
InputParameters validParams<SVPressureAux>();

class SVPressureAux : public AuxKernel
{
public:
  SVPressureAux(const InputParameters & parameters);

protected:
  virtual Real computeValue() override;

  /// Coupled water height variable
  const VariableValue & _h;

  /// Constant of gravity
  const Real _g;
};

#endif
