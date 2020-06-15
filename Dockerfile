FROM debian:bullseye-slim

RUN apt update && \
    apt install -y libssl1.1 && \
    rm -rf /var/lib/apt/lists/*

COPY ./build/trojan /root/

WORKDIR /root
ENTRYPOINT ["./trojan", "-c"]
CMD ["config.json"]