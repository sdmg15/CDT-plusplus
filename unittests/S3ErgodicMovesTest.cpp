/// Causal Dynamical Triangulations in C++ using CGAL
///
/// Copyright (c) 2015 Adam Getchell
///
/// Tests for S3 ergodic moves: randomness, (2,3) moves, (3,2) moves,
/// (6,2) moves (disabled), (2,6) moves (disabled)

/// @file S3ErgodicMovesTest.cpp
/// @brief Tests for S3 ergodic moves
/// @author Adam Getchell
/// @bug <a href="http://clang-analyzer.llvm.org/scan-build.html">
/// scan-build</a>: No bugs found.

#include <vector>
#include <tuple>
#include <utility>

#include "gmock/gmock.h"
#include "S3ErgodicMoves.h"

using namespace testing;  // NOLINT

class S3ErgodicMoves : public Test {
 protected:
  virtual void SetUp() {
    // make_S3_triangulation(number_of_simplices,
    //                       number_of_timeslices,
    //                       no_output,
    //                       &S3,
    //                       &three_one,
    //                       &two_two,
    //                       &one_three);
    universe_ptr = std::move(make_triangulation(simplices, timeslices));
    simplex_types = classify_simplices(universe_ptr);
    edge_types = classify_edges(universe_ptr);
  }
  Delaunay universe;
  std::unique_ptr<decltype(universe)>
    universe_ptr = std::make_unique<decltype(universe)>(universe);
  static constexpr auto simplices = static_cast<unsigned>(6400);
  static constexpr auto timeslices = static_cast<unsigned>(16);
  std::tuple<std::vector<Cell_handle>, std::vector<Cell_handle>,
             std::vector<Cell_handle>> simplex_types;
  std::pair<std::vector<Edge_tuple>, unsigned> edge_types;
};

TEST_F(S3ErgodicMoves, GenerateRandomTimeslice) {
  EXPECT_THAT(generate_random_timeslice(timeslices),
    AllOf(Gt(0), Le(timeslices)))
    << "Random timeslice out of bounds.";
}

TEST_F(S3ErgodicMoves, RandomSeedingTest) {
  // Set a wider range than just number_of_timeslices
  constexpr auto test_range_max = static_cast<unsigned>(128);
  const auto value1 = generate_random_timeslice(test_range_max);
  const auto value2 = generate_random_timeslice(test_range_max);
  const auto value3 = generate_random_timeslice(test_range_max);
  const auto value4 = generate_random_timeslice(test_range_max);


  EXPECT_THAT(value1, Ne(value2))
    << "Your random numbers don't seem to be random.";

  EXPECT_THAT(value1, Ne(value3))
    << "Your random numbers don't seem to be random.";

  EXPECT_THAT(value1, Ne(value4))
    << "Your random numbers don't seem to be random.";

  EXPECT_THAT(value2, Ne(value3))
    << "Your random numbers don't seem to be random.";

  EXPECT_THAT(value2, Ne(value4))
    << "Your random numbers don't seem to be random.";

  EXPECT_THAT(value3, Ne(value4))
    << "Your random numbers don't seem to be random.";
}

