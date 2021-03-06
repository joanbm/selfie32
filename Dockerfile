# import image where spike is already installed
FROM joanbm/riscv32-tools

# install git
RUN apt-get update && apt-get install -y \
    git gcc-multilib \
  && rm -rf /var/lib/apt/lists/*

# add selfie sources to the image
COPY . /opt/selfie/

# specify user work directory
WORKDIR /opt/selfie

# build selfie
RUN make selfie

# default command
CMD /bin/bash
