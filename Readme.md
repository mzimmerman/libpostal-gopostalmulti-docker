# Libpostal REST Docker

libpostal (https://github.com/openvenues/libpostal) is a C library for
parsing and normalizing street addresses. Using libpostal requires
compiling a C program that downloads ~2GB of training data.

This Dockerfile automates that compilation and creates a container
with libpostal and [gopostalmulti.c](https://github.com/mzimmerman/gopostalmulti/blob/master/cmd/gopostalcmd.c) which allows for an interaction of a TCP socket using msgpack responses for the best performance.

## Build image and start up service
```
docker build -t gopostalmulti .
docker swarm init
docker service create --publish published=9080,target=8080 --replicas=4 --name gopostalmultiservice gopostalmulti
```

## Need more cores?  No problem ##
```
docker service scale gopostalswarm=4
```
## Docker image taken from ##
See REST API [here](https://github.com/johnlonganecker/libpostal-rest) 

## Feature Requests and Bugs
File a Github issue

## Contributing
Just submit a pull request :D