TEST_F(S3ErgodicMoves, MakeA23Move) {
  auto number_of_vertices_before = universe_ptr->number_of_vertices();
  auto N3_31_before = std::get<0>(simplex_types).size();
  auto N3_22_before = std::get<1>(simplex_types).size();
  auto N3_13_before = std::get<2>(simplex_types).size();
  std::cout << "Number of (2,2) simplices before = " << N3_22_before
            << std::endl;

  // Make the move
  universe_ptr = std::move(make_23_move(universe_ptr, simplex_types));

  // Did we remove a (2,2) Cell_handle?
  EXPECT_THAT(std::get<1>(simplex_types).size(), Eq(N3_22_before-1))
    << "make_23_move removed a copy of a (2,2) simplex vector element.";

  EXPECT_THAT(std::get<0>(simplex_types).size(), Eq(N3_31_before))
    << "make_23_move removed a (3,1) simplex vector element.";

  EXPECT_THAT(std::get<2>(simplex_types).size(), Eq(N3_13_before))
    << "make_23_move removed a (1,3) simplex vector element.";

  // Now look at changes
  simplex_types = classify_simplices(universe_ptr);
  auto N3_31_after = std::get<0>(simplex_types).size();
  auto N3_22_after = std::get<1>(simplex_types).size();
  auto N3_13_after = std::get<2>(simplex_types).size();

  // We expect the triangulation to be valid, but not necessarily Delaunay
  EXPECT_TRUE(universe_ptr->tds().is_valid())
    << "Triangulation is invalid.";

  EXPECT_THAT(universe_ptr->dimension(), Eq(3))
    << "Triangulation has wrong dimensionality.";

  EXPECT_TRUE(check_and_fix_timeslices(universe_ptr))
    << "Some simplices do not span exactly 1 timeslice.";

  EXPECT_THAT(universe_ptr->number_of_vertices(), Eq(number_of_vertices_before))
    << "The number of vertices changed.";

  EXPECT_THAT(N3_31_after, Eq(N3_31_before))
    << "(3,1) simplices changed.";

  EXPECT_THAT(N3_22_after, Eq(N3_22_before+1))
    << "(2,2) simplices did not increase by 1.";

  EXPECT_THAT(N3_13_after, Eq(N3_13_before))
    << "(1,3) simplices changed.";
}

