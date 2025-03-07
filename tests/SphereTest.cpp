/// Causal Dynamical Triangulations in C++ using CGAL
///
/// Copyright © 2014-2019 Adam Getchell
///
/// Tests that 2-spheres and 3-spheres are correctly constructed
/// in 3D and 4D respectively.

/// @file SphereTest.cpp
/// @brief Tests for spheres
/// @author Adam Getchell

#include <S3Triangulation.hpp>
#include <Sphere_d.hpp>
#include <catch2/catch.hpp>

SCENARIO("Construct a foliated 2-sphere", "[sphere]")
{
  GIVEN("Simplices and timeslices")
  {
    constexpr auto simplices  = static_cast<std::int_fast32_t>(640);
    constexpr auto timeslices = static_cast<std::int_fast32_t>(4);
    WHEN("A foliated sphere is constructed.")
    {
      auto causal_vertices = make_foliated_sphere(simplices, timeslices);

      /// @TODO: Why does number of vertices = number of simplices?
      THEN("We have the correct number of vertices.")
      {
        REQUIRE(causal_vertices.size() == simplices);
      }
    }
  }
}

SCENARIO("Construct a higher-dimensional 3-sphere", "[sphere]")
{
  GIVEN("Number of points and dimensionality 4.")
  {
    std::vector<Kd::Point_d> points;
    constexpr auto           number_of_points{50};
    constexpr auto           dim{4};
    constexpr auto           radius{1.0};
    WHEN("A 3-sphere is constructed.")
    {
      make_d_sphere(number_of_points, dim, radius, points);
      THEN("We have the correct number of points.")
      {
        REQUIRE(points.size() == number_of_points);
        std::cout << "3-sphere has " << number_of_points << " points.\n";
      }
    }
  }
}
