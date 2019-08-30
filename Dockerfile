FROM ubuntu:16.04

ARG COMMIT
ENV COMMIT ${COMMIT:-master}
ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update && apt-get install -y \
    autoconf automake build-essential curl git libsnappy-dev libtool pkg-config

RUN git clone https://github.com/openvenues/libpostal -b $COMMIT

COPY ./build_libpostal.sh /libpostal/

WORKDIR /libpostal

RUN ./build_libpostal.sh

RUN apt-get update && apt-get install -y cmake socat

COPY ./build_gopostalcmd.sh /libpostal/
COPY ./gopostalcmd.c /libpostal/
RUN ./build_gopostalcmd.sh

EXPOSE 8080

CMD ./gopostalcmd
