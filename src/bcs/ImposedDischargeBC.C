#include "ImposedDischargeBC.h"

// MOOSE includes
#include "MooseMesh.h"

template <>
InputParameters
validParams<ImposedDischargeBC>()
{
  InputParameters params = validParams<IntegratedBC>();

  params.addClassDescription("The boundary condition in which a discharge normal "
                             "to the boundary is enforced.");

  params.addCoupledVar("q_x", "The variable that expresses the x-component"
                       " of the momentum (required for equation = MOMENTUM).");
  params.addCoupledVar("q_y", "The variable that expresses the y-component of "
                       "the momentum (required only in 2D).");
  params.addCoupledVar("h", "The water height variable.");

  MooseEnum equations("CONTINUITY=0 MOMENTUM=1");
  params.addRequiredParam<MooseEnum>("equation", equations, "The equation type the"
                                     " BC is applied to [CONTINUITY|MOMENTUM].");

  MooseEnum components("x=0 y=1");
  params.addParam<MooseEnum>("component", components, "The component of momentum "
                             "that the BC is applied to (only for equation = "
                             "MOMENTUM) [x|y].");

  params.addRequiredParam<Real>("h_imposed", "The imposed height which is used in"
                                " torrential flow when v * n < -c, in which"
                                "all the characteristics enter the domain.");
  params.addRequiredParam<Real>("q_imposed", "The imposed discharge where "
                                "q_imposed = ||q|| = |q * n| <= 0 (assuming"
                                " that the imposed discharge is always inwards).");

  params.addParam<Real>("g", 9.80665, "The gravity constant (m/s^2).");
  params.addParam<unsigned int>("newton_max", 10, "The max number of Newton "
                                "iterations allowed in computing the zero for "
                                "the fluvial flow case");
  params.addParam<Real>("newton_abs_tol", 1e-12, "The absolute tolerance used in "
                        "converging the Newton iteration for computing the zero "
                        "for the fluvial flow case");

  return params;
}


ImposedDischargeBC::ImposedDischargeBC(const InputParameters & parameters)
  : IntegratedBC(parameters),
    _q_x(isCoupled("q_x") ? coupledValue("q_x") : _zero),
    _q_y(isCoupled("q_y") ? coupledValue("q_y") : _zero),
    _h(isCoupled("h") ? coupledValue("h") : _zero),
    _eq(getParam<MooseEnum>("equation")),
    _comp(isParamValid("component") ? getParam<MooseEnum>("component") : 0),
    _h_imp(getParam<Real>("h_imposed")),
    _q_imp(getParam<Real>("q_imposed")),
    _g(getParam<Real>("g")),
    _newton_max(getParam<unsigned int>("newton_max")),
    _newton_abs_tol(getParam<Real>("newton_abs_tol"))
{
  // Do we have a component for the momentum equations
  if (_eq == 1 && !isParamValid("component"))
    mooseError("component is required in ImposedDischargeBC for equation = ",
               "MOMENTUM");

  // Sanity check on component
  if (_comp == 1 && _mesh.dimension() != 2)
    mooseError("component in ImposedDischargeBC is y but the mesh is 1D");

  // x-component of momentum is required for momentum eq
  if (_eq == 1 && !isCoupled("q_x"))
    mooseError("ImposedDischargeBC requires the x-component of momentum, q_x, ",
               "for the momentum equation");

  // y-component of momentum is required but not given (for momentum eq)
  if (_mesh.dimension() == 2 && !isCoupled("q_y") && _eq == 1)
    mooseError("ImposedDischargeBC requires the y-component of momentum, q_y, ",
               "for the momentum equation, in 2D");

  // y-component of momentum is given but is not required
  if (_mesh.dimension() == 1 && isCoupled("q_y"))
    mooseError("ImposedDischargeBC does not require the y-component of ",
               "momentum, q_y, in 1D but it was provided");

  // Height is required for momentum eq
  if (_eq == 1 && !isCoupled("h"))
    mooseError("ImposedDischargeBC requires the height variable, h, ",
               "for the momentum equation");
}

Real
ImposedDischargeBC::computeQpResidual()
{
  // Continuity equation
  if (_eq == 0)
    return - _q_imp * _test[_i][_qp];
  // Momentum equation: x or y-component
  else
  {
    // Sound speed inside domain
    Real c_in = std::sqrt(_g * _h[_qp]);

    // Velocity vector inside domain
    RealVectorValue v_in(_q_x[_qp] / _h[_qp], _q_y[_qp] / _h[_qp], 0);

    // Normal magnitude of velocity inside domain
    Real nu_n_in = v_in * _normals[_qp];

    // Variable values at the boundary that we are solving for
    Real h;
    Real q_x = -_q_imp * _normals[_qp](0); // Assume we are using user input
    Real q_y = -_q_imp * _normals[_qp](1); // Assume we are using user input

    // Fluvial flow, |nu_n_in < c_in|, nu_n_in + c_in > 0, nu_n_in < c_in
    if (nu_n_in + c_in > 0 && nu_n_in < c_in)
    {
      // Initial guess for h at boundary
      h = 2 * std::pow(_q_imp / std::sqrt(_g), 2. / 3) + 1;
      Real h_last;

      // Solve for the zero
      for (unsigned int i = 0; i <= _newton_max; ++i)
      {
        h_last = h;

        Real f = 2 * std::sqrt(_g * h) * h - _q_imp - (nu_n_in + 2 * c_in) * h;
        Real fp = 3 * std::sqrt(_g * h) - nu_n_in - 2 * c_in;
        h = h - f / fp;

        Real residual = std::abs(h_last - h);
        if (residual < _newton_abs_tol)
          break;

        if (i == _newton_max)
          mooseWarning("h not found after ", i, " iterations (residual = ",
                       residual, ") in ImposedDischargeBC");
      }

      // If h is small, let it be stagnant
      if (h < 1e-12)
      {
        h = 1e-12;
        q_x = 0;
        q_y = 0;
      }
    }
    // Torrential flow, all characteristics leave, nu_n_in + c_in > 0, nu_n_in > 0
    else if (nu_n_in + c_in > 0 && nu_n_in > c_in)
    {
      // All enter: information comes from inside the domain
      h = _h[_qp];
      q_x = _q_x[_qp];
      q_y = _q_y[_qp];
    }
    // Torrential flow, all characteristics enter, nu_n_in + c_in < 0
    else
    {
      // All exit: information comes from outside the domain (user input)
      // Note that q_x and q_y have already been set assuming this
      h = _h_imp;
    }

    // Pressure term
    Real pressure = _g * h * h * _normals[_qp](_comp) / 2;

    // x-momentum equation
    if (_comp == 0)
      return -_q_imp * (q_x / h + pressure) * _test[_i][_qp];
    // y-momentum equation
    else
      return -_q_imp * (q_y / h + pressure) * _test[_i][_qp];
  }
}
