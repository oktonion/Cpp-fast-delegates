# fast delegates for C++

Extended fast delegates (used Don Clugston original delegates).

How to use:
```
#include "include\delegate.h"

...

using namespace delegates;
```
then:

```
int func(unsigned char val1, size_t &val2) {/*some actual work*/}

...

delegate<int, unsigned char, size_t&> d2(&func);

...

size_t val = 50;
int t = d2(2, val); // calling 'func' by delegate
```
more fun with saving class object:

```
int func(unsigned char val1, size_t &val2) {/*some actual work*/}

class Dummy
{
   int mfunc(unsigned char val1, size_t &val2) {/*some actual class work*/}
   static int sfunc(unsigned char val1, size_t &val2) {/*some actual static work*/}
};

int gfunc(Dummy *pdummy, unsigned char val1, size_t &val2) {/*some actual class or global work*/}
int const_gfunc(const Dummy *pdummy, unsigned char val1, size_t &val2) {/*some actual class or global work*/}

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
int func(unsigned char val1, size_t &val2) {/*some actual work*/}

...

delegate<int, unsigned char, size_t&> d2; // same delegate again

d2 = bind(&func);
```
