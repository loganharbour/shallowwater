#include "SVMaterial.h"

template<>
InputParameters validParams<ComputeViscCoeff>()
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
                       " (required for viscosity_type = ENTROPY).")
  params.addCoupledVar("F", "The aux variable that expresses the x-component of "
                       "the entropy flux (required for viscosity_type = ENTROPY).");
  params.addCoupledVar("G", "The aux variable that expresses the Y-component of "
                       "the entropy flux (required in 2D for viscosity_type = ENTROPY).");

  // Coupled jump aux variable
  params.addCoupledVar("jump_entropy_flux", "The aux variable that expresses the"
                       " jump of the entropy flux (required for viscosity type ="
                       " ENTROPY).");

  // Constants
  params.addRequiredParam<Real>("g", "Constant of gravity.");
  params.addParam<Real>("C_entropy", 1.0, "The coefficient for entropy viscosity.");
  params.addParam<Real>("C_jump", 1.0, "The coefficient for jumps.");
  params.addParam<Real>("C_max", 1.0, "The coefficient for first-order viscosity.");

  return params;
}

SVMaterial::SVMaterial(const InputParameters & parameters) :
  Material(parameters),
  // Viscosity type
  _viscosity_type(getParam<MooseEnum>("viscosity_type")),
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
  _residual(declareProperty<Real>("residual"))
{
}

void
SVMaterial::computeProperties()
{
  // Characteristic length: no need to call this at every quadrature point
  Real _h_cell = std::pow(_current_elem_volume, 1 / _mesh.dimension());

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
  _kappa_max[_qp] = C_max * _h_cell * (3.0 / _h[_qp] + c);

  switch (_viscosity_type)
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
    if (t_step == 1)
      _kappa_qp = _kappa_max[_qp];
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
    }
}
