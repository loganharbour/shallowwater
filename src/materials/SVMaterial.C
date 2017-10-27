#include "SVMaterial.h"

// MOOSE includes
#include "MooseMesh.h"

template<>
InputParameters
validParams<SVMaterial>()
{
  InputParameters params = validParams<Material>();

  // Viscosity type
  MooseEnum viscosity_types("NONE=0 FIRST_ORDER=1 ENTROPY=2");
  params.addRequiredParam<MooseEnum>("viscosity_type", viscosity_types, "The "
                                     "viscosity type to use: [NONE|FIRST_ORDER"
                                     "|ENTROPY]");

  // Coupled variables
  params.addRequiredCoupledVar("h", "The water height variable.");
  params.addRequiredCoupledVar("q_x", "The variable that expresses the x-component"
                              " of the momentum.");
  params.addCoupledVar("q_y", "The variable that expresses the y-component of "
                       "the momentum (required only in 2D).");

  // Coupled bathymetry aux variable
  params.addCoupledVar("b", "The aux variable that represents the bathymetry"
                            " data (describes the topography of the bottom"
                            "terrain of the fluid body).");

  // Coupled entropy aux variables
  params.addCoupledVar("entropy", "The aux variable that expresses the entropy"
                       " (required for viscosity_type = ENTROPY).");
  params.addCoupledVar("F", "The aux variable that expresses the x-component of "
                       "the entropy flux (required for viscosity_type = ENTROPY).");
  params.addCoupledVar("G", "The aux variable that expresses the Y-component of "
                       "the entropy flux (required in 2D for viscosity_type = ENTROPY).");

  // Coupled jump aux variable
  params.addCoupledVar("jump_entropy_flux", "The aux variable that expresses the"
                       " jump of the entropy flux (required for viscosity type ="
                       " ENTROPY).");

  // Constants
  params.addParam<Real>("g", 9.80665, "Constant of gravity.");
  params.addParam<Real>("C_entropy", 1.0, "The coefficient for entropy viscosity.");
  params.addParam<Real>("C_jump", 1.0, "The coefficient for jumps.");
  params.addParam<Real>("C_max", 0.5, "The coefficient for first-order viscosity.");

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
    _q_y(coupledValue("q_y")),

    // Coupled bathymetry aux variable
    _b(coupledValue("b")),

    // Coupled entropy aux variables
    _E(coupledValue("entropy")),
    _E_old(coupledValueOld("entropy")),
    _E_older(coupledValueOlder("entropy")),
    _grad_F(coupledGradient("F")),
    _grad_G(coupledGradient("G")),

    // Coupled jump aux variable
    _jump(coupledValue("jump_entropy_flux")),

    // Constants
    _g(getParam<Real>("gravity")),
    _C_entropy(getParam<Real>("C_entropy")),
    _C_jump(getParam<Real>("C_jump")),
    _C_max(getParam<Real>("C_max")),

    // Declare material properties
    _kappa(declareProperty<Real>("kappa")),
    _kappa_max(declareProperty<Real>("kappa_max")),
    _residual(declareProperty<Real>("residual")),

    // Epsilon value normalization of unit vectors
    _eps(std::sqrt(std::numeric_limits<Real>::min()))
{
  // y-component of momentum is required but not given
  if (_mesh_dimension == 2 && !isCoupled("q_y"))
    mooseError("SVMaterial requires the y-component of momentum, q_y in 2D");

  // y-component of momentum is given but is not required
  if (_mesh_dimension == 1 && isCoupled("q_y"))
    mooseError("SVMaterial does not require the y-component of momentum, q_y"
               " in 1D but it was given");

  // Missing entropy variables or entropy variables given that are not needed
  // with viscosity_type = ENTROPY
  if (_viscosity_type == 2)
  {
    if (!isCoupled("entropy"))
      mooseError("SVMaterial requires the entropy variable, entropy, for "
                 "viscosity_type = ENTROPY");
    if (!isCoupled("F"))
      mooseError("SVMaterial requires the x-component of the entropy flux, F "
                 "for viscosity_type = ENTROPY");
    if (_mesh_dimension == 2 && !isCoupled("G"))
      mooseError("SVMaterial requires the y-component of the entropy flux, G "
                 "for viscosity_type = ENTROPY when in 2D");
    if (_mesh_dimension == 1 && isCoupled("G"))
      mooseError("SVMaterial does not require the y-component of the entropy "
                 "flux, G, for viscosity_type = ENTROPY in 1D but it was given");
  }

  // Sanity check on gravity
  if (_g < 0)
    mooseError("Gravity constant g is negative in SVMaterial.");
}

void
SVMaterial::computeProperties()
{
  // Characteristic length: no need to call this at every quadrature point
  _h_cell = std::pow(_current_elem->volume(), 1 / _mesh_dimension);

  Material::computeProperties();
}

void
SVMaterial::computeQpProperties()
{
  // Momentum vector
  RealVectorValue v_q(_q_x[_qp], _q_y[_qp], 0);

  // Sound speed
  Real c = std::sqrt(_g * _h[_qp]);

  // First-order viscosity
  _kappa_max[_qp] = _C_max * _h_cell * (3.0 / _h[_qp] + c);

  switch (_viscosity_type)
  {
    // None
    case 0:
      _kappa[_qp] = 0;
      break;
    // First order
    case 1:
      _kappa[_qp] = _kappa_max[_qp];
      break;
    // Entropy method
    case 2:
      // At first time step
      if (_t_step == 1)
        _kappa[_qp] = _kappa_max[_qp];
      // At later time steps
      else
      {
        // Weights for BDF2
        Real sum = _dt + _dt_old;
        Real w0 = (2.0 * _dt + _dt_old) / (_dt * sum);
        Real w1 = -sum / (_dt * _dt_old);
        Real w2 = _dt / (_dt_old * sum);

        // Absolute value of the entropy residual
        _residual[_qp] = w0 * _E[_qp] + w1 * _E_old[_qp] + w2 * _E_older[_qp];
        _residual[_qp] += _grad_F[_qp](0) + _grad_G[_qp](1);
        _residual[_qp] = std::fabs(_residual[_qp]);

        // Normalization is c^2
        Real norm = _g * ( std::fabs(_h[_qp]) + _b[_qp] + _eps);

        // Entropy viscosity
        Real kappa_e = _C_entropy * _residual[_qp] + _C_jump * _jump[_qp];
        kappa_e *= _h_cell * _h_cell / norm;
        mooseAssert(kappa_e < 0, "Entropy viscosity < 0 in" << this->name());

        // Compute and store kappa
        _kappa[_qp] = std::min(_kappa_max[_qp], kappa_e);
      }
      break;
  }
}
