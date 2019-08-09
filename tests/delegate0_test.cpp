#include "./testsuit/testsuit.h"

#include "..//delegates/delegate.h"


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
		delegate<void> d0(0);
		
		CHECK(!d0);
	}
}