#include <cppunit/TestListener.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

class CoverageScannerListener : public CppUnit::TestListener
{
public:
  CoverageScannerListener() 
  {
     m_testFailed=false;
  }

  void startTest( CppUnit::Test *test )
  {
    m_testFailed=false;
#ifdef __COVERAGESCANNER__
    int pos;
    // Adjusting the name of the test to display the tests
    // in a tree view in CoverageBrowser
    std::string testname = "CppUnit/" + test->getName();
    while ( (pos=testname.find("::",0)) != std::string::npos )
      testname.replace(pos,2,"/");

    // Reset the code coverage data to get only the code coverage 
    // of the actual unit test.
    __coveragescanner_clear();
    __coveragescanner_testname(testname.c_str()) ;
#endif
  }

  void addFailure( const CppUnit::TestFailure &failure ) { m_testFailed=true; }

  void endTest( CppUnit::Test *test )
  {
#ifdef __COVERAGESCANNER__
    // Recording the execution state in the coverage report
    if (m_testFailed)
      __coveragescanner_teststate("FAILED");
    else
      __coveragescanner_teststate("PASSED");

    // Saving the code coverage report of the unit test
    __coveragescanner_save();
    __coveragescanner_testname("");
#endif
  }

private:
  bool m_testFailed;
  // Prevents the use of the copy constructor and operator.
  CoverageScannerListener( const CoverageScannerListener &copy );
  void operator =( const CoverageScannerListener &copy );
};
