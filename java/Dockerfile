FROM quay.io/pypa/manylinux_2_28_x86_64 AS build

WORKDIR /tmp

RUN <<EOT
  set -e
  dnf -y install eigen3-devel gmp gmp-devel mpfr mpfr-devel java-21-openjdk
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

WORKDIR /usr/local

RUN <<EOT
  curl -O https://dlcdn.apache.org/maven/maven-3/3.10.0/binaries/apache-maven-3.10.0-bin.tar.gz
  tar -xzvf apache-maven-3.10.0-bin.tar.gz
EOT

ENV PATH="/usr/local/apache-maven-3.10.0/bin:${PATH}"

WORKDIR /app

COPY cli/ cli/
COPY include/shrink_wrap/ include/shrink_wrap/
COPY src/ src/
COPY java/ java/
COPY CMakeLists.txt CMakeLists.txt

RUN <<EOT
  set -e
  cmake -S . -B build
  cmake --build build
  cmake --install build --prefix /app/lib/build
EOT

WORKDIR /app/java

RUN <<EOT
  chmod +x package-binary.sh
  ./package-binary.sh /app/lib/build/bin/shrink_wrap /app/java/src/main/resources/bin/linux-x86_64
  mvn clean package -DskipTests
EOT

FROM scratch
COPY --from=build /app/java/target/*.jar /
