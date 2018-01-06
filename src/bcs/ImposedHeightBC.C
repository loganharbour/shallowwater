#include "ImposedHeightBC.h"

// MOOSE includes
#include "MooseMesh.h"

template <>
InputParameters
validParams<ImposedHeightBC>()
{
  InputParameters params = validParams<IntegratedBC>();

  params.addClassDescription("The boundary condition in which a height is imposed.");

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
                             "that the BC is applied to (required for equation = "
                             "MOMENTUM) [x|y].");

  params.addRequiredParam<Real>("h_imposed", "The imposed height.");
  params.addRequiredParam<Real>("q_imposed", "The imposed discharge which is used in"
                                " torrential flow when v * n < -c, in which"
                                "all the characteristics enter the domain.");

  params.addParam<Real>("g", 9.80665, "The gravity constant (m/s^2).");

  return params;
}


ImposedHeightBC::ImposedHeightBC(const InputParameters & parameters)
  : IntegratedBC(parameters),
    _q_x(coupledValue("q_x")),
    _q_y(isCoupled("q_y") ? coupledValue("q_y") : _zero),
    _h(coupledValue("h")),
    _eq(getParam<MooseEnum>("equation")),
    _comp(isParamValid("component") ? getParam<MooseEnum>("component") : 0),
    _h_imp(getParam<Real>("h_imposed")),
    _q_imp(getParam<Real>("q_imposed")),
    _g(getParam<Real>("g"))
{
  // Do we have a component for the momentum equations
  if (_eq == 1 && !isParamValid("component"))
    mooseError("component is required in ImposedHeightBC for equation = ",
               "MOMENTUM");

  // Sanity check on component
  if (_comp == 1 && _mesh.dimension() != 2)
    mooseError("component in ImposedHeightBC is y but the mesh is 1D");

  // y-component of momentum is required but not given
  if (_mesh.dimension() == 2 && !isCoupled("q_y"))
    mooseError("ImposedHeightBC requires the y-component of momentum, q_y, in 2D");

  // y-component of momentum is given but is not required
  if (_mesh.dimension() == 1 && isCoupled("q_y"))
    mooseError("ImposedHeightBC does not require the y-component of ",
               "momentum, q_y, in 1D but it was provided");
}

Real
ImposedHeightBC::computeQpResidual()
{
  // Sound speed inside domain
  Real c_in = std::sqrt(_g * _h[_qp]);

  // Velocity vector inside domain
  RealVectorValue v_in(_q_x[_qp] / _h[_qp], _q_y[_qp] / _h[_qp], 0);

  // Normal magnitude of velocity inside domain
  Real nu_n_in = v_in * _normals[_qp];

  // Variable values at the boundary that we are solving for
  Real h;
  Real q_x;
  Real q_y;

  // Fluvial flow, |nu_n_in < c_in|, nu_n_in + c_in > 0, nu_n_in < c_in
  if (nu_n_in + c_in > 0 && nu_n_in < c_in)
  {
    // Normal component of velocity outside the domain
    Real nu_n = nu_n_in + 2 * (c_in - std::sqrt(_g * _h_imp));

    // Normal component of momentum outside the domain
    Real q_n = _h_imp * nu_n;

    // Tangential component of momentum outside the domain (preserved)
    Real q_t = - _q_x[_qp] * _normals[_qp](1) + _q_y[_qp] * _normals[_qp](0);

    // x and y-components of momentum outside the domain
    q_x = q_n * _normals[_qp](0) - q_t * _normals[_qp](1);
    q_y = q_n * _normals[_qp](1) + q_t * _normals[_qp](0);

    // Height is imposed
    h = _h_imp;
  }
  // Torrential flow, all characteristics leave, nu_n_in + c_in > 0, nu_n_in > 0
  else if (nu_n_in + c_in > 0 && nu_n_in > c_in)
  {
    // All leave: information comes from inside the domain
    q_x = _q_x[_qp];
    q_y = _q_y[_qp];
    h = _h[_qp];
  }
  // Torrential flow, all characteristics enter, nu_n_in + c_in < 0
  else
  {
    // All enter: information comes from outside the domain (user input)
    q_x = -_q_imp * _normals[_qp](0);
    q_y = -_q_imp * _normals[_qp](1);
    h = _h_imp;
  }

  Real q_dot_n = q_x * _normals[_qp](0) + q_y * _normals[_qp](1);

  // Continuity equation
  if (_eq == 0)
    return q_dot_n * _test[_i][_qp];
  // Momentum equations
  else
  {
    // Pressure term
    Real pressure = _g * h * h * _normals[_qp](_comp) / 2;

    // x-momentum equation
    if (_comp == 0)
      return (q_dot_n * q_x / h + pressure) * _test[_i][_qp];
    // y-momentum equation
    else
      return (q_dot_n * q_y / h + pressure) * _test[_i][_qp];
  }
}
