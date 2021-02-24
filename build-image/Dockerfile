FROM ubuntu:20.04

LABEL Description="Docker for avr-gcc-embedded projects"

RUN apt update && apt install -y make && apt install -y gcc-avr && apt install -y avr-libc

CMD ["/bin/bash"]
