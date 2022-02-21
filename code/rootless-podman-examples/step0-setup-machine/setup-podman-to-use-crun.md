# Setting up podman to use crun

To actually start a container, the `podman` commands make use of one or 
other of the `runc` or `crun` commands. Both of these commands run a linx 
container on behalf of `podman`. 

However, only the `crun` command is able to use cgroups v2, so it is 
important that you configure `podmad` to use `crun` instead of the 
'default' `runc` command. 

To do this make sure that the `/etc/containers/libpod.conf` file exists 
and contains the line: 

```
runtime = "crun"
```

My `/etc/containers/libpod.conf` file is:

```
# setup podman to use cgroups v2 via crun
runtime = "crun"
```

---

See: [Basic Setup and Use of Podman in a Rootless 
environment](https://github.com/containers/libpod/blob/master/docs/tutorials/rootless_tutorial.md). 
