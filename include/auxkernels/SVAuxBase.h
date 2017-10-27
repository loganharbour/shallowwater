#ifndef SVAUXBASE_H
#define SVAUXBASE_H

#include "AuxKernel.h"

// Forward Declarations
class SVAuxBase;

template <>
InputParameters validParams<SVAuxBase>();

class SVAuxBase : public AuxKernel
{
public:
  SVAuxBase(const InputParameters & parameters);

protected:
  /// Coupled water height variable
  const VariableValue & _h;

  /// Coupled momentum variables
  const VariableValue & _q_x;
  const VariableValue & _q_y;

  /// Constant of gravity
  const Real _g;
};

#endif
