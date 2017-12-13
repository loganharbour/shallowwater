#include "SVBathymetry.h"

// MOOSE Includes
#include "MooseMesh.h"

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
                               "terrain of the fluid body)");

  MooseEnum components("x=0 y=1");
  params.addRequiredParam<MooseEnum>("component", components, "The component of"
                                     " b to evaluate [x|y].");

  params.addParam<Real>("g", 9.80665, "Constant of gravity (m/s^2).");

  return params;
}

SVBathymetry::SVBathymetry(const InputParameters & parameters)
  : Kernel(parameters),
    _h(coupledValue("h")),
    _h_ivar(coupled("h")),
    // _grad_b(coupledGradient("b")),
    _grad_b(coupledValue("b")),
    _comp(getParam<MooseEnum>("component")),
    _g(getParam<Real>("g"))
{
  // Sanity check on component
  if (_comp == 1 && _mesh.dimension() != 2)
    mooseError("Component in SVBathymetry is y but the mesh is 1D");

  // Sanity check on gravity
  if (_g < 0)
    mooseError("Gravity constant g is negative in SVBathymetry.");
}

Real
SVBathymetry::computeQpResidual()
{
  // return _g * _h[_qp] * _grad_b[_qp](_comp) * _test[_i][_qp];
  return _g * _h[_qp] * _grad_b[_qp] * _test[_i][_qp];
}

Real
SVBathymetry::computeQpOffDiagJacobian(unsigned int jvar)
{
  // With respect to h
  if (jvar == _h_ivar)
    // return _phi[_j][_qp] * _g * _grad_b[_qp](_comp) * _test[_i][_qp];
    return _phi[_j][_qp] * _g * _grad_b[_qp] * _test[_i][_qp];
  // With repsect to q_x or q_y
  else
    return 0;
}
