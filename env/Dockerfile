FROM ubuntu:26.04
WORKDIR /
COPY . .
RUN DEBIAN_FRONTEND=noninteractive \
    apt-get update \
    && apt-get install -y python3 \
    && rm -rf /var/lib/apt/lists/*

RUN useradd -ms /bin/bash ariel
USER ariel