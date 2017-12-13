#include "SVPressure.h"

// MOOSE Includes
#include "MooseMesh.h"

template <>
InputParameters
validParams<SVPressure>()
{
  InputParameters params = validParams<Kernel>();
  params.addClassDescription("Computes the residual and Jacobian contribution for "
                             "the pressure term: $0.5 gh^2$ in the Saint-Venant "
                             "equations.");

  params.addRequiredCoupledVar("h", "The water height variable.");

  MooseEnum components("x=0 y=1");
  params.addRequiredParam<MooseEnum>("component", components, "The component to"
                                     "evaluate [x|y].");

  params.addParam<Real>("g", 9.80665, "The gravity constant (m/s^2).");

  return params;
}

SVPressure::SVPressure(const InputParameters & parameters)
  : Kernel(parameters),
    _h(coupledValue("h")),
    _h_ivar(coupled("h")),
    _comp(getParam<MooseEnum>("component")),
    _g(getParam<Real>("g"))
{
  // Sanity check on component
  if (_comp == 1 && _mesh.dimension() != 2)
    mooseError("Component in SVPressure is y but the mesh is 1D");

  // Sanity check on gravity
  if (_g < 0)
    mooseError("Gravity constant g is negative in SVPressure.");
}

Real
SVPressure::computeQpResidual()
{
  return -0.5 * _grad_test[_i][_qp](_comp) * _g * _h[_qp] * _h[_qp];
}

Real
SVPressure::computeQpOffDiagJacobian(unsigned int jvar)
{
  // With respect to h
  if (jvar == _h_ivar)
    return -_phi[_j][_qp] * _grad_test[_i][_qp](_comp) * _g * _h[_qp];
  // Rest are zero
  else
    return 0;
}
