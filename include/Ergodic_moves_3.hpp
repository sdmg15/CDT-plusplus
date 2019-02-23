/// Causal Dynamical Triangulations in C++ using CGAL
///
/// Copyright © 2019 Adam Getchell
///
/// Performs the set of Pachner moves on a 2+1 dimensional manifold which
/// explore all possible triangulations.

/// @file Ergodic_moves_3.hpp
/// @brief Pachner moves on 2+1 dimensional foliated Delaunay triangulations

#ifndef CDT_PLUSPLUS_ERGODIC_MOVES_3_HPP
#define CDT_PLUSPLUS_ERGODIC_MOVES_3_HPP

#include <Manifold.hpp>

namespace manifold3_moves
{
  enum class move_type
  {
    TWO_THREE = 0,
    THREE_TWO = 1,
    TWO_SIX   = 2,
    SIX_TWO   = 3,
    FOUR_FOUR = 4
  };

  /// @brief Perform a null move
  /// @param manifold The simplicial manifold
  /// @return The null-moved manifold
  [[nodiscard]] inline auto null_move(Manifold3 manifold) { return manifold; }

  /// @brief Perform a TriangulationDataStructure_3::flip on a facet
  /// <https://doc.cgal.org/latest/TDS_3/classTriangulationDataStructure__3.html#a2ad2941984c1eac5561665700bfd60b4>
  /// @param manifold The manifold containing the cell to flip
  /// @param to_be_moved The cell on which to try the move
  /// @return True if move succeeded
  [[nodiscard]] inline auto try_23_move(Manifold3&         manifold,
                                        Cell_handle const& to_be_moved)
  {
    Expects(to_be_moved->info() == 22);
    auto flipped = false;
    // Try every facet of the (2,2) cell
    for (std::size_t i = 0; i < 4; ++i)
    {
      if (manifold.set_triangulation().set_delaunay().flip(to_be_moved, i))
      {
#ifndef NDEBUG
        std::cout << "Facet " << i << " was flippable.\n";
#endif
        flipped = true;
        break;
      }
      else
      {
#ifndef NDEBUG
        std::cout << "Facet " << i << " was not flippable.\n";
#endif
      }
    }
    return flipped;
  }  // try_23_move

  /// @brief Perform a (2,3) move
  ///
  /// A (2,3) move "flips" a timelike face into a timelike edge.
  /// This adds a (2,2) simplex and a timelike edge.
  ///
  /// This function calls try_23_move on (2,2) simplices drawn from a
  /// randomly shuffled container until it succeeds or runs out of simplices.
  ///
  /// If successful, the triangulation is no longer Delaunay.
  ///
  /// @param manifold The simplicial manifold
  /// @return The (2,3) moved manifold
  [[nodiscard]] inline auto do_23_move(Manifold3& manifold)
  {
#ifndef NDEBUG
    std::cout << "Attempting (2,3) move.\n";
#endif
    auto two_two = manifold.get_geometry().get_two_two();
    // Shuffle the container to pick a random sequence of (2,2) cells to try
    std::shuffle(two_two.begin(), two_two.end(), make_random_generator());
    for (auto& cell : two_two)
    {
      if (try_23_move(manifold, cell)) return manifold;
    }
    // We've run out of (2,2) cells
    throw std::domain_error("No (2,3) move is possible.");
  }

  /// @brief Perform a TriangulationDataStructure_3::flip on an edge
  /// https://doc.cgal.org/latest/TDS_3/classTriangulationDataStructure__3.html#a5837d666e4198f707f862003c1ffa033
  /// @param manifold The manifold containing the edge to flip
  /// @param to_be_moved The edge on which to try the move
  /// @return True if move succeeded
  [[nodiscard]] inline auto try_32_move(Manifold3&         manifold,
                                        Edge_handle const& to_be_moved)
  {
    auto flipped = false;
    if (manifold.set_triangulation().set_delaunay().flip(
            std::get<0>(to_be_moved), std::get<1>(to_be_moved),
            std::get<2>(to_be_moved)))
      flipped = true;
    return flipped;
  }

