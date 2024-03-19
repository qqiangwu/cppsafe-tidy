// CHECK=cppsafe-unused-return-value

int foo();
[[clang::annotate("cppsafe::may_discard")]] int foo2();

int& foo3();
const int& foo4();

void bar()
{
    // CHECK-MESSAGES: :[[@LINE+1]]:5: warning: the value returned by this function should not be disregarded; neglecting it may lead to errors [cppsafe-unused-return-value]
    foo();

    foo2();

    foo3();

    // CHECK-MESSAGES: :[[@LINE+1]]:5: warning: the value returned by this function should not be disregarded; neglecting it may lead to errors [cppsafe-unused-return-value]
    foo4();
}

using size_t = int;

namespace std {

int memset(void *b, int c, size_t len);
void* memcpy(void* a, void* b, size_t len);
void* memmove(void* a, void* b, size_t len);

}

int memset(void *b, int c, size_t len);
void* memcpy(void* a, void* b, size_t len);
void* memmove(void* a, void* b, size_t len);

int sleep(int);
int usleep(int);

void test_memops()
{
    std::memset({}, {}, {});
    std::memcpy({}, {}, {});
    std::memmove({}, {}, {});

    ::memset({}, {}, {});
    ::memcpy({}, {}, {});
    ::memmove({}, {}, {});

    sleep(0);
    usleep(0);
}

struct AssignOp
{
    AssignOp& operator=(const AssignOp&);

    AssignOp& operator++();
    AssignOp operator++(int);
};

void test_assign()
{
    AssignOp op;

    op = {};
    ++op;

    // CHECK-MESSAGES: :[[@LINE+1]]:5: warning: the value returned by this function should not be disregarded; neglecting it may lead to errors [cppsafe-unused-return-value]
    op++;

    int i = 0;
    ++i;
    i++;
    --i;
    i--;
}

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

void test_regex()
{
    abc::def::ATest a;
    abc::def::BTest b;

    // CHECK-MESSAGES: :[[@LINE+1]]:5: warning: the value returned by this function should not be disregarded; neglecting it may lead to errors [cppsafe-unused-return-value]
    a.SetA();

    // CHECK-MESSAGES: :[[@LINE+1]]:5: warning: the value returned by this function should not be disregarded; neglecting it may lead to errors [cppsafe-unused-return-value]
    b.SetB();
}

namespace std {

template <class T>
struct vector {
    using iterator = T*;

    T& emplace_back(int);
    iterator insert(int);
    iterator emplace(int);
    size_t erase(int);
};

template <class K, class V>
struct map {
    using iterator = K*;

    bool insert(int);
    bool emplace(int);
    size_t erase(int);
};

}

void test_containers()
{
    std::vector<int> v;
    v.emplace_back(0);
    v.insert(0);
    v.emplace(0);
    v.erase(0);

    std::map<int, int> m;

    // CHECK-MESSAGES: :[[@LINE+1]]:5: warning: the value returned by this function should not be disregarded; neglecting it may lead to errors [cppsafe-unused-return-value]
    m.insert(0);
    // CHECK-MESSAGES: :[[@LINE+1]]:5: warning: the value returned by this function should not be disregarded; neglecting it may lead to errors [cppsafe-unused-return-value]
    m.emplace(0);
    m.erase(0);
}

struct atomic {
    int fetch_add(int);
    int fetch_sub(int);
};

void test_fetch_add()
{
    atomic a;
    a.fetch_add(0);
    a.fetch_sub(0);
}