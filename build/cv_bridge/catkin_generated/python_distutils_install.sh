#!/bin/sh

if [ -n "$DESTDIR" ] ; then
    case $DESTDIR in
        /*) # ok
            ;;
        *)
            /bin/echo "DESTDIR argument must be absolute... "
            /bin/echo "otherwise python's distutils will bork things."
            exit 1
    esac
fi

echo_and_run() { echo "+ $@" ; "$@" ; }

echo_and_run cd "/home/steins_xin/catkin_ws/src/cv_bridge"

# ensure that Python install destination exists
echo_and_run mkdir -p "$DESTDIR/home/steins_xin/catkin_ws/install/lib/python3/dist-packages"

# Note that PYTHONPATH is pulled from the environment to support installing
# into one location when some dependencies were installed in another
# location, #123.
echo_and_run /usr/bin/env \
    PYTHONPATH="/home/steins_xin/catkin_ws/install/lib/python3/dist-packages:/home/steins_xin/catkin_ws/build/lib/python3/dist-packages:$PYTHONPATH" \
    CATKIN_BINARY_DIR="/home/steins_xin/catkin_ws/build" \
    "/usr/bin/python3" \
    "/home/steins_xin/catkin_ws/src/cv_bridge/setup.py" \
     \
    build --build-base "/home/steins_xin/catkin_ws/build/cv_bridge" \
    install \
    --root="${DESTDIR-/}" \
    --install-layout=deb --prefix="/home/steins_xin/catkin_ws/install" --install-scripts="/home/steins_xin/catkin_ws/install/bin"
