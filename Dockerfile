FROM ghcr.io/osgeo/gdal:ubuntu-small-3.10.2 AS builder

# disable interactive frontends
#ENV DEBIAN_FRONTEND=noninteractive 

# Refresh package list & upgrade existing packages 
RUN apt-get -y update && \
  apt-get -y upgrade  && \
  apt-get -y install \
    build-essential && \
  apt-get clean && rm -r /var/cache/ 


# Create a dedicated 'docker' group and user
RUN groupadd docker && \
useradd -m docker -g docker -p docker && \
chmod 0777 /home/docker && \
chgrp docker /usr/local/bin && \
mkdir -p /home/docker/bin && chown docker /home/docker/bin

# Use this user by default
USER docker

# Environment variables
ENV HOME=/home/docker
ENV INSTALL_DIR="$HOME/bin"
ENV PATH="$PATH:$INSTALL_DIR"
ENV SOURCE_DIR=/home/docker/src/mmu

# Copy src to SOURCE_DIR
RUN mkdir -p $SOURCE_DIR
WORKDIR $SOURCE_DIR
COPY --chown=docker:docker . .

# Build, install
RUN echo "building mmu" && \
make && \
make install && \
cd $HOME && \
rm -r $SOURCE_DIR


WORKDIR $HOME

CMD ["mmu"]
