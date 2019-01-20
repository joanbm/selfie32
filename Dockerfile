# builder image
FROM ubuntu:18.10 AS spikepkbuilder

# specify work directory and RISC-V install directory
ENV TOP /opt
ENV RISCV $TOP/riscv
ENV PATH $PATH:$RISCV/bin

WORKDIR $TOP

# install tools to build spike and pk
RUN apt-get update \
  && apt-get install --no-install-recommends -y make gcc g++ libc6-dev gcc-riscv64-linux-gnu libc6-dev-riscv64-cross device-tree-compiler git ca-certificates \
  && rm -rf /var/lib/apt/lists/*

# get sources from HEAD
# FIXME: this just gets the latest revision from each repository,
#        not the submodules pointed to by riscv-tools
RUN git clone --depth 1 https://github.com/riscv/riscv-tools \
  && cd riscv-tools \
  && git clone --depth 1 https://github.com/riscv/riscv-pk \
  && git clone --depth 1 https://github.com/riscv/riscv-isa-sim \
  && git clone --depth 1 https://github.com/riscv/riscv-fesvr

# set build flags compatible with Ubuntu's riscv64-* build flags,
# otherwise compilation fails with linker errors related to stack protection
ENV CFLAGS "-fstack-protector -fstack-protector-explicit -U_FORTIFY_SOURCE"
ENV CPPFLAGS "-fstack-protector -fstack-protector-explicit -U_FORTIFY_SOURCE"
# speed up make by using multiple cores
ENV MAKEFLAGS -j4

# by default, riscv-tools uses the riscv64-unknown-elf, but Ubuntu's packages are build as riscv64-linux-gnu.
# ATM, this doesn't seem entirely configurable from riscv-tools. so just mass replace all ocurrences to fix it
RUN find riscv-tools -type f -print0 | xargs -0 sed -i 's/riscv64-unknown-elf/riscv64-linux-gnu/g'

# build spike and the proxy kernel
RUN cd riscv-tools && ./build-spike-pk.sh

# test the RISC-V gnu toolchain and spike
#RUN echo '#include <stdio.h>\n int main(void) { printf("Hello world!\\n"); return 0; }' > hello.c \
#  && riscv64-linux-gnu-gcc -o hello hello.c \
#  && spike pk hello \

FROM ubuntu:18.10 AS builder

# specify work directory and RISC-V install directory
ENV TOP /opt
ENV RISCV $TOP/riscv
ENV PATH $PATH:$RISCV/bin

WORKDIR $TOP

# install make/gcc (to build selfie), device-tree-compiler (dep. of spike) and qemu
RUN apt-get update \
  && apt-get install -y --no-install-recommends make gcc libc6-dev qemu-user device-tree-compiler \
  && cp /usr/bin/qemu-riscv64 /usr/bin/qemu-riscv64-tmp \
  && apt-get remove --purge -y qemu-user \
  && mv /usr/bin/qemu-riscv64-tmp /usr/bin/qemu-riscv64 \
  && rm -rf /var/lib/apt/lists/*

# copy spike and pk from builder image
COPY --from=spikepkbuilder $RISCV/ $RISCV/

# add selfie sources to the image
COPY . /opt/selfie/

# specify user work directory
WORKDIR /opt/selfie

# build selfie
RUN make selfie

# default command
CMD /bin/bash