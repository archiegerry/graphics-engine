#include <catch2/catch_amalgamated.hpp>
#include "../vmlib/mat44.hpp"

TEST_CASE("4x4 matrix by matrix multiplication", "[mat44]")
{
	static constexpr float kEps_ = 1e-6f;

	SECTION("Identity * Identity")
	{
		auto const identity = Mat44f( Mat44f{ 1.f , 0.f, 0.f, 0.f,
											  0.f , 1.f, 0.f, 0.f, 
											  0.f , 0.f, 1.f, 0.f,
											  0.f , 0.f, 0.f, 1.f },

									  Mat44f{ 1.f , 0.f, 0.f, 0.f,
											  0.f , 1.f, 0.f, 0.f,
											  0.f , 0.f, 1.f, 0.f,
											  0.f , 0.f, 0.f, 1.f } );
		
		REQUIRE_THAT(identity(0, 0), WithinAbs(1.f, kEps_));
		REQUIRE_THAT(identity(0, 1), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(identity(0, 2), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(identity(0, 3), WithinAbs(0.f, kEps_));

		REQUIRE_THAT(identity(1, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(identity(1, 1), WithinAbs(1.f, kEps_));
		REQUIRE_THAT(identity(1, 2), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(identity(1, 3), WithinAbs(0.f, kEps_));

		REQUIRE_THAT(identity(2, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(identity(2, 1), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(identity(2, 2), WithinAbs(1.f, kEps_));
		REQUIRE_THAT(identity(2, 3), WithinAbs(0.f, kEps_));

		REQUIRE_THAT(identity(3, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(identity(3, 1), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(identity(3, 2), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(identity(3, 3), WithinAbs(1.f, kEps_));
	}
}

TEST_CASE("4x4 matrix by vector multiplication", "[mat44][vec4]")
{
	//TODO: implement your own tests here
}