#include "SVVelocityAux.h"

// MOOSE includes
#include "MooseMesh.h"

template<>
InputParameters
validParams<SVVelocityAux>()
{
  InputParameters params = validParams<AuxKernel>();

  params.addRequiredCoupledVar("h", "The water height variable.");
  params.addRequiredCoupledVar("q_x", "The variable that expresses the x-component"
                               " of the momentum.");
  params.addCoupledVar("q_y", "The variable that expresses the y-component of "
                       "the momentum (required only in 2D).");

  return params;
}

SVVelocityAux::SVVelocityAux(const InputParameters & parameters)
  : AuxKernel(parameters),
    _h(coupledValue("h")),
    _q_x(coupledValue("q_x")),
    _q_y(coupledValue("q_y")),
    _mesh_dimension(_mesh.dimension())
{
  // y-component of momentum is required but not given
  if (_mesh_dimension == 2 && !isCoupled("q_y"))
    mooseError("SVVelocityAux requires the y-component of momentum, q_y in 2D");
  // y-component of momentum is given but is not required

  if (_mesh_dimension == 1 && isCoupled("q_y"))
    mooseError("SVVelocityAux does not require the y-component of momentum, q_y"
               " in 1D but it was provided");
}

Real SVVelocityAux::computeValue()
{
  if (_mesh_dimension == 1)
    return _q_x[_qp] / _h[_qp];
  else
    return std::sqrt(_q_x[_qp] * _q_x[_qp] + _q_y[_qp] * _q_y[_qp]) / _h[_qp];
}
