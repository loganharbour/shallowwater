#ifndef SVMATERIAL_H
#define SVMATERIAL_H

#include "Material.h"

// Forward Declarations
class SVMaterial;

template <>
InputParameters validParams<SVMaterial>();

class SVMaterial : public Material
{
public:
  SVMaterial(const InputParameters & parameters);

protected:
  virtual void computeProperties();
  virtual void computeQpProperties();

  /// Viscosity type
  const MooseEnum _viscosity_type;

  /// Mesh dimension
  const unsigned int _mesh_dimension;

  /// Coupled variables
  const VariableValue & _h;
  const VariableValue & _q_x;
  const VariableValue & _q_y;

  /// Coupled bathymetry aux variable
  const VariableValue & _b;

  /// Coupled entropy aux variables
  const VariableValue & _E;
  const VariableValue & _E_old;
  const VariableValue & _E_older;

  /// Coupled entropy component gradients
  const VariableGradient & _grad_F;
  const VariableGradient & _grad_G;

  /// Coupled jump aux variable
  const VariableValue & _jump;

  /// Gravity constant
  const Real _g;

  /// Viscosity approximation coefficients
  const Real _C_entropy;
  const Real _C_jump;
  const Real _C_max;

  /// Delcared material properties
  MaterialProperty<Real> & _kappa;
  MaterialProperty<Real> & _kappa_max;
  MaterialProperty<Real> & _residual;

  /// Epsilon value normalization of unit vectors
  const Real _eps;

  /// Characteristic cell length
  Real _h_cell;
};

#endif
