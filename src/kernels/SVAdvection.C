#include "SVAdvection.h"

// MOOSE includes
#include "MooseMesh.h"
#include "Function.h"

template <>
InputParameters
validParams<SVAdvection>()
{
  InputParameters params = validParams<Kernel>();
  params.addClassDescription("Computes residual and Jacobian contribution for "
                             "the off diagonal advection term: "
                             "$\\frac{q_x q_y}{h}$ or $\\frac{q_x q_y}{h}");

  params.addRequiredCoupledVar("h", "The water height variable.");
  params.addRequiredCoupledVar("q_x", "The variable that expresses the x-component"
                               " of the momentum.");
  params.addCoupledVar("q_y", "The variable that expresses the y-component of "
                       "the momentum (required only in 2D).");
  params.addRequiredParam<unsigned int>("component", "The component of the momentum "
                                        "equation to evaluate (0,1)->(x,y).");

  return params;
}

SVAdvection::SVAdvection(const InputParameters & parameters)
  : Kernel(parameters),
    _h(coupledValue("h")),
    _q_x(coupledValue("q_x")),
    _q_y(isParamValid("q_y") ? coupledValue("q_y") : _zero),
    _comp(getParam<unsigned int>("component")),
    _h_ivar(coupled("h"))
{
  // Sanity check on component
  if (_comp > 1)
    mooseError("component in SVAdvection can only take values 0 or 1");

  // y-component of momentum is required but not given
  if (_mesh.dimension() == 2 && !isParamValid("q_y"))
      mooseError("SVAdvection requires the y-component of momentum, q_y in 2D");
  // y-component of momentum is given but is not required
  else if (isParamValid("q_y"))
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
  return -_phi[_j][_qp] / _h[_qp] * (v * _grad_test[_i][_qp]);
}

Real
SVAdvection::computeQpOffDiagJacobian(unsigned int jvar)
{
  // Velocity vector
  RealVectorValue v(_q_x[_qp] / _h[_qp], _q_y[_qp] / _h[_qp], 0);

  // With respect to h
  if (jvar == _h_ivar)
    return -_phi[_j][_qp] * _u[_qp] / _h[_qp] * (v * _grad_test[_i][_qp]);
  else
    return 0;
}
