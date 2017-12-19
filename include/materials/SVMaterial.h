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

  /// Gravity constant
  const Real _g;

  /// Viscosity approximation coefficients
  const Real _C_max;

  /// Delcared material properties
  MaterialProperty<Real> & _kappa;
  MaterialProperty<Real> & _kappa_max;

  /// Characteristic cell length
  Real _h_cell;
};

#endif
