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
  params.addCoupledVar("q_other", "The other momentum variable.");
  params.addRequiredParam<unsigned int>("component", "The component of the momentum "
                                        "equation to evaluate (0,1)->(x,y).");

  return params;
}

SVAdvection::SVAdvection(const InputParameters & parameters)
  : Kernel(parameters),
    _h(coupledValue("h")),
    _qo(isParamValid("q_other") ? coupledValue("q_other") : _zero),
    _comp_u(getParam<unsigned int>("component")),
    _comp_qo(_comp_u == 0 ? 1 : 0)
{
  // Sanity check on component
  if (_comp_u > 1)
    mooseError("component in SVAdvection can only take values 0 or 1");

  // Other component of momentum is required but it is not given
  if (_mesh.dimension() == 2 && !isParamValid("q_other"))
      mooseError("SVAdvection requires the other momentum variable q_other in "
                 "2D but it was not provided");
  // Other component of momentum is not required but it is given
  else if (isParamValid("q_other"))
      mooseError("SVAdvection does not require the other momentum variable "
                 "q_other in 1D but it was provided");
}

Real
SVAdvection::computeQpResidual()
{
  // Diagonal component * grad test
  Real components = _u[_qp] * _grad_test[_i][_qp](_comp_u);

  // Off-diagonal component if 2D * grad test
  if (_use_qo)
    components += _qo[_qp] * _grad_test[_i][_qp](_comp_qo);

  // -\int (qxqx/h dbdx + qxqy/h dbdy) or -\int (qxqy/h dbdx + qyqy/h dbdy)
  return -components * _u[_qp] / _h[_qp];
}

Real SVAdvection::computeQpJacobian()
{
  // Diagonal component
  Real components = 2 * _u[_qp] * _grad_test[_i][_qp](_comp_u);

  // Off-diagonal component if 2D
  if (_use_qo)
    components += _qo[_qp] * _grad_test[_i][_qp](_comp_qo);

  return -components * _phi[_j][_qp] / _h[_qp];
}
