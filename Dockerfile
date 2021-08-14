FROM ubuntu:16.04

RUN apt-get update -y
RUN apt-get install gcc make valgrind -y

ADD . /openu
