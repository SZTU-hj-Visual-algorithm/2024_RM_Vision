# catkin_ws

ROS的cv_bridge包重编译

```
.
├── CMakeLists.txt -> /opt/ros/noetic/share/catkin/cmake/toplevel.cmake
└── cv_bridge
    ├── CHANGELOG.rst
    ├── cmake
    │   └── cv_bridge-extras.cmake.in
    ├── CMakeLists.txt
    ├── doc
    │   ├── conf.py
    │   ├── index.rst
    │   └── mainpage.dox
    ├── include
    │   └── cv_bridge
    │       ├── cv_bridge.h
    │       └── rgb_colors.h
    ├── package.xml
    ├── python
    │   ├── CMakeLists.txt
    │   ├── cv_bridge
    │   │   ├── core.py
    │   │   └── __init__.py
    │   └── __init__.py.plain.in
    ├── rosdoc.yaml
    ├── setup.py
    ├── src
    │   ├── boost
    │   │   ├── core
    │   │   │   └── scoped_enum.hpp
    │   │   ├── endian
    │   │   │   ├── conversion.hpp
    │   │   │   └── detail
    │   │   │       └── intrinsic.hpp
    │   │   ├── predef
    │   │   │   ├── detail
    │   │   │   │   ├── _cassert.h
    │   │   │   │   ├── endian_compat.h
    │   │   │   │   └── test.h
    │   │   │   ├── library
    │   │   │   │   └── c
    │   │   │   │       ├── gnu.h
    │   │   │   │       └── _prefix.h
    │   │   │   ├── make.h
    │   │   │   ├── os
    │   │   │   │   ├── android.h
    │   │   │   │   ├── bsd
    │   │   │   │   │   ├── bsdi.h
    │   │   │   │   │   ├── dragonfly.h
    │   │   │   │   │   ├── free.h
    │   │   │   │   │   ├── net.h
    │   │   │   │   │   └── open.h
    │   │   │   │   ├── bsd.h
    │   │   │   │   ├── ios.h
    │   │   │   │   └── macos.h
    │   │   │   ├── other
    │   │   │   │   └── endian.h
    │   │   │   └── version_number.h
    │   │   └── README
    │   ├── CMakeLists.txt
    │   ├── cv_bridge.cpp
    │   ├── module.cpp
    │   ├── module.hpp
    │   ├── module_opencv.cpp
    │   ├── pycompat.hpp
    │   └── rgb_colors.cpp
    └── test
        ├── CMakeLists.txt
        ├── conversions.py
        ├── enumerants.py
        ├── python_bindings.py
        ├── test_compression.cpp
        ├── test_endian.cpp
        ├── test_rgb_colors.cpp
        ├── utest2.cpp
        └── utest.cpp
```

在ros noetic版本中，默认的cv_bridge依赖的opencv版本为4.2.0，所以需要重新自编译cv_bridge

