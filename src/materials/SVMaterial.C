#include "SVMaterial.h"

// MOOSE includes
#include "MooseMesh.h"

template<>
InputParameters
validParams<SVMaterial>()
{
  InputParameters params = validParams<Material>();

  // Viscosity type
  MooseEnum viscosity_types("NONE=0 FIRST_ORDER=1");
  params.addRequiredParam<MooseEnum>("viscosity_type", viscosity_types, "The "
                                     "viscosity type to use: [NONE|FIRST_ORDER]");

  // Coupled variables
  params.addRequiredCoupledVar("h", "The water height variable.");
  params.addRequiredCoupledVar("q_x", "The variable that expresses the x-component"
                              " of the momentum.");
  params.addCoupledVar("q_y", "The variable that expresses the y-component of "
                       "the momentum (required only in 2D).");

  // Constants
  params.addParam<Real>("g", 9.80665, "Constant of gravity (m/s^2).");
  params.addParam<Real>("C_max", 0.5, "The coefficient for first-order viscosity.");
  params.addParam<Real>("extra_duration", std::numeric_limits<Real>::min(),
                        "The amount of time at the beginning of the transient to"
                        "apply additional artificial viscosity.");

  return params;
}

SVMaterial::SVMaterial(const InputParameters & parameters)
  : Material(parameters),
    // Viscosity type
    _viscosity_type(getParam<MooseEnum>("viscosity_type")),

    // Mesh dimension
    _mesh_dimension(_mesh.dimension()),

    // Coupled variables
    _h(coupledValue("h")),
    _q_x(coupledValue("q_x")),
    _q_y(isCoupled("q_y") ? coupledValue("q_y") : _zero),

    // Constants
    _g(getParam<Real>("g")),
    _C_max_0(getParam<Real>("C_max")),
    _extra_duration(getParam<Real>("extra_duration")),

    // Declare material properties
    _kappa(declareProperty<Real>("kappa")),
    _kappa_max(declareProperty<Real>("kappa_max"))
{
  // y-component of momentum is required but not given
  if (_mesh_dimension == 2 && !isCoupled("q_y"))
    mooseError("SVMaterial requires the y-component of momentum, q_y in 2D");

  // y-component of momentum is given but is not required
  if (_mesh_dimension == 1 && isCoupled("q_y"))
    mooseError("SVMaterial does not require the y-component of momentum, q_y"
               " in 1D but it was given");

  // Sanity check on gravity
  if (_g < 0)
    mooseError("Gravity constant g is negative in SVMaterial.");
}

void
SVMaterial::computeProperties()
{
  // Characteristic length: no need to call this at every quadrature point
  _h_cell = std::pow(_current_elem->volume(), 1 / _mesh_dimension);

  // Add additional viscosity if necessary
  if (_t < _extra_duration)
    _C_max = _C_max_0 * 10 * (_extra_duration - _t) / _extra_duration;
  else
    _C_max = _C_max_0;

  // This exectues SVMaterial::computeQpProperties as necessary
  Material::computeProperties();
}

void
SVMaterial::computeQpProperties()
{
  // Momentum vector
  RealVectorValue v_q(_q_x[_qp], _q_y[_qp], 0);

  // Sound speed
  Real c = std::sqrt(_g * _h[_qp]);

  switch (_viscosity_type)
  {
    // None
    case 0:
      _kappa[_qp] = 0;
      break;
    // First order
    case 1:
      _kappa_max[_qp] = _C_max * _h_cell * (v_q.norm() / _h[_qp] + c);
      _kappa[_qp] = _kappa_max[_qp];
      break;
  }
}
