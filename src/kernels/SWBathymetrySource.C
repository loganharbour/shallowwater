#include "SWBathymetrySource.h"
#include "Function.h"

template <>
InputParameters
validParams<SWBathymetrySource>()
{
  InputParameters params = validParams<Kernel>();
  params.addClassDescription("Computes residual contribution for "
                             "$gh\\frac{\\partial b}{\\partial x}$ or "
                             "$gh\\frac{\\partial b}{\\partial y}$.");

  params.addRequiredCoupledVar("h", "The water height variable.");
  params.addRequiredParam<FunctionName>("b_func",
                                        "A function that describes the topography of the bottom "
                                        "terain of the fluid body (the bathymetry function).");
  params.addRequiredParam<unsigned int>("component", "The component of b to evaluate (0,1)->(x,y).");
  params.addRequiredParam<Real>("g", "Constant of gravity (add units here).");

  return params;
}

SWBathymetrySource::SWBathymetrySource(const InputParameters & parameters)
  : Kernel(parameters),
    _h(coupledValue("h")),
    _b(getFunction("b_func")),
    _component(getParam<unsigned int>("component")),
    _g(getParam<Real>("gravity"))
{
  if (_component > 1)
    mooseError("Component in SWBathymetrySource can only take values 0 or 1");
}

Real
SWBathymetrySource::computeQpResidual()
{
  return _g * _h[_qp] * _b.gradient(_t, _q_point[_qp])(_component);
}