  /// @brief Perform a (3,2) move
  ///
  /// A (3,2) move "flips" a timelike edge into a timelike face.
  /// This removes a (2,2) simplex and the timelike edge.
  ///
  /// This function calls try_32_move on timelike edges drawn from a
  /// randomly shuffled container until it succeeds or runs out of edges.
  ///
  /// If successful, the triangulation is no longer Delaunay.
  ///
  /// @param manifold The simplicial manifold
  /// @return The (3,2) moved manifold
  [[nodiscard]] inline auto do_32_move(Manifold3& manifold)
  {
#ifndef NDEBUG
    std::cout << "Attempting (3,2) move.\n";
#endif
    auto movable_timelike_edges = manifold.get_geometry().get_timelike_edges();
    // Shuffle the container to pick a random sequence of edges to try
    std::shuffle(movable_timelike_edges.begin(), movable_timelike_edges.end(),
                 make_random_generator());
    for (auto& edge : movable_timelike_edges)
    {
      if (try_32_move(manifold, edge))
      {
#ifndef NDEBUG
        std::cout << "Edge was flippable.\n";
#endif
        return manifold;
      }
      else
      {
#ifndef NDEBUG
        std::cout << "Edge not flippable.\n";
#endif
      }
    }
    // We've run out of edges to try
    throw std::domain_error("No (3,2) move possible.");
  }  // do_32_move()

  /// @brief Find a (2,6) move
  ///
  /// This function checks to see if a (2,6) move is possible. Starting with
  /// a (1,3) simplex, it checks neighbors for a (3,1) simplex. The index of
  /// that neighbor is passed via an out parameter.
  ///
  /// @param c The (1,3) simplex that is checked
  /// @param n The out parameter integer of the neighboring (3,1) simplex
  /// @return True if the (2,6) move is possible
  [[nodiscard]] inline auto find_26_move(Cell_handle const& c, int& n)
  {
    Expects(c->info() == 13);
    auto movable = false;
    for (auto i = 0; i < 4; ++i)
    {
#ifndef NDEBUG
      std::cout << "Neighbor " << i << " is of type " << c->neighbor(i)->info()
                << "\n";
#endif
      if (c->neighbor(i)->info() == 31)
      {
        n       = i;
        movable = true;
        break;
      }
    }
    return movable;
  }  // find_26_move()

  /// @brief Perform a (2,6) move
  ///
  /// A (2,6) move inserts a vertex into the spacelike face between a
  /// (1,3) simplex on the bottom connected to a (3,1) simplex on top.
  /// This adds 2 (1,3) simplices and 2 (3,1) simplices.
  /// It adds 2 spacelike faces and 6 timelike faces.
  /// It also adds 2 timelike edges and 3 spacelike edges, as well as the
  /// vertex.
  ///
  /// This function calls find_26_move on (1,3) simplices drawn from a
  /// randomly shuffled container until it succeeds or runs out of simplices.
  ///
  /// If successful, the triangulation is no longer Delaunay.
  ///
  /// @param manifold The simplicial manifold
  /// @return The (2,6) moved manifold
  [[nodiscard]] inline auto do_26_move(Manifold3& manifold)
  {
#ifndef NDEBUG
    std::cout << "Attempting (2,6) move.\n";
#endif
    auto one_three = manifold.get_geometry().get_one_three();
    // Shuffle the container to pick a random sequence of (1,3) cells to try
    std::shuffle(one_three.begin(), one_three.end(), make_random_generator());
    auto neighboring_31_index = std::numeric_limits<int>::max();
    for (auto& bottom : one_three)
    {
      if (find_26_move(bottom, neighboring_31_index))
      {
#ifndef NDEBUG
        std::cout << "neighboring_31_index is " << neighboring_31_index << "\n";
#endif
        Cell_handle top = bottom->neighbor(neighboring_31_index);
        // Calculate the common face with respect to the bottom cell
        auto common_face_index = std::numeric_limits<int>::max();
        Expects(bottom->has_neighbor(top, common_face_index));

        // Get indices of vertices of common face with respect to bottom cell
        auto i1 = (common_face_index + 1) & 3;
        auto i2 = (common_face_index + 2) & 3;
        auto i3 = (common_face_index + 3) & 3;

        // Get vertices of common face
        auto v1 = bottom->vertex(i1);
        auto v2 = bottom->vertex(i2);
        auto v3 = bottom->vertex(i3);

        // Timeslice of vertices should be same
        Expects(v1->info() == v2->info() && v2->info() == v3->info());

        // Do the (2,6) move
        // Insert new vertex
        Vertex_handle v_center =
            manifold.set_triangulation().set_delaunay().tds().insert_in_facet(
                bottom, neighboring_31_index);

        // Checks
        std::vector<Cell_handle> incident_cells;
        manifold.set_triangulation().set_delaunay().tds().incident_cells(
            v_center, std::back_inserter(incident_cells));
        // the (2,6) center vertex should be bounded by 6 simplices
        Expects(incident_cells.size() == 6);
        // Each incident cell should be combinatorially and geometrically valid
        for (auto const& cell : incident_cells)
        {
          Expects(
              manifold.get_triangulation().get_delaunay().tds().is_valid(cell));
        }

        // Now assign a geometric point to the center vertex
        auto center_point =
            CGAL::centroid(v1->point(), v2->point(), v3->point());
#ifndef NDEBUG
        std::cout << "Center point is: " << center_point << "\n";
#endif
        v_center->set_point(center_point);

        // Assign a timevalue to the new vertex
        auto timevalue   = v1->info();
        v_center->info() = timevalue;

#ifndef NDEBUG
        if (manifold.get_triangulation().get_delaunay().tds().is_vertex(
                v_center))
        { std::cout << "It's a vertex in the TDS.\n"; }
        else
        {
          std::cout << "It's not a vertex in the TDS.\n";
        }
        std::cout << "Spacelike face timevalue is " << timevalue << "\n";
        std::cout << "Inserted vertex (" << v_center->point()
                  << ") with timevalue " << v_center->info() << "\n";
#endif

        // Final check
        Expects(manifold.get_triangulation().get_delaunay().tds().is_valid(
            v_center, true, 1));

        return manifold;
      }
      // Try next cell
#ifndef NDEBUG
      std::cout << "Cell not insertable.\n";
#endif
    }
    // We've run out of (1,3) simplices to try
    throw std::domain_error("No (2,6) move possible.");
  }  // do_26_move()

