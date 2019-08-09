#include "./testsuit/testsuit.h"

#include "..//delegates/delegate.h"


bool func_called = false;

void void_func_int(int)
{
    func_called = true;
}

void void_func_void_p_int(void*, int)
{
    func_called = true;
}

TEST_CASE("Testing cpp delegate 1") {
	
	using delegates::delegate;
	using delegates::bind;

	SUBCASE("Delegate 1 empty construction")
	{
		delegate<void, int> d1;
		
		CHECK(!d1);
	}

	SUBCASE("Delegate 1 in-place construction")
	{
        delegate<void, int> d1(0);
		
		CHECK(!d1);
	}

	SUBCASE("Delegate 1 copy construction")
	{
        delegate<void, int> d1 = delegate<void, int>(&void_func_int);

		CHECK(d1);

        void *vptr;
        int *iptr;

        d1 = delegate<void, int>(vptr, &void_func_void_p_int);
	}
}