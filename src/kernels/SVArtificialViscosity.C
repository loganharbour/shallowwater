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
  // Only add if node is not on the boundary
  if (!_mesh.isBoundaryNode(_current_elem->node(_i)))
    return _kappa[_qp] * _grad_u[_qp] * _grad_test[_i][_qp];
  // Node is not on the boundary
  else
    return 0;
}

Real
SVArtificialViscosity::computeQpJacobian()
{
  // Only add if node is not on the boundary
  // Approximate by the parabolic regularization
  if (!_mesh.isBoundaryNode(_current_elem->node(_i)))
    return _kappa[_qp] * _grad_phi[_j][_qp] * _grad_test[_i][_qp];
  // Node is not on the boundary
  else
    return 0;
}