  /// @brief Find a (6,2) move
  ///
  /// This function checks to see if a (6,2) move is possible. Starting
  /// with a vertex, it checks all incident cells. There must be 6
  /// incident cells; 3 should be (3,1) simplices, 3 should be (1,3) simplices,
  /// and there should be no (2,2) simplices.
  /// @param manifold The simplicial manifold
  /// @param candidate The vertex to check
  /// @return True if (6,2) move is possible
  [[nodiscard]] inline auto find_62_move(Manifold3&           manifold,
                                         Vertex_handle const& candidate)
  {
    Expects(manifold.get_triangulation().get_delaunay().is_vertex(candidate));
    Expects(manifold.dim() == 3);

    // Obtain all incident cells
    std::vector<Cell_handle> incident_cells;
    manifold.get_triangulation().get_delaunay().tds().incident_cells(
        candidate, std::back_inserter(incident_cells));
    // We must have 6 cells incident to the vertex to make a (6,2) move
    if (incident_cells.size() != 6)
    {
#ifndef NDEBUG
      std::cout << "Vertex has " << incident_cells.size()
                << " incident cells.\n";
#endif
      return false;
    }

    auto incident_31 = manifold.get_geometry().filter_cells(
        incident_cells, Cell_type::THREE_ONE);
    auto incident_22 = manifold.get_geometry().filter_cells(incident_cells,
                                                            Cell_type::TWO_TWO);
    auto incident_13 = manifold.get_geometry().filter_cells(
        incident_cells, Cell_type::ONE_THREE);

#ifndef NDEBUG
    std::cout << "Vertex has " << incident_31.size()
              << " incident (3,1) simplices and " << incident_22.size()
              << " incident (2,2) simplices and " << incident_13.size()
              << " incident (1,3) simplices.\n";
    manifold.get_geometry().print_cells(incident_cells);
#endif
    return ((incident_31.size() == 3) && (incident_22.empty()) &&
            (incident_13.size() == 3));

  }  // find_62_moves()

