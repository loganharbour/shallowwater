#include "SVPressureAux.h"

template<>
InputParameters
validParams<SVPressureAux>()
{
  InputParameters params = validParams<AuxKernel>();

  params.addRequiredCoupledVar("h", "The water height variable.");
  params.addParam<Real>("g", 9.80665, "Constant of gravity (m/s^2).");

  return params;
}

SVPressureAux::SVPressureAux(const InputParameters & parameters)
  : AuxKernel(parameters),
    _h(coupledValue("h")),
    _g(getParam<Real>("g"))
{
  // Sanity check on gravity
  if (_g < 0)
    mooseError("Gravity constant g is negative in SVPressureAux");
}

Real SVPressureAux::computeValue()
{
  return 0.5 * _g * _h[_qp] * _h[_qp];
}
