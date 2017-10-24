#include "SVContinuity.h"

// MOOSE includes
#include "MooseMesh.h"

template <>
InputParameters
validParams<SVContinuity>()
{
  InputParameters params = validParams<Kernel>();
  params.addClassDescription("Computes the convection flux of the contunity "
                             "equation $\\div{q}$ for use in the Saint-Venant equations");

  params.addRequiredCoupledVar("q_x", "The variable that expresses the x-component"
                              " of the momentum.");
  params.addCoupledVar("q_y", "The variable that expresses the y-component of "
                       "the momentum (required only in 2D).");

  return params;
}

SVContinuity::SVContinuity(const InputParameters & parameters)
  : Kernel(parameters),
    _q_x(coupledValue("q_x")),
    _q_y(isParamValid("q_y") ? coupledValue("q_y") : _zero),
    _q_x_ivar(coupled("q_x")),
    _q_y_ivar(isParamValid("q_y") ? coupled("q_y") : 0)
{
  // y-component of momentum is required but not given
  if (_mesh.dimension() == 2 && !isParamValid("q_y"))
    mooseError("SVContinuity requires the y-component of momentum, q_y in 2D");
  // y-component of momentum is given but is not required
  else if (isParamValid("q_y"))
    mooseError("SVContinuity does not require the y-component of momentum, q_y"
               " in 1D but it was provided");
}

Real
SVContinuity::computeQpResidual()
{
  // Momentum vector
  RealVectorValue v_q(_q_x[_qp], _q_y[_qp], 0);

  return -v_q * _grad_test[_i][_qp];
}

Real
SVContinuity::computeQpOffDiagJacobian(unsigned int jvar)
{
  // With respect to x-momentum component
  if (jvar == _q_x_ivar)
    return _phi[_j][_qp] * _grad_test[_i][_qp](0);
  // With respect to y-momentum component
  else if (jvar == _q_y_ivar)
    return _phi[_j][_qp] * _grad_test[_i][_qp](1);
  else
    return 0;
}
