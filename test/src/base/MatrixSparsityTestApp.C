//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html
#include "MatrixSparsityTestApp.h"
#include "MatrixSparsityApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "MooseSyntax.h"
#include "ModulesApp.h"

template <>
InputParameters
validParams<MatrixSparsityTestApp>()
{
  InputParameters params = validParams<MatrixSparsityApp>();
  return params;
}

MatrixSparsityTestApp::MatrixSparsityTestApp(InputParameters parameters) : MooseApp(parameters)
{
  MatrixSparsityTestApp::registerAll(
      _factory, _action_factory, _syntax, getParam<bool>("allow_test_objects"));
}

MatrixSparsityTestApp::~MatrixSparsityTestApp() {}

void
MatrixSparsityTestApp::registerAll(Factory & f, ActionFactory & af, Syntax & s, bool use_test_objs)
{
  MatrixSparsityApp::registerAll(f, af, s);
  if (use_test_objs)
  {
    Registry::registerObjectsTo(f, {"MatrixSparsityTestApp"});
    Registry::registerActionsTo(af, {"MatrixSparsityTestApp"});
  }
}

void
MatrixSparsityTestApp::registerApps()
{
  registerApp(MatrixSparsityApp);
  registerApp(MatrixSparsityTestApp);
}

/***************************************************************************************************
 *********************** Dynamic Library Entry Points - DO NOT MODIFY ******************************
 **************************************************************************************************/
// External entry point for dynamic application loading
extern "C" void
MatrixSparsityTestApp__registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  MatrixSparsityTestApp::registerAll(f, af, s);
}
extern "C" void
MatrixSparsityTestApp__registerApps()
{
  MatrixSparsityTestApp::registerApps();
}
