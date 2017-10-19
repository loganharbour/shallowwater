#ifndef SHALLOWWATERAPP_H
#define SHALLOWWATERAPP_H

#include "MooseApp.h"

class shallowwaterApp;

template <>
InputParameters validParams<shallowwaterApp>();

class shallowwaterApp : public MooseApp
{
public:
  shallowwaterApp(InputParameters parameters);
  virtual ~shallowwaterApp();

  static void registerApps();
  static void registerObjects(Factory & factory);
  static void registerObjectDepends(Factory & factory);
  static void associateSyntax(Syntax & syntax, ActionFactory & action_factory);
  static void associateSyntaxDepends(Syntax & syntax, ActionFactory & action_factory);
};

#endif /* SHALLOWWATERAPP_H */
