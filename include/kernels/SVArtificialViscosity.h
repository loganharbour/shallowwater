#ifndef SVARTIFICIALVISCOSITY_H
#define SVARTIFICIALVISCOSITY_H

#include "Kernel.h"

// Forward Declarations
class SVArtificialViscosity;

template <>
InputParameters validParams<SVArtificialViscosity>();

class SVArtificialViscosity : public Kernel
{
public:
  SVArtificialViscosity(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;

  /// Viscosity coefficient
  const MaterialProperty<Real> & _kappa;
};

#endif
