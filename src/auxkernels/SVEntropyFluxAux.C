#include "SVEntropyFluxAux.h"

// MOOSE includes
#include "MooseMesh.h"

template<>
InputParameters
validParams<SVEntropyFluxAux>()
{
  InputParameters params = validParams<AuxKernel>();

  params.addRequiredCoupledVar("h", "The water height variable.");
  params.addRequiredCoupledVar("q_x", "The variable that expresses the x-component"
                               " of the momentum.");
  params.addCoupledVar("q_y", "The variable that expresses the y-component of "
                       "the momentum (required only in 2D).");
  params.addCoupledVar("b", "The aux variable that represents the bathymetry"
                       " data (describes the topography of the bottom"
                       "terrain of the fluid body");

  params.addRequiredParam<unsigned int>("component", "The component of the entropy"
                                        " flux to evaluate (0,1)->(x,y).");
  params.addParam<Real>("g", 9.80665, "Constant of gravity (m/s^2).");

  return params;
}

SVEntropyFluxAux::SVEntropyFluxAux(const InputParameters & parameters)
  : AuxKernel(parameters),
    _h(coupledValue("h")),
    _q_x(coupledValue("q_x")),
    _q_y(isCoupled("_q_y") ? coupledValue("q_y") : _zero),
    _b(isCoupled("b") ? coupledValue("b") : _zero),
    _comp(getParam<unsigned int>("component")),
    _momentum_comp(_comp == 0 ? _q_x : _q_y),
    _g(getParam<Real>("g"))
{
  // Sanity check on gravity
  if (_g < 0)
    mooseError("Gravity constant g is negative in SVEntropyFluxAux");

  // y-component of momentum is required but not given
  if (_mesh.dimension() == 2 && !isCoupled("q_y"))
    mooseError("SVEntropyFluxAux requires the y-component of momentum, q_y in 2D");

  // y-component of momentum is given but is not required
  if (_mesh.dimension() == 1 && isCoupled("q_y"))
    mooseError("SVEntropyFluxAux does not require the y-component of momentum, q_y"
               " in 1D but it was provided");

  // Requesting component 1, but mesh is 1D
  if (_mesh.dimension() == 1 && _comp == 1)
    mooseError("SVEntropyFluxAux component is 1, but the mesh is 1D");
}

Real SVEntropyFluxAux::computeValue()
{
  // Velocity vector
  RealVectorValue v(_q_x[_qp] / _h[_qp], _q_y[_qp] / _h[_qp], 0);

  return _momentum_comp[_qp] * (_g * (_h[_qp] + _b[_qp]) + 0.5 * v.norm() * v.norm());
}
