# fast delegates for C++

Extended fast delegates (used Don Clugston original delegates).

In this delegates you could store:
1) global function
2) global function with class object (both const obj and non-const)
3) static function of class (same as two above actually)
4) member function of class with class object (both const obj and non-const)

Calls to functions stored are extremely fast (like call to original function). Delegate takes minimum memory, are crossplatform and C++98, C++11, C++14 and later standart compatible.

# What are we talking about?
f.e. we have some functions (class functions, global functions, constant or not, whatever...) with same arguments and return value:
```
int func(std::string, size_t&); // regular function
int func(SomeClass*, std::string, size_t&); // same but with class pointer
int func(SomeOtherClass*, std::string, size_t&); // same but with class pointer
int func(const SomeClass*, std::string, size_t&); // same but with class const pointer
int func(const SomeOtherClass*, std::string, size_t&); // same but with class const pointer

int SomeClass::mfunc(std::string, size_t&); // class member function
int SomeClass::mcfunc(std::string, size_t&) const; // class member const function
static int SomeClass::sfunc(std::string, size_t&); // class member static function
static int SomeClass::sfunc(SomeOtherClass*, std::string, size_t&); // same but with class pointer
static int SomeClass::sfunc(const SomeClass*, std::string, size_t&); // same but with class const pointer

int SomeOtherClass::mfunc(std::string, size_t&); // class member function
int SomeOtherClass::mcfunc(std::string, size_t&) const; // class member const function
static int SomeOtherClass::sfunc(std::string, size_t&); // class member static function
static int SomeOtherClass::sfunc(SomeOtherClass*, std::string, size_t&); // same but with class pointer
static int SomeOtherClass::sfunc(const SomeClass*, std::string, size_t&); // same but with class const pointer

// any other class actually...
```
and there is one delegate to rule them all:
```
// could contain any of functions above (with class pointer if needed):

delegate<int, std::string, size_t&> allmighty_delegate;

```

# How to use:
```
#include "delegates\delegate.h"

...

using namespace delegates;
```
then:

```
int func(unsigned char val1, size_t &val2) {/*some actual work*/ return 0;}

...

delegate<int, unsigned char, size_t&> d2(&func);

...

size_t val = 50;
int t = d2(2, val); // calling 'func' by delegate
```
more fun with saving class object:

```
int func(unsigned char val1, size_t &val2) {/*some actual work*/ return 0;}

struct Dummy
{
   int mfunc(unsigned char val1, size_t &val2) {/*some actual class work*/ return 0;}
      
   static int sfunc(unsigned char val1, size_t &val2) {/*some actual static work*/ return 0;}
};

int gfunc(Dummy *pdummy, unsigned char val1, size_t &val2) {/*some actual class or global work*/ return 0;}
int const_gfunc(const Dummy *pdummy, unsigned char val1, size_t &val2) {/*some actual class or global work*/ return 0;}

...

delegate<int, unsigned char, size_t&> d2(&func); // same delegate

...

size_t val = 50;
int t = d2(2, val); // calling 'func' by delegate

Dummy dummy;
d2.bind(&dummy, &Dummy::mfunc); // binding to specific object
t = d2(2, val); // calling 'dummy->mfunc' by delegate

d2.bind(&Dummy::sfunc); // same as just regular 'func' (first example)
t = d2(2, val); // calling 'Dummy::sfunc' by delegate

d2.bind(&dummy, &gfunc); // binding to specific object but global function taking 'object_type'
t = d2(2, val); // calling 'gfunc' by delegate with pointer to 'dummy' as first argument

d2.bind(&dummy, &const_gfunc); // binding to specific object but const global function taking 'object_type'
t = d2(2, val); // calling 'const_gfunc' by delegate with const pointer to 'dummy' as first argument
```

also you could use 'bind' to create delegate:

```
int func(unsigned char val1, size_t &val2) {/*some actual work*/ return 0;}

...

delegate<int, unsigned char, size_t&> d2; // same delegate again

d2 = bind(&func);
```