TEST_F(S3ErgodicMoves, MakeA32Move) {
  auto number_of_vertices_before = universe_ptr->number_of_vertices();
  auto N3_31_before = std::get<0>(simplex_types).size();
  auto N3_22_before = std::get<1>(simplex_types).size();
  auto N3_13_before = std::get<2>(simplex_types).size();
  std::cout << "Number of (2,2) simplices before = " << N3_22_before
            << std::endl;
  // std::vector<Edge_tuple> V2;
  // auto N1_SL = static_cast<unsigned>(0);
  //
  // // Get timelike edges
  // get_timelike_edges(S3, &V2, &N1_SL);
  //
  // // Get size of V2
  // auto V2_before = V2.size();
  auto V2_before = edge_types.first.size();

  // Make the move
  // make_32_move(&S3, &V2);
  universe_ptr = std::move(make_32_move(universe_ptr, edge_types));

  // Did we remove a timelike edge?
  EXPECT_THAT(edge_types.first.size(), Eq(V2_before-1))
    << "make_32_move removed a copy of a timelike edge vector element.";

  // Now look at changes
  simplex_types = classify_simplices(universe_ptr);
  auto N3_31_after = std::get<0>(simplex_types).size();
  auto N3_22_after = std::get<1>(simplex_types).size();
  auto N3_13_after = std::get<2>(simplex_types).size();
  edge_types = classify_edges(universe_ptr);
  auto V2_after = edge_types.first.size();

  // We expect the triangulation to be valid, but not necessarily Delaunay
  EXPECT_TRUE(universe_ptr->tds().is_valid())
    << "Triangulation is invalid.";

  EXPECT_THAT(universe_ptr->dimension(), Eq(3))
    << "Triangulation has wrong dimensionality.";

  EXPECT_TRUE(check_and_fix_timeslices(universe_ptr))
    << "Some simplices do not span exactly 1 timeslice.";

  EXPECT_THAT(universe_ptr->number_of_vertices(), Eq(number_of_vertices_before))
    << "The number of vertices changed.";

  EXPECT_THAT(N3_31_after, Eq(N3_31_before))
    << "(3,1) simplices changed.";

  EXPECT_THAT(N3_22_after, Eq(N3_22_before-1))
    << "(2,2) simplices did not decrease by 1.";

  EXPECT_THAT(N3_13_after, Eq(N3_13_before))
    << "(1,3) simplices changed.";

  EXPECT_THAT(V2_after, Eq(V2_before-1))
    << "The edge that was flipped wasn't removed.";
}
//
// TEST_F(S3ErgodicMoves, DISABLED_MakeA26Move) {
//   auto number_of_vertices_before = S3.number_of_vertices();
//   auto N3_31_before = three_one.size();
//   auto N3_22_before = two_two.size();
//   auto N3_13_before = one_three.size();
//   std::cout << "Number of vertices before = " << number_of_vertices_before
//             << std::endl;
//   make_26_move(&S3, &one_three);
//   std::cout << "Number of vertices after = " << S3.number_of_vertices()
//             << std::endl;
//   // Now look at changes
//   reclassify_3_simplices(&S3, &three_one, &two_two, &one_three);
//   auto N3_31_after = three_one.size();
//   auto N3_22_after = two_two.size();
//   auto N3_13_after = one_three.size();
//
//   EXPECT_TRUE(S3.tds().is_valid(true))
//     << "Triangulation is invalid.";
//
//   EXPECT_THAT(S3.dimension(), Eq(3))
//     << "Triangulation has wrong dimensionality.";
//
//   EXPECT_TRUE(check_timeslices(&S3, no_output))
//     << "Cells do not span exactly 1 timeslice.";
//
//   EXPECT_THAT(S3.number_of_vertices(), Eq(number_of_vertices_before+1))
//     << "A vertex was not added to the triangulation.";
//
//   EXPECT_THAT(N3_31_after, Eq(N3_31_before+2))
//     << "(3,1) simplices did not increase by 2.";
//
//   EXPECT_THAT(N3_22_after, Eq(N3_22_before))
//     << "(2,2) simplices changed.";
//
//   EXPECT_THAT(N3_13_after, Eq(N3_13_before+2))
//     << "(1,3) simplices did not increase by 2.";
// }
//
// TEST_F(S3ErgodicMoves, DISABLED_MakeA62Move) {
//   auto number_of_vertices_before = S3.number_of_vertices();
//   auto N3_31_before = three_one.size();
//   auto N3_22_before = two_two.size();
//   auto N3_13_before = one_three.size();
//   std::cout << "Number of vertices before = " << number_of_vertices_before
//             << std::endl;
//   std::vector<Vertex_handle> V;
//   // Get vertices
//   get_vertices(S3, &V);
//
//   EXPECT_THAT(V.size(), Eq(S3.number_of_vertices()))
//     << "Vertex handle vector V doesn't have all vertices in triangulation";
//
//   // Now make the move
//   make_62_move(&S3, &V);
//   std::cout << "Number of vertices after = " << S3.number_of_vertices()
//             << std::endl;
//   // Now look at changes
//   reclassify_3_simplices(&S3, &three_one, &two_two, &one_three);
//   auto N3_31_after = three_one.size();
//   auto N3_22_after = two_two.size();
//   auto N3_13_after = one_three.size();
//
//   EXPECT_TRUE(S3.is_valid())
//     << "Triangulation is not Delaunay.";
//
//   EXPECT_THAT(S3.dimension(), Eq(3))
//     << "Triangulation has wrong dimensionality.";
//
//   EXPECT_TRUE(check_timeslices(&S3, no_output))
//     << "Cells do not span exactly 1 timeslice.";
//
//   EXPECT_THAT(S3.number_of_vertices(), Eq(number_of_vertices_before-1))
//     << "A vertex was not removed from the triangulation.";
//
//   EXPECT_THAT(N3_31_after, Eq(N3_31_before-2))
//     << "(3,1) simplices did not decrease by 2.";
//
//   EXPECT_THAT(N3_22_after, Eq(N3_22_before))
//     << "(2,2) simplices changed.";
//
//   EXPECT_THAT(N3_13_after, Eq(N3_13_before-2))
//     << "(1,3) simplices did not decrease by 2.";
// }
