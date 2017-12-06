#include "TimeStepCFL.h"

// MOOSE includes
#include "MooseMesh.h"

// Libmesh includes
#include "libmesh/quadrature.h"

template<>
InputParameters validParams<TimeStepCFL>()
{
  InputParameters params = validParams<ElementPostprocessor>();

  params.addRequiredCoupledVar("h", "The water height variable.");
  params.addCoupledVar("q_y", "The variable that expresses the y-component of "
                       "the momentum (required only in 2D).");
  params.addRequiredCoupledVar("q_x", "The variable that expresses the x-component"
                              " of the momentum.");

  params.addParam<Real>("cfl", 0.8, "The CFL number.");
  params.addParam<Real>("g", 9.80665, "Constant of gravity (m/s^2).");

  return params;
}

TimeStepCFL::TimeStepCFL(const InputParameters & parameters)
  : ElementPostprocessor(parameters),
    _h(coupledValue("h")),
    _q_x(coupledValue("q_x")),
    _q_y(isCoupled("q_y") ? coupledValue("q_y") : _zero),
    _cfl(getParam<Real>("cfl")),
    _g(getParam<Real>("g"))
{
}

void
TimeStepCFL::initialize()
{
  // Initialize to the max: we're looking for the min
  _value = std::numeric_limits<Real>::max();
}

void
TimeStepCFL::execute()
{
  // Characteristic cell size
  Real h_cell = std::pow(_current_elem->volume(), 1./_mesh.dimension());

  // Loop over quadrature points
  for (unsigned int qp = 0; qp < _qrule->n_points(); ++qp)
  {
    // Momentum vector
    RealVectorValue vq(_q_x[qp], _q_y[qp], 0);

    // Local max eigenvalue
    Real eigen = vq.norm() / _h[qp] + std::sqrt(_g * _h[qp]);

    // Local timestep (minimum so far)
    _value = std::min(_value, _cfl * h_cell / eigen);
  }
}

Real
TimeStepCFL::getValue()
{
  gatherMin(_value);
  return _value;
}

void
TimeStepCFL::threadJoin(const UserObject & uo)
{
  const TimeStepCFL & pps = dynamic_cast<const TimeStepCFL &>(uo);
  _value = std::min(_value, pps._value);
}
