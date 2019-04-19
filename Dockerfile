FROM debian:stretch
LABEL maintainer="Vladislav Yarmak <vladislav-ex-src@vm-0.com>"
RUN \
  DEBIAN_FRONTEND=noninteractive \
    apt-get update && apt-get install --assume-yes --no-install-recommends \
      build-essential \
      libevent-dev \
      pkg-config
COPY . /build
WORKDIR /build
RUN [ "bash", "-c", "make -j $[ $(nproc) + 1 ]" ]

FROM debian:stretch-slim
LABEL maintainer="Vladislav Yarmak <vladislav-ex-src@vm-0.com>"
RUN \
  DEBIAN_FRONTEND=noninteractive \
    apt-get update && apt-get install --assume-yes --no-install-recommends \
      libevent-2.0 && apt-get clean && rm -rf /var/lib/apt/lists/*
COPY --from=0 /build/bloom /usr/local/bin
VOLUME /var/lib/bloom
EXPOSE 8889
ENTRYPOINT ["bloom"]
