#
# CGRA Project
# If you add new source files, you must list them here
#

# TODO list your header files (.hpp) here
SET(headers
	"cgra_geometry.hpp"
	"cgra_math.hpp"
	"opengl.hpp"
	"imageLoader.hpp"
	"./glm/glm.hpp"
	"quat.hh"
	"agent.hpp"
	"world.hpp"
	"parkobject.hpp"
	"gsim/gs.h"
	"gsim/gs_array.h"
	"gsim/gs_deque.h"
	"gsim/gs_geo2.h"
	"gsim/gs_heap.h"
	"gsim/gs_input.h"
	"gsim/gs_manager.h"
	"gsim/gs_output.h"
	"gsim/gs_polygon.h"
	"gsim/gs_set.h"
	"gsim/gs_shared.h"
	"gsim/gs_string.h"
	"gsim/gs_trace.h"
	"gsim/gs_tree.h"
	"gsim/gs_vec2.h"
	"gsim/se_dcdt.h"
	"gsim/se_dcdt_elements.h"
	"gsim/se_dcdt_manager.h"
	"gsim/se_elements.h"
	"gsim/se_lct.h"
	"gsim/se_mesh.h"
	"gsim/se_mesh_import.h"
	"gsim/se_triangulator.h"
	"gsim/se_triangulator_manager.h"
	"gsim/se_triangulator_tools.h"
)


# TODO list your source files (.cpp) here
SET(sources
	"main.cpp"
	"agent.cpp"
	"world.cpp"
	"parkobject.cpp"
	"gsim/gs.cpp"
	"gsim/gs_array.cpp"
	"gsim/gs_geo2.cpp"
	"gsim/gs_input.cpp"
	"gsim/gs_output.cpp"
	"gsim/gs_polygon.cpp"
	"gsim/gs_set.cpp"
	"gsim/gs_string.cpp"
	"gsim/gs_tree.cpp"
	"gsim/gs_vec2.cpp"
	"gsim/se_dcdt.cpp"
	"gsim/se_dcdt_elements.cpp"
	"gsim/se_dcdt_manager.cpp"
	"gsim/se_elements.cpp"
	"gsim/se_lct.cpp"
	"gsim/se_lct_funnel.cpp"
	"gsim/se_lct_search.cpp"
	"gsim/se_lct_tests.cpp"
	"gsim/se_mesh.cpp"
	"gsim/se_mesh_import.cpp"
	"gsim/se_mesh_io.cpp"
	"gsim/se_mesh_operators.cpp"
	"gsim/se_triangulator.cpp"
	"gsim/se_triangulator_internal.h"
	"gsim/se_triangulator_manager.cpp"
	"gsim/se_triangulator_search.cpp"
	"gsim/se_triangulator_tools.cpp"
)

# Add executable target and link libraries
# You do not need to touch this
add_executable(${CGRA_PROJECT} ${headers} ${sources})
target_link_libraries(${CGRA_PROJECT} PRIVATE glew glfw ${GLFW_LIBRARIES})
target_link_libraries(${CGRA_PROJECT} PRIVATE stb)