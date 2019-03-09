FROM ubuntu:18.10
MAINTAINER Silvman
USER root

RUN apt update -y
RUN apt install -y wget cmake g++ make

ENV PROJECT_NAME eeskorka

WORKDIR /opt/$PROJECT_NAME/
ADD . /opt/$PROJECT_NAME/

RUN mkdir build &&\
    cd build &&\
    cmake -DCMAKE_BUILD_TYPE=Release .. &&\
    make

EXPOSE 80
CMD ["/opt/eeskorka/build/eeskorka", "--port", "80", "--config", "/etc/httpd.conf"]
