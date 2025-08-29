FROM ubuntu:24.04

ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y --no-install-recommends \
    clang lld make ca-certificates curl \
    && rm -rf /var/lib/apt/lists/*

COPY wamrc /usr/local/bin/wamrc
RUN chmod +x /usr/local/bin/wamrc

ENV CC=clang \
    WAMRC=/usr/local/bin/wamrc

ENV BUILD_DIR=/work/build

WORKDIR /work

CMD ["bash","-lc","make -j$(nproc) all"]
