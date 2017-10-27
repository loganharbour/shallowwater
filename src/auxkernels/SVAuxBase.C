#include "SVAuxBase.h"

// MOOSE includes
#include "MooseMesh.h"

template<>
InputParameters
validParams<SVAuxBase>()
{
  InputParameters params = validParams<AuxKernel>();

  params.addRequiredCoupledVar("h", "The water height variable.");
  params.addRequiredCoupledVar("q_x", "The variable that expresses the x-component"
                               " of the momentum.");
  params.addCoupledVar("q_y", "The variable that expresses the y-component of "
                       "the momentum (required only in 2D).");

  params.addParam<Real>("g", 9.80665, "Constant of gravity (m/s^2).");

  return params;
}

SVAuxBase::SVAuxBase(const InputParameters & parameters)
  : AuxKernel(parameters),
    _h(coupledValue("h")),
    _q_x(coupledValue("q_x")),
    _q_y(coupledValue("q_y")),
    _g(getParam<Real>("g"))
{
  // Sanity check on gravity
  if (_g < 0)
    mooseError("Gravity constant g is negative in SVEntropyAux");

  // y-component of momentum is required but not given
  if (_mesh.dimension() == 2 && !isCoupled("q_y"))
    mooseError("SVEntropyAux requires the y-component of momentum, q_y in 2D");
  // y-component of momentum is given but is not required

  if (_mesh.dimension() == 1 && isCoupled("q_y"))
    mooseError("SVEntropyAux does not require the y-component of momentum, q_y"
               " in 1D but it was provided");
}
