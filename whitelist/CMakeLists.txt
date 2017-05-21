add_library(WhiteListPass MODULE
    # List your source files here.
    WhiteList.cpp
)

# Use C++11 to compile our pass (i.e., supply -std=c++11).
target_compile_features(WhiteListPass PRIVATE cxx_range_for cxx_auto_type)

# LLVM is (typically) built with no C++ RTTI. We need to match that;
# otherwise, we'll get linker errors about missing RTTI data.
set_target_properties(WhiteListPass PROPERTIES
    COMPILE_FLAGS "-fno-rtti -D_GLIBCXX_USE_CXX11_ABI=0 "
)


