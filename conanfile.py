import os
from conan import ConanFile
from conan.tools.files import copy
from conan.tools.cmake import cmake_layout, CMake
from conan.tools.build import check_min_cppstd


# After: https://docs.conan.io/2/tutorial/creating_packages/other_types_of_packages/header_only_packages.html
# and: https://github.com/conan-io/conan-center-index/blob/master/recipes/doctest/2.x.x/conanfile.py
class WolfSoundDspUtilsRecipe(ConanFile):
    name = "wolfsound-dsp-utils"
    version = "0.5.0"
    settings = "os", "arch", "compiler", "build_type"
    exports_sources = "src/*", "tests/*", "CMakeLists.txt", "cmake/*"
    no_copy_source = True
    generators = "CMakeToolchain", "CMakeDeps"
    # Important, define the package_type
    package_type = "header-library"

    def requirements(self):
        # tests/CMakeLists.txt contains a corresponding CPM command so we don't need gtest here
        # self.test_requires("gtest/1.17.0")
        pass

    def validate(self):
        # the test project requires at least C++ 20
        check_min_cppstd(self, 20)

    def layout(self):
        cmake_layout(self)

    def build(self):
        if not self.conf.get("tools.build:skip_test", default=False):
            cmake = CMake(self)
            cmake.configure(variables={"BUILD_TESTS": "ON"})
            cmake.build()
            self.run(
                os.path.join(self.cpp.build.bindir, "tests", "WolfSoundDspUtilsTests")
            )

    def package(self):
        copy(
            self, "*.hpp", os.path.join(self.source_folder, "src"), self.package_folder
        )

    def package_info(self):
        # For header-only packages, libdirs and bindirs are not used
        # so it's necessary to set those as empty.
        self.cpp_info.bindirs = []
        self.cpp_info.libdirs = []
        # These metadata allow CMake commands
        # find_package(WolfSoundDspUtils REQUIRED)
        # target_link_libraries(MyTarget PRIVATE wolfsound::wolfsound_dsp_utils)
        self.cpp_info.set_property("cmake_file_name", "WolfSoundDspUtils")
        self.cpp_info.set_property(
            "cmake_target_name", "wolfsound::wolfsound_dsp_utils"
        )

    def package_id(self):
        self.info.clear()
