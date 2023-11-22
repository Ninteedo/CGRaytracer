# CMakeList.txt : CMake project for CGRaytracer, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("CGRaytracer")

# Add source to this project's executable.
add_executable (CGRaytracer "src/CGRaytracer.cpp" "src/CGRaytracer.h" "src/Vector3D.cpp" "src/Vector3D.h" "src/Image.h" "src/Image.cpp" "src/Colour.cpp" "src/Colour.h" "src/Ray.h" "src/Ray.cpp" "src/JSONParser.h" "src/JSONParser.cpp" "test/json_parser_test.cpp"
        src/Scene.h
        src/shapes/Shape.h
        src/lights/LightSource.h
        src/materials/Material.h
        src/cameras/Camera.h
        src/shapes/Sphere.h
        src/shapes/Cylinder.h
        src/shapes/Triangle.h
        src/lights/PointLight.h
        src/shapes/Sphere.cpp
        src/shapes/Shape.cpp
        src/materials/Material.cpp
        src/Scene.cpp
        src/lights/LightSource.cpp
        src/lights/PointLight.cpp
        src/cameras/Camera.cpp
        src/cameras/PinholeCamera.cpp
        src/cameras/PinholeCamera.h
        src/shapes/Cylinder.cpp
        src/shapes/Triangle.cpp
        src/Interval.cpp
        src/Interval.h
        src/Utility.cpp
        src/Utility.h
        src/lights/SphereLight.cpp
        src/lights/SphereLight.h
        src/AABB.cpp
        src/AABB.h
        src/Vector2D.cpp
        src/Vector2D.h
        src/cameras/FiniteApertureCamera.cpp
        src/cameras/FiniteApertureCamera.h
        src/materials/Lambertian.cpp
        src/materials/Lambertian.h
        src/materials/MetalMaterial.cpp
        src/materials/MetalMaterial.h
        src/materials/GlassMaterial.cpp
        src/materials/GlassMaterial.h
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET CGRaytracer PROPERTY CXX_STANDARD 20)
endif()

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
  target_link_libraries(CGRaytracer PUBLIC OpenMP::OpenMP_CXX)
endif()

target_compile_options(CGRaytracer PRIVATE -mavx)

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -ffast-math")


# TODO: Add tests and install targets if needed.
