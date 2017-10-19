#include "shallowwaterTestApp.h"
#include "shallowwaterApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "MooseSyntax.h"

template <>
InputParameters
validParams<shallowwaterTestApp>()
{
  InputParameters params = validParams<shallowwaterApp>();
  return params;
}

shallowwaterTestApp::shallowwaterTestApp(InputParameters parameters) : MooseApp(parameters)
{
  Moose::registerObjects(_factory);
  shallowwaterApp::registerObjectDepends(_factory);
  shallowwaterApp::registerObjects(_factory);

  Moose::associateSyntax(_syntax, _action_factory);
  shallowwaterApp::associateSyntaxDepends(_syntax, _action_factory);
  shallowwaterApp::associateSyntax(_syntax, _action_factory);

  bool use_test_objs = getParam<bool>("allow_test_objects");
  if (use_test_objs)
  {
    shallowwaterTestApp::registerObjects(_factory);
    shallowwaterTestApp::associateSyntax(_syntax, _action_factory);
  }
}

shallowwaterTestApp::~shallowwaterTestApp() {}

// External entry point for dynamic application loading
extern "C" void
shallowwaterTestApp__registerApps()
{
  shallowwaterTestApp::registerApps();
}
void
shallowwaterTestApp::registerApps()
{
  registerApp(shallowwaterApp);
  registerApp(shallowwaterTestApp);
}

// External entry point for dynamic object registration
extern "C" void
shallowwaterTestApp__registerObjects(Factory & factory)
{
  shallowwaterTestApp::registerObjects(factory);
}
void
shallowwaterTestApp::registerObjects(Factory & /*factory*/)
{
}

// External entry point for dynamic syntax association
extern "C" void
shallowwaterTestApp__associateSyntax(Syntax & syntax, ActionFactory & action_factory)
{
  shallowwaterTestApp::associateSyntax(syntax, action_factory);
}
void
shallowwaterTestApp::associateSyntax(Syntax & /*syntax*/, ActionFactory & /*action_factory*/)
{
}
