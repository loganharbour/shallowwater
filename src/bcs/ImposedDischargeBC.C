#include "ImposedDischargeBC.h"

template <>
InputParameters
validParams<ImposedDischargeBC>()
{
  InputParameters params = validParams<IntegratedBC>();

  params.addClassDescription("The wall boundary condition in which the normal "
                             "velocity is assumed to be zero.");

  params.addRequiredCoupledVar("q_x", "The variable that expresses the x-component"
                               " of the momentum.");
  params.addCoupledVar("q_y", "The variable that expresses the y-component of "
                       "the momentum (required only in 2D).");
  params.addRequiredCoupledVar("h", "The water height variable.");

  MooseEnum equations("CONTINUITY=0 MOMENTUM=1");
  params.addRequiredParam<MooseEnum>("equation", equations, "The equation type the"
                                     " BC is applied to [CONTINUITY|MOMENTUM].");

  MooseEnum components("x=0 y=1");
  params.addParam<MooseEnum>("component", components, "The component of momentum "
                             "that the BC is applied to (only for equation = "
                             "MOMENTUM) [x|y].");

  params.addRequiredParam<Real>("h_imposed", "The imposed height which is used in"
                                " torrential flow when $v \cdot n < -c$, in which
                                 all the characteristics enter the domain.");
  params.addRequiredParam<Real>("q_imposed", "The imposed discharge where "
                                "$q_imposed = ||q|| = |q \cdot n| \geq 0$ (assuming"
                                " that the imposed discharge is always inwards).");
  params.addParam<Real>("g", 9.80665, "The gravity constant (m/s^2).");

  return params;
}


ImposedDischargeBC::ImposedDischargeBC(const InputParameters & parameters)
  : IntegratedBC(parameters),
    _q_x(coupledValue("q_x")),
    _q_y(isCoupled("q_y") ? coupledValue("q_y") : _zero),
    _h(coupledValue("h")),
    _eq(getParam<MooseEnum>("equation")),
    _comp(getParam<MooseEnum>("component")),
    _h_imposed(getParam<Real>("h_imposed")),
    _q_imposed(getParam<Real>("q_imposed")),
    _g(getParam<Real>("g"))
{
}

Real
ImposedDischargeBC::computeQpResidual()
{
  // Continuity equation
  if (_eq == 0)
    return - _q_imposed * _test[_i][_qp];
  // Momentum equation: x or y-component
  else
  {
    // Sound speed
    Real c = std::sqrt(_g * _h[_qp]);

    // Velocity vector
    RealVectorValue v = (_q_x[_qp] / _h[_qp], _q_y[_qp] / _h[_qp]);

    // Normal component of velocity
    Real v_n = v * _n[_qp];

    // Fluvial flow, |v_n < c|
    if (v_n < c)
    {
      // Initial guess for h at boundary
      Real h_bd = 2 * std::pow(_q_imposed / std::sqrt(_g), 2. / 3) + 1;
    }
    // Torrential flow, all characteristics leave, v_n > c
    else if (v_n > c)
    {

    }
    // Torrential flow, all characteristics enter, v_n < - c
    else
    {

    }
  }
}
