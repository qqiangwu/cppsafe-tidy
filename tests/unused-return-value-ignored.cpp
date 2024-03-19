// CHECK=cppsafe-unused-return-value IgnoredFunctions=foo2;foo3 IgnoredFunctionRegex=^abc::def::.*Test::Set.*

int foo();
int foo2();
int foo3();

namespace abc { namespace def {

struct ATest
{
    int SetA();
};

struct BTest
{
    int SetB();
};

}}

void test_ignore()
{
    abc::def::ATest a;
    abc::def::BTest b;

    // CHECK-MESSAGES: :[[@LINE+1]]:5: warning: the value returned by this function should not be disregarded; neglecting it may lead to errors [cppsafe-unused-return-value]
    foo();
    foo2();
    foo3();
    a.SetA();
    b.SetB();
}
