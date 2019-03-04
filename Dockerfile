FROM ubuntu:18.10
MAINTAINER Silvman
USER root

RUN apt update -y &&\
    apt install -y wget cmake gcc make

ENV PROJECT_NAME eeskorka

WORKDIR /opt/$PROJECT_NAME/
ADD . /opt/$PROJECT_NAME/

RUN mkdir build &&\
    cd build &&\
    cmake -DCMAKE_BUILD_TYPE=Release .. &&\
    make

RUN cd .. &&\
    mkdir bin &&\
    mv ./build/$PROJECT_NAME ./bin &&\
    rm -rf build

EXPOSE 80
CMD ["./bin/$PROJECT_NAME", "--port 80", "--config /etc/httpd.conf"]
