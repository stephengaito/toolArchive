# Creating a pod of podman containerized services

In this simple example I show how to create and run a podman pod of:

- A nginx reverse proxy (in fact in my system this nginx also hosts a 
  number of static Jekyll based websites as well). 

- A gitea git repository manager.

When up and running you will see two different containers (nginx and gitea).

(In fact there will be three running contianers... podman pods always 
contain a very light weight `pause` container which performs some simple 
management tasks). 

The most important files for our purposes are the `pod-services-up` and 
`pod-services-down` bash scripts which use podman to:

- create the pod,
- create each service container,
- run the pod,

and eventually:

- stop and remove the pod.

I will let you read these bash scripts... however the important points to 
note in the `pod-services-up` bash script are: 

1. A pod is created using the `podman pod create` command.

2. The pod is given an explicit name using the `--name podder-web` command 
line option. 

3. Each service container in the pod is created using the `podman 
container create` command. 

4. Each service container is associated to its pod by the `--pod 
podder-web` command line option. 

5. Finally the pod is started using the `podman pod start` command.

*All of the other command line options* are *Docker* options, with exactly 
the same meanings. 

For each container the `-v` command line option, maps various 
configuration and/or data directories from the host into the container. 
The ensures that any critical data is kept on the host and is not 
destroyed when the pod and its containers are stopped. 

The `podman pod create` command line option `--publish 0.0.0.0:443:443` 
maps the host port 443 (https) to the 443 (https) port inside each 
container (containers in a pod share all network ports). This https port 
will be used by nginx in the `podder-web-nginx-proxy` container. 

Similarly the `--publish 0.0.0.0:2222:22` option maps the host port 2222 
to the 22 (ssh) port inside each container. This ssh port will be used 
by gitea to allow restricted ssh access to the git repositories. 

See: [install with docker: SSH container 
passthrough](https://docs.gitea.io/en-us/install-with-docker/#ssh-container-passthrough) 
for details. 

Equally importantly is the `proxy_pass http://localhost:3000/;` line in 
NGinx's `gitea.conf` file. This tells NGinx to revers proxy the the 
`localhost` port `3000` used by gitea as its web front-end. 

Since Podman pods share their network ports as far as NGinx is concerned, 
`localhost:3000` is a port it can connect to inside its `localhost`.

Equally importantly, this `pod`s `localhost` *is not accessible* outside 
the pod, and in particular are not visible or correspond to the host's 
network ports of the same numbers. This is part of the linux namespaces 
'magic'. 

