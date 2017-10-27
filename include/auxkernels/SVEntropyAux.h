#ifndef SVENTROPYAUX_H
#define SVENTROPYAUX_H

#include "SVAuxBase.h"

// Forward Declarations
class SVEntropyAux;

template <>
InputParameters validParams<SVEntropyAux>();

class SVEntropyAux : public SVAuxBase
{
public:
  SVEntropyAux(const InputParameters & parameters);

protected:
  virtual Real computeValue() override;
};

#endif
