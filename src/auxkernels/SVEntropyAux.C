#include "SVEntropyAux.h"

// MOOSE includes
#include "MooseMesh.h"

template<>
InputParameters
validParams<SVEntropyAux>()
{
  InputParameters params = validParams<SVAuxBase>();
  return params;
}

SVEntropyAux::SVEntropyAux(const InputParameters & parameters)
  : SVAuxBase(parameters)
{
}

Real SVEntropyAux::computeValue()
{
  // Momentum vector
  RealVectorValue vq(_q_x[_qp], _q_y[_qp], 0);

  // Velocity vector
  RealVectorValue v = vq / _h[_qp];

  // size_sq returns the square of the vector, hence u^2+v^2
  return 0.5 * _h[_qp] * (_g * _h[_qp] + v.size_sq());
}
