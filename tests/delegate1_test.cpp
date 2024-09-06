#include "./testsuit/testsuit.h"

#include "..//delegates/delegate.h"


namespace {
	bool func_called = false;
	bool child_func_called = false;
}

void void_func_int(int)
{
    func_called = true;
}

void void_func_void_p_int(void*, int)
{
    func_called = true;
}


struct Test
{
	void call(int)
	{
		func_called = true;
	}

	void call_const(int) const
	{
		func_called = true;
	}

	virtual void v_func(int)
	{
		func_called = true;
	}

	virtual void v_func_const(int)
	{
		func_called = true;
	}
};

struct TestChild
	: Test
{
	virtual void v_func(int) 
	{
		child_func_called = true;
	}

	virtual void v_func_const(int)
	{
		child_func_called = true;
	}
};

template<class T>
void tmpl_int_func(int) {}
template<class T>
void tmpl_func_p(T*, int) {}

template<class T>
delegates::delegate<void, int> tmpl1_int_func_delegate() {
	void(*func)(int) = &tmpl_int_func<T>;
	return func;
}
template<class T>
delegates::delegate<void, int> tmpl2_int_func_delegate() {
	void(*func)(int) = &tmpl_int_func<T>;
	return func;
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
        delegate<void, int> d1(&void_func_int);
		
		CHECK(d1);
	}

	SUBCASE("Delegate 1 copy construction")
	{
        delegate<void, int> d1 = delegate<void, int>(&void_func_int);

		CHECK(d1);

        void *vptr = 0;

        d1 = delegate<void, int>(vptr, &void_func_void_p_int);


		delegate<void, int> dd1(d1);
	}

	SUBCASE("Delegate 1 copy")
	{
        delegate<void, int> d1;
		d1 = delegate<void, int>(&void_func_int);
		delegate<void, int> dd1(d1);

		CHECK(d1);
		CHECK(d1 == dd1);

        void *vptr = 0;
		
        d1 = delegate<void, int>(vptr, &void_func_void_p_int);

		CHECK(d1 != dd1);
	}

	SUBCASE("Delegate 1 free func invocation")
	{
		delegate<void, int> d1(&void_func_int);

		func_called = false;
		d1(42);
		CHECK(true == func_called);

		d1 = &void_func_int;

		func_called = false;
		d1(42);
		CHECK(true == func_called);

		delegate<void, int> dd1 = d1;

		func_called = false;
		dd1(42);
		CHECK(true == func_called);

		d1.bind(&void_func_int);

		func_called = false;
		d1(42);
		CHECK(true == func_called);

		func_called = false;

		d1 = delegates::bind(&void_func_int);

		func_called = false;
		d1(42);
		CHECK(true == func_called);

		func_called = false;
	}

	SUBCASE("Delegate 1 free func like member invocation")
	{
        delegate<void, int> d1;
		d1 = delegate<void, int>(&void_func_int);

		func_called = false;
		d1(42);
		CHECK(true == func_called);

        void *vptr = 0;

        d1 = delegate<void, int>(vptr, &void_func_void_p_int);

		func_called = false;
		d1(42);
		CHECK(true == func_called);

		func_called = false;
	}

	SUBCASE("Delegate 1 class member func invocation")
	{
		delegate<void, int> d1;
		Test tt;
		d1 = delegate<void, int>(&tt, &Test::call);

		func_called = false;
		d1(42);
		CHECK(true == func_called);

		d1 = delegate<void, int>(&tt, &Test::call_const);

		func_called = false;
		d1(42);
		CHECK(true == func_called);


		delegate<void, int> dd1 = delegate<void, int>(&tt, &Test::call_const);

		CHECK(dd1 == d1);

		func_called = false;
		dd1(42);
		CHECK(true == func_called); 
		
		d1.bind(&tt, &Test::call);

		func_called = false;
		d1(42);
		CHECK(true == func_called);

		d1 = delegates::bind(&tt, &Test::call);

		func_called = false;
		d1(42);
		CHECK(true == func_called);

		d1.bind(&tt, &Test::call_const);

		func_called = false;
		d1(42);
		CHECK(true == func_called);

		d1 = delegates::bind(&tt, &Test::call_const);

		func_called = false;
		d1(42);
		CHECK(true == func_called);

		func_called = false;

	}

	SUBCASE("Delegate 1 child class member func invocation")
	{
		delegate<void, int> d1;
		TestChild tt;
		d1 = delegate<void, int>(&tt, &Test::call);

		func_called = false;
		d1(42);
		CHECK(true == func_called);

		d1 = delegate<void, int>(&tt, &Test::call_const);

		func_called = false;
		d1(42);
		CHECK(true == func_called);


		delegate<void, int> dd1 = delegate<void, int>(&tt, &Test::call_const);

		CHECK(dd1 == d1);

		func_called = false;
		dd1(42);
		CHECK(true == func_called);

		func_called = false;

	}

	SUBCASE("Delegate 1 child class virtual member func invocation")
	{
		delegate<void, int> d1;
		TestChild tt;
		d1 = delegate<void, int>(&tt, &Test::v_func);

		child_func_called = false;
		d1(42);
		CHECK(true == child_func_called);

		d1 = delegate<void, int>(&tt, &Test::v_func_const);

		child_func_called = false;
		d1(42);
		CHECK(true == child_func_called);


		delegate<void, int> dd1 = delegate<void, int>(&tt, &Test::v_func_const);

		CHECK(dd1 == d1);

		child_func_called = false;
		dd1(42);
		CHECK(true == child_func_called);

		child_func_called = false;

		{
			Test &t = tt;

			d1 = delegate<void, int>(&t, &Test::v_func_const);

			child_func_called = false;
			d1(42);
			CHECK(true == child_func_called);

		
			delegate<void, int> ddd1 = delegate<void, int>(&t, &Test::v_func_const);

			CHECK(ddd1 == d1);

			child_func_called = false;
			ddd1(42);
			CHECK(true == child_func_called);

			child_func_called = false;
		}

	}

	SUBCASE("Delegate 1 comparison")
	{
		delegate<void, int> d1;
		REQUIRE(!d1);
		void(*func)(int) = &tmpl_int_func<int>;
		d1 = delegate<void, int>(func);
		delegate<void, int> d1_other = d1;

		CHECK(d1_other == d1);
		CHECK_FALSE(d1_other != d1);
		CHECK_FALSE(d1_other < d1);

		func = &tmpl_int_func<int>;
		d1_other = delegate<void, int>(func);

		CHECK(d1_other == d1);
		CHECK_FALSE(d1_other != d1);
		CHECK_FALSE(d1_other < d1);

		d1_other = delegate<void, int>(tmpl1_int_func_delegate<int>());

		CHECK(d1_other == d1);
		CHECK_FALSE(d1_other != d1);
		CHECK_FALSE(d1_other < d1);

		d1 = delegate<void, int>(tmpl2_int_func_delegate<int>());

		CHECK(d1_other == d1);
		CHECK_FALSE(d1_other != d1);
		CHECK_FALSE(d1_other < d1);

		{
			int a = 0;
			void(*func)(int*, int) = &tmpl_func_p<int>;
			d1_other = delegate<void, int>(&a, func);

			CHECK_FALSE(d1_other == d1);
			CHECK(d1_other != d1);

			func = &tmpl_func_p<int>;
			d1 = delegate<void, int>(&a, func);

			CHECK(d1_other == d1);
			CHECK_FALSE(d1_other != d1);
			CHECK_FALSE(d1_other < d1);
		}

		{
			Test t;
			d1_other = delegate<void, int>(&t, &Test::call);

			CHECK_FALSE(d1_other == d1);
			CHECK(d1_other != d1);

			d1 = delegate<void, int>(&t, &Test::call);

			CHECK(d1_other == d1);
			CHECK_FALSE(d1_other != d1);
			CHECK_FALSE(d1_other < d1);

			d1 = delegate<void, int>(&t, &Test::call_const);

			CHECK_FALSE(d1_other == d1);
			CHECK(d1_other != d1);

			d1_other = delegate<void, int>(&t, &Test::call_const);

			CHECK(d1_other == d1);
			CHECK_FALSE(d1_other != d1);
			CHECK_FALSE(d1_other < d1);
		}

		func = &tmpl_int_func<float>;
		d1_other = delegate<void, int>(func);

		CHECK_FALSE(d1_other == d1);
		CHECK(d1_other != d1);
	}
}
