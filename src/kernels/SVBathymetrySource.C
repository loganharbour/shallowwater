#include "SVBathymetrySource.h"
#include "Function.h"

template <>
InputParameters
validParams<SVBathymetrySource>()
{
  InputParameters params = validParams<Kernel>();
  params.addClassDescription("Computes residual and Jacobian contribution for "
                             "the bathymetry source terms "
                             "$gh\\frac{\\partial b}{\\partial x}$ or "
                             "$gh\\frac{\\partial b}{\\partial y}$ in the "
                             "Saint-Venant equations.");

  params.addRequiredCoupledVar("h", "The water height variable.");
  params.addRequiredParam<FunctionName>("b_func",
                                        "A function that describes the topography of the bottom "
                                        "terain of the fluid body (the bathymetry function).");
  params.addRequiredParam<unsigned int>("component", "The component of b to evaluate (0,1)->(x,y).");
  params.addRequiredParam<Real>("g", "Constant of gravity (add units here).");

  return params;
}

SVBathymetrySource::SVBathymetrySource(const InputParameters & parameters)
  : Kernel(parameters),
    _h(coupledValue("h")),
    _h_ivar(coupled("h")),
    _b(getFunction("b_func")),
    _comp(getParam<unsigned int>("component")),
    _g(getParam<Real>("g"))
{
  // Sanity check on component
  if (_comp > 1)
    mooseError("component in SVBathymetrySource can only take values 0 or 1");
}

Real
SVBathymetrySource::computeQpResidual()
{
  return _g * _h[_qp] * _b.gradient(_t, _q_point[_qp])(_comp);
}

Real
SVBathymetrySource::computeQpOffDiagJacobian(unsigned int jvar)
{
  // With respect to h
  if (jvar == _h_ivar)
    return _phi[_j][_qp] * _g * _b.gradient(_t, _q_point[_qp])(_comp) * _test[_i][_qp];
  // With repsect to q_x or q_y
  else
    return 0;
}
