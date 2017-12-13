#include "SVAdvection.h"

// MOOSE includes
#include "MooseMesh.h"

template <>
InputParameters
validParams<SVAdvection>()
{
  InputParameters params = validParams<Kernel>();
  params.addClassDescription("Computes residual and Jacobian contribution for "
                             "the off diagonal advection term: "
                             "$\\frac{\\vec{q}}{h}$ in the Saint-Venant equations.");

  params.addRequiredCoupledVar("h", "The water height variable.");
  params.addRequiredCoupledVar("q_x", "The variable that expresses the x-component"
                               " of the momentum.");
  params.addCoupledVar("q_y", "The variable that expresses the y-component of "
                       "the momentum (required only in 2D).");

  MooseEnum components("x=0 y=1");
  params.addRequiredParam<MooseEnum>("component", components, "The component of"
                                     "the momentum equation to evaluate [x|y].");

  return params;
}

SVAdvection::SVAdvection(const InputParameters & parameters)
  : Kernel(parameters),
    _h(coupledValue("h")),
    _q_x(coupledValue("q_x")),
    _q_y(isCoupled("q_y") ? coupledValue("q_y") : _zero),
    _h_ivar(coupled("h")),
    _q_x_ivar(coupled("q_x")),
    _q_y_ivar(isCoupled("q_y") ? coupled("q_y") : 0),
    _comp(getParam<MooseEnum>("component"))
{
  // Sanity check on component
  if (_comp == 1 && _mesh.dimension() != 2)
    mooseError("Component in SVAdvection is y but the mesh is 1D");

  // y-component of momentum is required but not given
  if (_mesh.dimension() == 2 && !isCoupled("q_y"))
    mooseError("SVAdvection requires the y-component of momentum, q_y in 2D");
  // y-component of momentum is given but is not required
  if (_mesh.dimension() == 1 && isCoupled("q_y"))
    mooseError("SVAdvection does not require the y-component of momentum, q_y"
               " in 1D but it was provided");
}

Real
SVAdvection::computeQpResidual()
{
  // Momentum vector
  RealVectorValue v_q(_q_x[_qp], _q_y[_qp], 0);

  // -\int (u qx/h dbdx + u dy/h dbdy) = -int u/h \vec{q} \vec{grad}b
  return -_u[_qp] / _h[_qp] * (v_q * _grad_test[_i][_qp]);
}

Real
SVAdvection::computeQpJacobian()
{
  // Velocity vector
  RealVectorValue v(_q_x[_qp] / _h[_qp], _q_y[_qp] / _h[_qp], 0);

  // Diagonal derivitive results in 2 * v(_comp)
  v(_comp) *= 2;

  // -\int (2qx/h dbdx + qy/h dbdy) = -\int (\vec{v} \vec{grad}b) / h
  return -_phi[_j][_qp] * (v * _grad_test[_i][_qp]);
}

Real
SVAdvection::computeQpOffDiagJacobian(unsigned int jvar)
{
  // Velocity vector
  RealVectorValue v(_q_x[_qp] / _h[_qp], _q_y[_qp] / _h[_qp], 0);

  // With respect to h
  if (jvar == _h_ivar)
    return _phi[_j][_qp] * _u[_qp] / _h[_qp] * (v * _grad_test[_i][_qp]);
  // With respect to x-momentum component (q_y equation)
  else if (jvar == _q_x_ivar)
    return -_phi[_j][_qp] * (_grad_test[_i][_qp](0) * _u[_qp] / _h[_qp]);
  // With respect to y-momentum component (q_x equation)
  else if (jvar == _q_y_ivar)
    return -_phi[_j][_qp] * (_grad_test[_i][_qp](1) * _u[_qp] / _h[_qp]);
  else
    return 0;
}
