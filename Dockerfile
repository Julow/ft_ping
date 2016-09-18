FROM debian:wheezy

RUN apt-get update
RUN apt-get install -y make git clang

CMD bash
