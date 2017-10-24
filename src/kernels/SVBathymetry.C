#include "SVBathymetry.h"
#include "Function.h"

template <>
InputParameters
validParams<SVBathymetry>()
{
  InputParameters params = validParams<Kernel>();
  params.addClassDescription("Computes residual and Jacobian contribution for "
                             "the bathymetry terms "
                             "$gh\\frac{\\partial b}{\\partial x}$ or "
                             "$gh\\frac{\\partial b}{\\partial y}$ in the "
                             "Saint-Venant equations.");

  params.addRequiredCoupledVar("h", "The water height variable.");
  params.addRequiredCoupledVar("b", "The aux variable that represents the bathymetry"
                               " data (describes the topography of the bottom"
                               "terrain of the fluid body");
  params.addRequiredParam<unsigned int>("component", "The component of b to evaluate (0,1)->(x,y).");
  params.addRequiredParam<Real>("g", "Constant of gravity (add units here).");

  return params;
}

SVBathymetry::SVBathymetry(const InputParameters & parameters)
  : Kernel(parameters),
    _h(coupledValue("h")),
    _h_ivar(coupled("h")),
    _grad_b(coupledGradient("b")),
    _comp(getParam<unsigned int>("component")),
    _g(getParam<Real>("g"))
{
  // Sanity check on component
  if (_comp > 1)
    mooseError("component in SVBathymetry can only take values 0 or 1");
}

Real
SVBathymetry::computeQpResidual()
{
  return _g * _h[_qp] * _grad_b[_qp](_comp);
}

Real
SVBathymetry::computeQpOffDiagJacobian(unsigned int jvar)
{
  // With respect to h
  if (jvar == _h_ivar)
    return _phi[_j][_qp] * _g * _grad_b[_qp](_comp) * _test[_i][_qp];
  // With repsect to q_x or q_y
  else
    return 0;
}
