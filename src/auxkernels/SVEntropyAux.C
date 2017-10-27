#include "SVEntropyAux.h"

template<>
InputParameters
validParams<SVEntropyAux>()
{
  InputParameters params = validParams<AuxKernel>();
  params.addClassDescription("Computes the entropy in the Saint-Venant equations.");

  params.addRequiredCoupledVar("h", "The water height variable.");
  params.addRequiredCoupledVar("q_x", "The variable that expresses the x-component"
                               " of the momentum.");
  params.addCoupledVar("q_y", "The variable that expresses the y-component of "
                       "the momentum (required only in 2D).");

  params.addParam<Real>("g", 9.80665, "Constant of gravity (m/s^2).");

  return params;
}

SVEntropyAux::SVEntropyAux(const InputParameters & parameters)
  : AuxKernel(parameters),
    _h(coupledValue("h")),
    _q_x(coupledValue("q_x")),
    _q_y(coupledValue("q_y")),
    _g(getParam<Real>("g"))
{
  // Sanity check on gravity
  if (_g < 0)
    mooseError("Gravity constant g is negative in SVEntropyAux");
}

Real SVEntropyAux::computeValue()
{
  // Momentum vector
  RealVectorValue vq(_q_x[_qp], _q_y[_qp], 0);

  // Velocity vector
  RealVectorValue v = vq / _h[_qp];

  // size_sq returns the square of the vector, hence u^2+v^2
  return 0.5 * _h[_qp] * (_g * _h[_qp] + v.size_sq());
}
