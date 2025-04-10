FROM ghcr.io/osgeo/gdal:ubuntu-small-3.10.2 AS builder

# disable interactive frontends
#ENV DEBIAN_FRONTEND=noninteractive 

# Refresh package list & upgrade existing packages 
RUN apt-get -y update && \
  apt-get -y upgrade  && \
  apt-get -y install \
    build-essential && \
  apt-get clean && rm -r /var/cache/ 

# Use this user by default
USER ubuntu

# Environment variables
ENV SOURCE_DIR=/home/ubuntu/src/mmu
ENV INSTALL_DIR=/home/ubuntu/bin

# Copy src to SOURCE_DIR
RUN mkdir -p $SOURCE_DIR
WORKDIR $SOURCE_DIR
COPY --chown=ubuntu:ubuntu . .

# Build, install
RUN echo "building mmu" && \
  make && \
  make install

# Install folder
ENV HOME=/home/ubuntu \
    PATH="$PATH:/home/ubuntu/bin"

#FROM davidfrantz/mmu:latest as mmu

#COPY --chown=docker:docker --from=builder $HOME/bin $HOME/bin

WORKDIR /home/ubuntu

CMD ["mmu"]
