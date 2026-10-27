FROM quay.io/pypa/manylinux_2_34_x86_64 AS build

WORKDIR /tmp

RUN <<EOT
  dnf -y install boost-devel eigen3-devel gmp gmp-devel mpfr mpfr-devel
  curl -L -O --max-redirs 1 https://github.com/CGAL/cgal/releases/download/v6.0.3/CGAL-6.0.3.tar.xz
  tar -xf CGAL-6.0.3.tar.xz
EOT

WORKDIR CGAL-6.0.3

RUN <<EOT
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
  /opt/python/cp39-cp39/bin/python -m pip install hatch --no-cache-dir
  cmake -S . -B build
  cmake --build build --verbose
  cmake --install build --prefix python/src/shrink_wrap
EOT

WORKDIR python

RUN <<EOT
  hatch build -t wheel
EOT

FROM scratch AS output
COPY --from=build /app/python/dist/ /
