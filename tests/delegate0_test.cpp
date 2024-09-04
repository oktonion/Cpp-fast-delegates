#include "./testsuit/testsuit.h"

#include "..//delegates/delegate.h"


namespace {
	bool func_called = false;
}

static void void_func() {func_called = true;}
void void_func_void_p(void*)
{
    func_called = true;
}

struct Test
{
	void call()
	{
		func_called = true;
	}

	void call_const() const
	{
		func_called = true;
	}
};

template<class T>
void tmpl_void_func() {}
template<class T>
void tmpl_func_p(T*) {}

template<class T>
delegates::delegate<void> tmpl1_void_func_delegate() {
	return &tmpl_void_func<T>;
}
template<class T>
delegates::delegate<void> tmpl2_void_func_delegate() {
	return &tmpl_void_func<T>;
}

TEST_CASE("Testing cpp delegate 0") {
	
	using delegates::delegate;
	using delegates::bind;

	SUBCASE("Delegate 0 empty construction")
	{
		delegate<void> d0;
		
		CHECK(!d0);
	}

	SUBCASE("Delegate 0 in-place construction")
	{
		delegate<void> d0(&void_func);
		
		CHECK(d0);
	}

	SUBCASE("Delegate 0 copy construction")
	{
		delegate<void> d0 = delegate<void>(&void_func);

		CHECK(d0);
	}

	SUBCASE("Delegate 0 free func invocation")
	{
        delegate<void> d0;
		REQUIRE(!d0);
		d0 = delegate<void>(&void_func);
		CHECK(d0);

		func_called = false;
		d0();
		CHECK(true == func_called);

        void *vptr = 0;

        d0 = delegate<void>(vptr, &void_func_void_p);

		func_called = false;
		d0();
		CHECK(true == func_called);

		func_called = false;
	}

	SUBCASE("Delegate 0 class member func invocation")
	{
		delegate<void> d0;
		REQUIRE(!d0);
		Test tt;
		d0 = delegate<void>(&tt, &Test::call);
		CHECK(d0);

		func_called = false;
		d0();
		CHECK(true == func_called);

		d0 = delegate<void>(&tt, &Test::call_const);

		func_called = false;
		d0();
		CHECK(true == func_called);


		delegate<void> dd0;

		CHECK(dd0 != d0);
		CHECK(d0 != dd0);

		REQUIRE(dd0 != d0);

		dd0 = delegate<void>(&tt, &Test::call_const);

		CHECK(dd0 == d0);
		CHECK(d0 == dd0);

		func_called = false;
		dd0();
		CHECK(true == func_called);

		func_called = false;

	}

	SUBCASE("Delegate 0 comparison")
	{
		delegate<void> d0;
		REQUIRE(!d0);
		d0 = delegate<void>(&tmpl_void_func<int>);
		delegate<void> d0_other = d0;

		CHECK(d0_other == d0);
		CHECK_FALSE(d0_other != d0);
		CHECK_FALSE(d0_other < d0);

		d0_other = delegate<void>(&tmpl_void_func<int>);

		CHECK(d0_other == d0);
		CHECK_FALSE(d0_other != d0);
		CHECK_FALSE(d0_other < d0);

		d0_other = delegate<void>(tmpl1_void_func_delegate<int>());

		CHECK(d0_other == d0);
		CHECK_FALSE(d0_other != d0);
		CHECK_FALSE(d0_other < d0);

		d0 = delegate<void>(tmpl2_void_func_delegate<int>());

		CHECK(d0_other == d0);
		CHECK_FALSE(d0_other != d0);
		CHECK_FALSE(d0_other < d0);

		{
			int a = 0;
			d0_other = delegate<void>(&a, &tmpl_func_p<int>);

			CHECK_FALSE(d0_other == d0);
			CHECK(d0_other != d0);

			d0 = delegate<void>(&a, &tmpl_func_p<int>);

			CHECK(d0_other == d0);
			CHECK_FALSE(d0_other != d0);
			CHECK_FALSE(d0_other < d0);
		}

		d0_other = delegate<void>(&tmpl_void_func<float>);

		CHECK_FALSE(d0_other == d0);
		CHECK(d0_other != d0);
	}
}
