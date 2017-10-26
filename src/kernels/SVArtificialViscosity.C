#include "SVArtificialViscosity.h"

// MOOSE includes
#include "MooseMesh.h"

template <>
InputParameters
validParams<SVArtificialViscosity>()
{
  InputParameters params = validParams<Kernel>();
  params.addClassDescription("Computes the artificial viscosity term to enforce"
                             " stability in the Saint-Venant equations.");
  return params;
}

SVArtificialViscosity::SVArtificialViscosity(const InputParameters & parameters)
  : Kernel(parameters),
    _kappa(getMaterialProperty<Real>("kappa"))
{
}

Real
SVArtificialViscosity::computeQpResidual()
{
  // Cell is on the boundary
  if (_mesh.isBoundaryNode(_current_elem->node(_i)))
    return 0;
  // Is not on the boundary
  else
    return _kappa[_qp] * _grad_u[_qp] * _grad_test[_i][_qp];
}

Real
SVArtificialViscosity::computeQpJacobian()
{
  // Approximate by the parabolic regularization
  return _kappa[_qp] * _grad_phi[_j][_qp] * _grad_test[_i][_qp];
}
