/// Causal Dynamical Triangulations in C++ using CGAL
///
/// Copyright © 2018-2019 Adam Getchell
///
/// Tests that foliated triangulations are correctly constructed
/// in 3D and 4D respectively.

/// @file Foliated_triangulation_test.cpp
/// @brief Tests for foliated triangulations
/// @author Adam Getchell

#include <Foliated_triangulation.hpp>
#include <catch2/catch.hpp>

using namespace std;

SCENARIO(
    "Foliated_triangulation std::function compatibility and exception-safety",
    "[triangulation]")
{
  GIVEN("A FoliatedTriangulation3 class.")
  {
    WHEN("It's properties are examined.")
    {
      THEN("It is default constructible.")
      {
        REQUIRE(is_default_constructible<FoliatedTriangulation3>::value);
      }
      /// TODO: Make FoliatedTriangulation no-throw default constructible
      //      THEN("It is no-throw default constructible.")
      //      {
      //        CHECK(is_nothrow_default_constructible<FoliatedTriangulation3>::value);
      //      }
      THEN("It is no-throw destructible.")
      {
        REQUIRE(is_nothrow_destructible<FoliatedTriangulation3>::value);
      }
      THEN("It is copy constructible.")
      {
        REQUIRE(is_copy_constructible<FoliatedTriangulation3>::value);
        cout << "std::function<FoliatedTriangulation3> supported:" << boolalpha
             << is_copy_constructible<FoliatedTriangulation3>::value << "\n";
      }
      /// TODO: Make FoliatedTriangulation no-throw copy constructible
      //      THEN("It is no-throw copy constructible.")
      //      {
      //        CHECK(is_nothrow_copy_constructible<FoliatedTriangulation3>::value);
      //      }
      /// TODO: Make FoliatedTriangulation no-throw copy assignable
      //      THEN("It is no-throw copy assignable.")
      //      {
      //        CHECK(is_nothrow_copy_assignable<FoliatedTriangulation3>::value);
      //      }
      THEN("It is move constructible.")
      {
        REQUIRE(is_move_constructible<FoliatedTriangulation3>::value);
      }
      /// TODO: Make FoliatedTriangulation no-throw move constructible
      //      THEN("It is no-throw move constructible.")
      //      {
      //        CHECK(is_nothrow_move_constructible<FoliatedTriangulation3>::value);
      //      }
      /// TODO: Make FoliatedTriangulation no-throw move assignable
      //      THEN("It is no-throw move assignable.")
      //      {
      //        CHECK(is_nothrow_move_assignable<FoliatedTriangulation3>::value);
      //      }
    }
  }
}

