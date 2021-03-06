FROM ubuntu:16.04

# Install
#ENV /home

RUN echo "deb http://apt.llvm.org/trusty/ llvm-toolchain-trusty-3.9 main\ndeb-src http://apt.llvm.org/trusty/ llvm-toolchain-trusty-3.9 main\ndeb http://de.archive.ubuntu.com/ubuntu trusty main universe" >> /etc/apt/sources.list
RUN apt-key adv --keyserver keyserver.ubuntu.com --recv-keys 15CF4D18AF4F7421
RUN apt-get update

#install llvm
RUN apt-get -y --allow-unauthenticated install clang-3.9 clang-3.9-doc libclang-common-3.9-dev libclang-3.9-dev libclang1-3.9 libclang1-3.9-dbg libllvm-3.9-ocaml-dev libllvm3.9 libllvm3.9-dbg lldb-3.9 llvm-3.9 llvm-3.9-dev llvm-3.9-doc llvm-3.9-examples llvm-3.9-runtime clang-format-3.9 python-clang-3.9 libfuzzer-3.9-dev
RUN apt-get -y install cmake
RUN apt-get -y install build-essential
#get skeleton sample
RUN apt-get -y install git

WORKDIR "/home"
RUN git clone https://github.com/VitaNuova/cf-integrity.git
RUN   sed -i "s|-fno-rtti|-fno-rtti -D_GLIBCXX_USE_CXX11_ABI=0 |g" /home/cf-integrity/whitelist/CMakeLists.txt
WORKDIR "/home/cf-integrity"
RUN mkdir build
WORKDIR "/home/cf-integrity/build"
RUN cmake ..
RUN make
WORKDIR "/home/cf-integrity"
