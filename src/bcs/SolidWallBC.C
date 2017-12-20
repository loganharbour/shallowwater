#include "SolidWallBC.h"

template <>
InputParameters
validParams<SolidWallBC>()
{
  InputParameters params = validParams<IntegratedBC>();

  params.addClassDescription("The wall boundary condition in which the normal "
                             "velocity is assumed to be zero.");

  params.addRequiredCoupledVar("h", "The water height variable.");

  MooseEnum equations("CONTINUITY=0 MOMENTUM=1");
  params.addRequiredParam<MooseEnum>("equation", equations, "The equation type the"
                                     " BC is applied to [CONTINUITY|MOMENTUM].");

  MooseEnum components("x=0 y=1");
  params.addParam<MooseEnum>("component", components, "The component of momentum "
                             "that the BC is applied to (only for equation = "
                             "MOMENTUM) [x|y].");

  params.addParam<Real>("g", 9.80665, "The gravity constant (m/s^2).");

  return params;
}


SolidWallBC::SolidWallBC(const InputParameters & parameters)
  : IntegratedBC(parameters),
    _h(coupledValue("h")),
    _eq(getParam<MooseEnum>("equation")),
    _comp(getParam<MooseEnum>("component")),
    _g(getParam<Real>("g"))
{
}

Real
SolidWallBC::computeQpResidual()
{
  // Continuity equation: zero
  if (_eq == 0)
    return 0;
  // Momentum equation: x or y-component (default is x)
  else
    return 0.5 * _g * _h[_qp] * _h[_qp] * _normals[_qp](_comp) * _test[_i][_qp];
}
