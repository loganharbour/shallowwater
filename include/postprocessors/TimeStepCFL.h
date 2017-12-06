#ifndef TIMESTEPCFL_H
#define TIMESTEPCFL_H

#include "ElementPostprocessor.h"

// Forward Declarations
class TimeStepCFL;

template <>
InputParameters validParams<TimeStepCFL>();

class TimeStepCFL : public ElementPostprocessor
{
public:
  TimeStepCFL(const InputParameters & parameters);

protected:
  virtual void initialize();
  virtual void execute();
  virtual Real getValue();
  virtual void threadJoin(const UserObject & uo);

  // Coupled variables
  const VariableValue & _h;
  const VariableValue & _q_x;
  const VariableValue & _q_y;

  // Parameters
  const Real _cfl;
  const Real _g;

  // Value used in communication
  Real _value;
};

#endif
