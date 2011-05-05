#include <cxxtest/TestRunner.h>
#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/ValueTraits.h>
#include <cxxtest/ValueTraits.h>
#include <cxxtest/ErrorPrinter.h>

class CoverageScannerListener : public CxxTest::ErrorPrinter
{
  public:
    CoverageScannerListener( 
        std::ostream &o=std::cout,
        const char *preLine = ":",
        const char *postLine = "" 
        ) : CxxTest::ErrorPrinter( o, preLine , postLine ) { }

    int run()
    {
      return CxxTest::ErrorPrinter::run();
    }

    void enterTest( const CxxTest::TestDescription & desc)
    {
      test_passed=true;
#ifdef __COVERAGESCANNER__

      // Adjusting the name of the test to display the tests
      // in a tree view in CoverageBrowser
      std::string testname="CxxTest/";
      testname += desc.suiteName();
      testname += "/";
      testname += desc.testName();

      // Reset the code coverage data to get only the code coverage
      // of the actual unit test.
      __coveragescanner_clear();
      __coveragescanner_testname(testname.c_str());
#endif
      return CxxTest::ErrorPrinter::enterTest( desc );
    }

    void leaveTest( const CxxTest::TestDescription & desc)
    {
#ifdef __COVERAGESCANNER__
      // Recording the execution state in the coverage report
      if (test_passed)
        __coveragescanner_teststate("PASSED");
      else
        __coveragescanner_teststate("FAILED");

      // Saving the code coverage report of the unit test
      __coveragescanner_save();
#endif
      return CxxTest::ErrorPrinter::leaveTest( desc );
    }

    void failedTest( 
        const char *file, 
        unsigned line, 
        const char *expression 
        )
    { // Just record that the test fails
      test_passed=false;
      return CxxTest::ErrorPrinter::failedTest(file,line,expression );
    }

    void failedAssert( 
        const char *file, 
        unsigned line,
        const char *expression 
        )
    { // Just record that the test fails
      test_passed=false;
      return CxxTest::ErrorPrinter::failedAssert(file,line,expression );
    }

    void failedAssertEquals( 
        const char *file,
        unsigned line,
        const char *xStr, 
        const char *yStr,
        const char *x,
        const char *y 
        )
    { // Just record that the test fails
      test_passed=false;
      return CxxTest::ErrorPrinter::failedAssertEquals(
          file,
          line,
          xStr,
          yStr,
          x,
          y );
    }

    void failedAssertSameData( 
        const char *file,
        unsigned line,
        const char *xStr,
        const char *yStr,
        const char *sizeStr, 
        const void *x,
        const void *y, 
        unsigned size 
        )
    { // Just record that the test fails
      test_passed=false;
      return CxxTest::ErrorPrinter::failedAssertSameData(
          file,
          line,
          xStr,
          yStr,
          sizeStr,
          x,
          y,
          size );
    }

    void failedAssertDelta( 
        const char *file,
        unsigned line,
        const char *xStr,
        const char *yStr,
        const char *dStr,
        const char *x, 
        const char *y, 
        const char *d 
        )
    { // Just record that the test fails
      test_passed=false;
      return CxxTest::ErrorPrinter::failedAssertDelta(
          file,
          line,
          xStr,
          yStr,
          dStr,
          x,
          y,
          d );
    }

    void failedAssertDiffers(
        const char *file,
        unsigned line,
        const char *xStr, 
        const char *yStr,
        const char *value
        )
    { // Just record that the test fails
      test_passed=false;
      return CxxTest::ErrorPrinter::failedAssertDiffers(
          file,
          line,
          xStr,
          yStr,
          value );
    }

    void failedAssertLessThan( 
        const char *file,
        unsigned line,
        const char *xStr, 
        const char *yStr,
        const char *x,
        const char *y 
        )
    { // Just record that the test fails
      test_passed=false;
      return CxxTest::ErrorPrinter::failedAssertLessThan(
          file,
          line,
          xStr,
          yStr,
          x,
          y );
    }

    void failedAssertLessThanEquals( 
        const char *file,
        unsigned line,
        const char *xStr,
        const char *yStr,
        const char *x, 
        const char *y 
        )
    { // Just record that the test fails
      test_passed=false;
      return CxxTest::ErrorPrinter::failedAssertLessThanEquals(
          file,
          line,
          xStr,
          yStr,
          x,
          y );
    }

    void failedAssertRelation( 
        const char *file, 
        unsigned line,
        const char *relation,
        const char *xStr,
        const char *yStr,
        const char *x,
        const char *y 
        )
    { // Just record that the test fails
      test_passed=false;
      return CxxTest::ErrorPrinter::failedAssertRelation(
          file,
          line,
          relation,
          xStr,
          yStr,
          x,
          y 
          );
    }

    void failedAssertPredicate(
        const char *file,
        unsigned line,
        const char *predicate,
        const char *xStr,
        const char *x )
    { // Just record that the test fails
      test_passed=false;
      return CxxTest::ErrorPrinter::failedAssertPredicate(
          file,
          line,
          predicate,
          xStr,
          x 
          );
    }

    void failedAssertThrows(
        const char *file,
        unsigned line,
        const char *expression,
        const char *type,
        bool otherThrown 
        )
    { // Just record that the test fails
      test_passed=false;
      return CxxTest::ErrorPrinter::failedAssertThrows(
          file,
          line,
          expression,
          type,
          otherThrown );
    }

    void failedAssertThrowsNot( 
        const char *file,
        unsigned line,
        const char *expression 
        )
    { // Just record that the test fails
      test_passed=false;
      return CxxTest::ErrorPrinter::failedAssertThrowsNot(
          file,
          line,
          expression );
    }

  private:
    bool test_passed;
};


int main()
{
#ifdef __COVERAGESCANNER__
  __coveragescanner_install(argv[0]);
#endif

  // Use "CoverageScannerListener().run()" instead of "CxxTest::ErrorPrinter().run()"
  return CoverageScannerListener().run();
}

