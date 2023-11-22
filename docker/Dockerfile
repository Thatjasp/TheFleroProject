# syntax=docker/dockerfile:1.3-labs
FROM ubuntu/apache2:latest
RUN apt-get update && apt-get install -y build-essential libapache2-mod-fcgid apache2-dev \ 
    libfcgi-dev libboost-all-dev libcgicc-dev libfmt-dev clang libomp-dev libspdlog-dev git \
     vim mysql-client default-libmysqlclient-dev libsoci-dev doxygen gdb
RUN git config --global core.editor "vim"