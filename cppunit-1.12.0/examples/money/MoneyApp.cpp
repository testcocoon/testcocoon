#ifdef __COVERAGESCANNER__
// This code is only used for testing Money class.
// We exclude it from the code coverage analysis
#pragma CoverageScanner(cov-off)
#endif
#include "StdAfx.h"
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include "../CoverageScannerListener.h"

int main(int argc, char* argv[])
{
#ifdef __COVERAGESCANNER__
  __coveragescanner_install(argv[0]);
#endif
  CPPUNIT_NS::TestResult controller;

  // Add a listener that colllects test result
  CPPUNIT_NS::TestResultCollector result;
  controller.addListener( &result );

  // Add a listener that print dots as test run.
  CPPUNIT_NS::BriefTestProgressListener progress;
  controller.addListener( &progress );

  // Add a listener that saves the code coverage information
  CoverageScannerListener coveragescannerlistener;
  controller.addListener( &coveragescannerlistener );

  // Get the top level suite from the registry
  CPPUNIT_NS::Test *suite = CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest();

  // Adds the test to the list of test to run
  CPPUNIT_NS::TextUi::TestRunner runner;
  runner.addTest( suite );

  // Change the default outputter to a compiler error format outputter
  runner.setOutputter( new CPPUNIT_NS::CompilerOutputter( &runner.result(),
                                                          CPPUNIT_NS::stdCOut() ) );
  // Run the test.
  runner.run(controller);
  bool wasSucessful = result.wasSuccessful() ;

  // Return error code 1 if the one of test failed.
  return wasSucessful ? 0 : 1;
}