SCENARIO("FoliatedTriangulation3 functions from Delaunay3", "[triangulation]")
{
  GIVEN("A FoliatedTriangulation3.")
  {
    WHEN("Constructing a small triangulation.")
    {
      constexpr auto         desired_simplices = static_cast<int_fast32_t>(640);
      constexpr auto         desired_timeslices = static_cast<int_fast32_t>(4);
      FoliatedTriangulation3 triangulation(desired_simplices,
                                           desired_timeslices);
      THEN("Delaunay3 functions work as expected.")
      {
        CHECK(triangulation.number_of_vertices() > 0);
        std::cout << "Base Delaunay number of vertices is: "
                  << triangulation.number_of_vertices() << "\n";
        CHECK(triangulation.dimension() == 3);
        std::cout << "Base Delaunay dimension is : "
                  << triangulation.dimension() << "\n";
      }
    }
  }
}
SCENARIO("FoliatedTriangulation3 initialization", "[triangulation]")
{
  GIVEN("A 3D foliated triangulation.")
  {
    WHEN("It is default constructed.")
    {
      FoliatedTriangulation3 triangulation;
      THEN("It is not yet correctly foliated.")
      {
        REQUIRE_FALSE(triangulation.is_foliated());
      }
      THEN("The default Delaunay triangulation is valid.")
      {
        REQUIRE(triangulation.is_delaunay());
        REQUIRE(triangulation.is_tds_valid());
        REQUIRE(triangulation.max_time() == 0);
        REQUIRE(triangulation.min_time() == 0);
      }
    }
    WHEN(
        "It is constructed from a Delaunay triangulation with 4 causal "
        "vertices.")
    {
      vector<Delaunay3::Point> Vertices{
          Delaunay3::Point{0, 0, 0}, Delaunay3::Point{0, 1, 0},
          Delaunay3::Point{1, 0, 0}, Delaunay3::Point{0, 0, 1}};
      vector<std::size_t> timevalue{1, 1, 1, 2};
      Causal_vertices     causal_vertices;
      for (gsl::index j = 0; j < 4; ++j)
      {
        causal_vertices.emplace_back(std::make_pair(Vertices[j], timevalue[j]));
      }
      Delaunay3 triangulation(causal_vertices.begin(), causal_vertices.end());
      FoliatedTriangulation3 foliatedTriangulation(triangulation);
      THEN("Triangulation is valid and foliated.")
      {
        // Human verification
        foliatedTriangulation.print_cells();
        REQUIRE(foliatedTriangulation.dimension() == 3);
        REQUIRE(foliatedTriangulation.number_of_vertices() == 4);
        REQUIRE(foliatedTriangulation.number_of_finite_edges() == 6);
        REQUIRE(foliatedTriangulation.number_of_finite_facets() == 4);
        REQUIRE(foliatedTriangulation.number_of_finite_cells() == 1);
        REQUIRE(foliatedTriangulation.is_delaunay());
        REQUIRE(foliatedTriangulation.is_tds_valid());
        REQUIRE(foliatedTriangulation.max_time() == 2);
        REQUIRE(foliatedTriangulation.min_time() == 1);
        //        foliatedTriangulation.check_vertices();
      }
    }
    WHEN("Constructing the minimum triangulation.")
    {
      constexpr auto         desired_simplices  = static_cast<int_fast32_t>(2);
      constexpr auto         desired_timeslices = static_cast<int_fast32_t>(2);
      FoliatedTriangulation3 foliatedTriangulation(desired_simplices,
                                                   desired_timeslices);
      THEN("Triangulation is valid and foliated.")
      {
        REQUIRE(foliatedTriangulation.is_delaunay());
        REQUIRE(foliatedTriangulation.is_tds_valid());
        REQUIRE(foliatedTriangulation.is_foliated());
      }
      THEN("The triangulation has sensible values.")
      {
        auto vertices{foliatedTriangulation.number_of_vertices()};
        CHECK(1 << vertices);
        CHECK(vertices <= 8);
        auto cells{foliatedTriangulation.number_of_finite_cells()};
        CHECK(1 <= cells);
        CHECK(cells <= 12);
        // Human verification
        print_triangulation(foliatedTriangulation);
        foliatedTriangulation.check_vertices();
      }
    }
    WHEN("Constructing a medium triangulation.")
    {
      constexpr auto desired_simplices  = static_cast<int_fast32_t>(6400);
      constexpr auto desired_timeslices = static_cast<int_fast32_t>(7);
      FoliatedTriangulation3 triangulation(desired_simplices,
                                           desired_timeslices);
      THEN("Triangulation is valid and foliated.")
      {
        REQUIRE(triangulation.is_delaunay());
        REQUIRE(triangulation.is_tds_valid());
        REQUIRE(triangulation.is_foliated());
      }
      THEN("The triangulation has sensible values.")
      {
        REQUIRE(triangulation.min_time() == 1);
        // Human verification
        print_triangulation(triangulation);
      }
      THEN("Data members are correctly populated.")
      {
        print_triangulation(triangulation);
        // Every cell is classified as (3,1), (2,2), or (1,3)
        CHECK(triangulation.get_cells().size() ==
              (triangulation.get_three_one().size() +
               triangulation.get_two_two().size() +
               triangulation.get_one_three().size()));
        // Every cell is properly labelled
        for (auto const& cell : triangulation.get_three_one())
        { CHECK(cell->info() == static_cast<int>(Cell_type::THREE_ONE)); }
        for (auto const& cell : triangulation.get_two_two())
        { CHECK(cell->info() == static_cast<int>(Cell_type::TWO_TWO)); }
        for (auto const& cell : triangulation.get_one_three())
        { CHECK(cell->info() == static_cast<int>(Cell_type::ONE_THREE)); }
        CHECK(triangulation.check_cells(triangulation.get_cells()));
        CHECK_FALSE(triangulation.N2_SL().empty());

        CHECK(triangulation.max_time() > 0);
        CHECK(triangulation.min_time() > 0);
        CHECK(triangulation.max_time() > triangulation.min_time());
        // Human verification
        cout << "There are " << triangulation.number_of_finite_edges()
             << " edges.\n";
        cout << "There are " << triangulation.N1_TL() << " timelike edges and "
             << triangulation.N1_SL() << " spacelike edges.\n";
        triangulation.print_edges();
        cout << "There are " << triangulation.number_of_vertices()
             << " vertices with a max timevalue of " << triangulation.max_time()
             << " and a min timevalue of " << triangulation.min_time() << ".\n";
        triangulation.print_volume_per_timeslice();
        for (auto const& edge : triangulation.get_timelike_edges())
        { CHECK(triangulation.classify_edge(edge)); }
        for (auto const& edge : triangulation.get_spacelike_edges())
        { CHECK_FALSE(triangulation.classify_edge(edge)); }
      }
    }
  }
}

SCENARIO("FoliatedTriangulation3 copying", "[triangulation]")
{
  GIVEN("A FoliatedTriangulation3")
  {
    constexpr auto         desired_simplices  = static_cast<int_fast32_t>(6400);
    constexpr auto         desired_timeslices = static_cast<int_fast32_t>(7);
    FoliatedTriangulation3 triangulation(desired_simplices, desired_timeslices);
    WHEN("It is copied")
    {
      auto triangulation2 = triangulation;
      THEN("The two objects are distinct.")
      {
        auto* triangulation_ptr  = &triangulation;
        auto* triangulation2_ptr = &triangulation2;
        CHECK_FALSE(triangulation_ptr == triangulation2_ptr);
      }
      THEN("The foliated triangulations have identical properties.")
      {
        CHECK(triangulation.is_foliated() == triangulation2.is_foliated());
        CHECK(triangulation.number_of_finite_cells() ==
              triangulation2.number_of_finite_cells());
        CHECK(triangulation.min_time() == triangulation2.min_time());
        CHECK(triangulation.get_cells().size() ==
              triangulation2.get_cells().size());
        CHECK(triangulation.get_three_one().size() ==
              triangulation2.get_three_one().size());
        CHECK(triangulation.get_two_two().size() ==
              triangulation2.get_two_two().size());
        CHECK(triangulation.get_one_three().size() ==
              triangulation2.get_one_three().size());
        CHECK(triangulation.N2_SL().size() == triangulation2.N2_SL().size());
      }
    }
  }
}
