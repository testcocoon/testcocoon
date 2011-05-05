#include <gtest/gtest.h>
#include <direct.h>
#include <stdlib.h>

class CodeCoverageListener : public    ::testing::TestEventListener
{
  public:
    // Fired before any test activity starts.
    virtual void OnTestProgramStart(const ::testing::UnitTest& unit_test) 
    {
    }

    // Fired before each iteration of tests starts.  There may be more than
    // one iteration if GTEST_FLAG(repeat) is set. iteration is the iteration
    // index, starting from 0.
    virtual void OnTestIterationStart(const ::testing::UnitTest& unit_test, int iteration) 
    {
    }

    // Fired before environment set-up for each iteration of tests starts.
    virtual void OnEnvironmentsSetUpStart(const ::testing::UnitTest& unit_test) 
    {
    }

    // Fired after environment set-up for each iteration of tests ends.
    virtual void OnEnvironmentsSetUpEnd(const ::testing::UnitTest& unit_test)
    {
    }

    // Fired before the test case starts.
    virtual void OnTestCaseStart(const ::testing::TestCase& test_case) 
    {
    }

    // Fired before the test starts.
    virtual void OnTestStart(const ::testing::TestInfo& test_info) 
    {
#ifdef __COVERAGESCANNER__
      __coveragescanner_clear(); 
      std::string test_name=
        std::string(test_info.test_case_name())
        + '/'
        + std::string(test_info.name());
      __coveragescanner_testname(test_name.c_str());
#endif
    }

    // Fired after a failed assertion or a SUCCESS().
    virtual void OnTestPartResult(const ::testing::TestPartResult& test_part_result) 
    {
    }

    // Fired after the test ends.
    virtual void OnTestEnd(const ::testing::TestInfo& test_info) 
    {
#ifdef __COVERAGESCANNER__
      __coveragescanner_teststate("UNKNOWN");
      if (test_info.result())
      {
        if (test_info.result()->Passed())
          __coveragescanner_teststate("PASSED");
        if (test_info.result()->Failed())
          __coveragescanner_teststate("FAILED");
      }
      __coveragescanner_save(); 
#endif
    }

    // Fired after the test case ends.
    virtual void OnTestCaseEnd(const ::testing::TestCase& test_case) 
    {
    }

    // Fired before environment tear-down for each iteration of tests starts.
    virtual void OnEnvironmentsTearDownStart(const ::testing::UnitTest& unit_test) 
    {
    }

    // Fired after environment tear-down for each iteration of tests ends.
    virtual void OnEnvironmentsTearDownEnd(const ::testing::UnitTest& unit_test) 
    {
    }

    // Fired after each iteration of tests finishes.
    virtual void OnTestIterationEnd(const ::testing::UnitTest& unit_test, int iteration) 
    {
    }

    // Fired after all test activities have ended.
    virtual void OnTestProgramEnd(const ::testing::UnitTest& unit_test) 
    {
    }
} ;

int main(int argc, char **argv){

  //initialize CoverageScanner library
#ifdef __COVERAGESCANNER__
  __coveragescanner_install(argv[0]);
#endif

  ::testing::FLAGS_gtest_output = "xml";	
  ::testing::UnitTest::GetInstance()->listeners().Append(new CodeCoverageListener);

  ::testing::InitGoogleTest(&argc, argv);	//init google test framework

  return RUN_ALL_TESTS();	//run all tests which are listed in this project
}
