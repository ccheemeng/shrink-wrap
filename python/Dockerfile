FROM quay.io/pypa/manylinux_2_28_x86_64 AS build

WORKDIR /tmp

RUN <<EOT
  set -e
  dnf -y install eigen3-devel gmp gmp-devel mpfr mpfr-devel
  curl -O https://archives.boost.io/release/1.72.0/source/boost_1_72_0.tar.bz2
  tar -xf boost_1_72_0.tar.bz2
  curl -L -O --max-redirs 1 https://github.com/CGAL/cgal/releases/download/v6.0.3/CGAL-6.0.3.tar.xz
  tar -xf CGAL-6.0.3.tar.xz
EOT

WORKDIR /tmp/boost_1_72_0

RUN <<EOT
  set -e
  ./bootstrap.sh --prefix=/usr/local
  ./b2 headers
  ./b2 install
EOT

WORKDIR /tmp/CGAL-6.0.3

RUN <<EOT
  set -e
  cmake .
  make install
EOT

WORKDIR /app

COPY cli/ cli/
COPY include/shrink_wrap/ include/shrink_wrap/
COPY python/ python/
COPY src/ src/
COPY CMakeLists.txt CMakeLists.txt

RUN <<EOT
  set -e
  # https://github.com/pypa/hatch/issues/2193
  /opt/python/cp39-cp39/bin/python -m pip install hatch "virtualenv<21" --no-cache-dir
  cmake -S . -B build
  cmake --build build
  cmake --install build --prefix /app/python/src/shrink_wrap
EOT

WORKDIR /app/python

RUN <<EOT
  set -e
  /opt/python/cp39-cp39/bin/hatch build -t wheel
  auditwheel repair dist/*.whl -w /wheelhouse
EOT

FROM scratch
COPY --from=build /wheelhouse /
