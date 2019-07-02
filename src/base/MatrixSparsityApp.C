#include "MatrixSparsityApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"
#include "MooseSyntax.h"

template <>
InputParameters
validParams<MatrixSparsityApp>()
{
  InputParameters params = validParams<MooseApp>();
  return params;
}

MatrixSparsityApp::MatrixSparsityApp(InputParameters parameters) : MooseApp(parameters)
{
  MatrixSparsityApp::registerAll(_factory, _action_factory, _syntax);
}

MatrixSparsityApp::~MatrixSparsityApp() {}

void
MatrixSparsityApp::registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  ModulesApp::registerAll(f, af, s);
  Registry::registerObjectsTo(f, {"MatrixSparsityApp"});
  Registry::registerActionsTo(af, {"MatrixSparsityApp"});

  /* register custom execute flags, action syntax, etc. here */
}

void
MatrixSparsityApp::registerApps()
{
  registerApp(MatrixSparsityApp);
}

/***************************************************************************************************
 *********************** Dynamic Library Entry Points - DO NOT MODIFY ******************************
 **************************************************************************************************/
extern "C" void
MatrixSparsityApp__registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  MatrixSparsityApp::registerAll(f, af, s);
}
extern "C" void
MatrixSparsityApp__registerApps()
{
  MatrixSparsityApp::registerApps();
}
