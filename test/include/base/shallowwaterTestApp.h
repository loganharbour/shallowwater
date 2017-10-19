#ifndef SHALLOWWATERTESTAPP_H
#define SHALLOWWATERTESTAPP_H

#include "MooseApp.h"

class shallowwaterTestApp;

template <>
InputParameters validParams<shallowwaterTestApp>();

class shallowwaterTestApp : public MooseApp
{
public:
  shallowwaterTestApp(InputParameters parameters);
  virtual ~shallowwaterTestApp();

  static void registerApps();
  static void registerObjects(Factory & factory);
  static void associateSyntax(Syntax & syntax, ActionFactory & action_factory);
};

#endif /* SHALLOWWATERTESTAPP_H */