  /// @brief Perform a (6,2) move
  ///
  /// A (6,2) move removes a vertex which has 3 incident (3,1) simplices
  /// and 3 (1,3) simplices for a total of 6 incident simplices exactly.
  /// This converts the 3 (1,3) simplices into a single (1,3) simplex on
  /// the bottom and the 3 (3,1) simplices into a single (3,1) simplex on
  /// top. It thus removes 2 (1,3) simplices, 2 (3,1) simplices, 2 spacelike
  /// faces, 6 timelike faces, 3 spacelike edges, 2 timelike edges, and a
  /// single vertex.
  ///
  /// This function calls find_62_move on a randomly shuffled container
  /// of vertices until it succeeds or runs out of vertices.
  ///
  /// If successful, the triangulation remains Delaunay. (Other moves may
  /// change this, however.)
  ///
  /// @param manifold The simplicial manifold
  /// @return The (6,2) moved manifold
  [[nodiscard]] inline auto do_62_move(Manifold3& manifold)
  {
#ifndef NDEBUG
    std::cout << "Attempting (6,2) move.\n";
#endif
    auto vertices = manifold.get_geometry().get_vertices();
    // Shuffle the container to pick a random sequence of vertices to try
    std::shuffle(vertices.begin(), vertices.end(), make_random_generator());
    for (auto& vertex : vertices)
    {
      if (find_62_move(manifold, vertex))
      {
        manifold.set_triangulation().set_delaunay().remove(vertex);
        return manifold;
      }
      // Try next vertex
    }
    // We've run out of vertices to try
    //    throw std::domain_error("No (6,2) move possible.");
    std::cout << "No (6,2) move is possible.\n";
    return manifold;
  }  // do_62_move()

  /// @brief Check move correctness
  /// @param before The manifold before the move
  /// @param after The manifold after the move
  /// @param move The type of move
  /// @return True if the move correctly changed the triangulation
  [[nodiscard]] inline auto check_move(Manifold3 const& before,
                                       Manifold3 const& after,
                                       move_type const& move) -> bool
  {
    switch (move)
    {
      case move_type::FOUR_FOUR:
      {
        return (after.is_valid() && after.N3() == before.N3() &&
                after.N3_31() == before.N3_31() &&
                after.N3_22() == before.N3_22() &&
                after.N3_13() == before.N3_13() && after.N2() == before.N2() &&
                after.N1() == before.N1() && after.N1_TL() == before.N1_TL() &&
                after.N1_SL() == before.N1_SL() && after.N0() == before.N0() &&
                after.max_time() == before.max_time() &&
                after.min_time() == before.min_time());
      }
      case move_type::TWO_THREE:
      {
        return (after.is_valid() && after.N3() == before.N3() + 1 &&
                after.N3_31() == before.N3_31() &&
                after.N3_22() == before.N3_22() + 1 &&
                after.N3_13() == before.N3_13() &&
                after.N2() == before.N2() + 2 &&
                after.N1() == before.N1() + 1 &&
                after.N1_TL() == before.N1_TL() + 1 &&
                after.N1_SL() == before.N1_SL() && after.N0() == before.N0() &&
                after.max_time() == before.max_time() &&
                after.min_time() == before.min_time());
      }
      case move_type::THREE_TWO:
      {
        return (after.is_valid() && after.N3() == before.N3() - 1 &&
                after.N3_31() == before.N3_31() &&
                after.N3_22() == before.N3_22() - 1 &&
                after.N3_13() == before.N3_13() &&
                after.N2() == before.N2() - 2 &&
                after.N1() == before.N1() - 1 &&
                after.N1_TL() == before.N1_TL() - 1 &&
                after.N1_SL() == before.N1_SL() && after.N0() == before.N0() &&
                after.max_time() == before.max_time() &&
                after.min_time() == before.min_time());
      }
      case move_type::TWO_SIX:
      {
        return (after.is_valid() && after.N3() == before.N3() + 4 &&
                after.N3_31() == before.N3_31() + 2 &&
                after.N3_22() == before.N3_22() &&
                after.N3_13() == before.N3_13() + 2 &&
                after.N2() == before.N2() + 8 &&
                after.N1() == before.N1() + 5 &&
                after.N1_TL() == before.N1_TL() + 2 &&
                after.N1_SL() == before.N1_SL() + 3 &&
                after.N0() == before.N0() + 1 &&
                after.max_time() == before.max_time() &&
                after.min_time() == before.min_time());
      }
      case move_type::SIX_TWO:
      {
        return (after.is_valid() && after.N3() == before.N3() - 4 &&
                after.N3_31() == before.N3_31() - 2 &&
                after.N3_22() == before.N3_22() &&
                after.N3_13() == before.N3_13() - 2 &&
                after.N2() == before.N2() - 8 &&
                after.N1() == before.N1() - 5 &&
                after.N1_TL() == before.N1_TL() - 2 &&
                after.N1_SL() == before.N1_SL() - 3 &&
                after.N0() == before.N0() - 1 &&
                after.max_time() == before.max_time() &&
                after.min_time() == before.min_time());
      }
      default:
      {
        return false;
      }
    }
  }  // check_move()

}  // namespace manifold3_moves

#endif  // CDT_PLUSPLUS_ERGODIC_MOVES_3_HPP
