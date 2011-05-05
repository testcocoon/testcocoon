#ifndef _TEST_COVERAGE_OBJECT_H
#define _TEST_COVERAGE_OBJECT_H
#include <QObject>
class TestCoverageObject : public QObject
{
  Q_OBJECT
  public:
    virtual void initTest() {}
    virtual void cleanupTest() {}
  protected slots:
    void init() ;
    void cleanup();
};
#endif
