#include "shallowwaterApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"
#include "MooseSyntax.h"

// Saint-Venant auxkernels
#include "SVEntropyAux.h"

// Saint-Venant kernels
#include "SVAdvection.h"
#include "SVArtificialViscosity.h"
#include "SVBathymetry.h"
#include "SVContinuity.h"
#include "SVPressure.h"

// Saint-Venant material
#include "SVMaterial.h"

template <>
InputParameters
validParams<shallowwaterApp>()
{
  InputParameters params = validParams<MooseApp>();
  return params;
}

shallowwaterApp::shallowwaterApp(InputParameters parameters) : MooseApp(parameters)
{
  Moose::registerObjects(_factory);
  ModulesApp::registerObjects(_factory);
  shallowwaterApp::registerObjects(_factory);

  Moose::associateSyntax(_syntax, _action_factory);
  ModulesApp::associateSyntax(_syntax, _action_factory);
  shallowwaterApp::associateSyntax(_syntax, _action_factory);
}

shallowwaterApp::~shallowwaterApp() {}

// External entry point for dynamic application loading
extern "C" void
shallowwaterApp__registerApps()
{
  shallowwaterApp::registerApps();
}
void
shallowwaterApp::registerApps()
{
  registerApp(shallowwaterApp);
}

void
shallowwaterApp::registerObjectDepends(Factory & /*factory*/)
{
}

// External entry point for dynamic object registration
extern "C" void
shallowwaterApp__registerObjects(Factory & factory)
{
  shallowwaterApp::registerObjects(factory);
}
void
shallowwaterApp::registerObjects(Factory & factory)
{
  // Saint-Venant auxkernels
  registerKernel(SVEntropyAux);
  
  // Saint-Venant kernels
  registerKernel(SVAdvection);
  registerKernel(SVArtificialViscosity);
  registerKernel(SVBathymetry);
  registerKernel(SVContinuity);
  registerKernel(SVPressure);

  // Saint-Venant material
  registerMaterial(SVMaterial);
}

void
shallowwaterApp::associateSyntaxDepends(Syntax & /*syntax*/, ActionFactory & /*action_factory*/)
{
}

// External entry point for dynamic syntax association
extern "C" void
shallowwaterApp__associateSyntax(Syntax & syntax, ActionFactory & action_factory)
{
  shallowwaterApp::associateSyntax(syntax, action_factory);
}
void
shallowwaterApp::associateSyntax(Syntax & /*syntax*/, ActionFactory & /*action_factory*/)
{
}
