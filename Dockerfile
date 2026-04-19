# ===== ÉTAGE 1 : Builder =====
FROM ubuntu:22.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    g++ \
    cmake \
    make \
    git \
    librabbitmq-dev \
    libboost-all-dev \
    && rm -rf /var/lib/apt/lists/*

# Compiler SimpleAmqpClient depuis les sources
RUN git clone https://github.com/alanxz/SimpleAmqpClient.git /tmp/SimpleAmqpClient && \
    cd /tmp/SimpleAmqpClient && \
    mkdir build && cd build && \
    cmake .. && \
    make && \
    make install

WORKDIR /app
COPY . .

RUN rm -rf build && mkdir build && cd build && cmake .. && cmake --build .

# ===== ÉTAGE 2 : Runner =====
FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    librabbitmq4 \
    libboost-all-dev \
    && rm -rf /var/lib/apt/lists/*

COPY --from=builder /usr/local/lib/libSimpleAmqpClient* /usr/local/lib/
RUN ldconfig

COPY --from=builder /app/build/worker /app/worker

CMD ["/app/worker"]