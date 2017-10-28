#ifndef SVVELOCITYAUX_H
#define SVVELOCITYAUX_H

#include "AuxKernel.h"

// Forward Declarations
class SVVelocityAux;

template <>
InputParameters validParams<SVVelocityAux>();

class SVVelocityAux : public AuxKernel
{
public:
  SVVelocityAux(const InputParameters & parameters);

protected:
  virtual Real computeValue() override;

  /// Coupled water height variable
  const VariableValue & _h;

  /// Coupled momentum variables
  const VariableValue & _q_x;
  const VariableValue & _q_y;

  /// Mesh dimension
  const unsigned int _mesh_dimension;
};

#endif
