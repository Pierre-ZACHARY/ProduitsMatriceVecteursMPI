FROM ubuntu:20.04


ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get -y update
RUN apt-get install -y g++ openmpi-bin libopenmpi-dev libcppunit-dev
RUN apt-get install sudo cmake -y

WORKDIR /app

COPY . .

RUN cmake -S . -B build
RUN cmake --build build

CMD ["mpirun","--allow-run-as-root", "-np", "1", "/app/build/LeaderFollower"]
#CMD ["./buid/tests/hello_test"]