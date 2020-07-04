# Setting up namespaces

Using [user 
namespaces](https://en.wikipedia.org/wiki/Linux_namespaces#User_ID_(user)) 
allows you to remap the "users" inside your OCI containers to "users" on 
your host. This is an important tool for ensuring security breaches inside 
your OCI container do not result in an attacker having excess priviledges 
on your host machine. 

For each user on your host, the Linux kernel assigns a whole range of 
userids to the "users" inside your OCI container. 

Each host user who will be using podman in rootless mode, *must* have a 
namespaces entry in the `/etc/subuid` and `/etc/subgid` files on your 
host. 

An example `/etc/subuid` file is:

```
user1:100000:65536
user2:165536:65536
user3:231072:65536
podder:296608:65536
admin:362144:65536
dev:427680:65536
```

Each line details the mapping *from* a user on the host *to* a range of 
userids to be used by the host kernel to correspond to the range of 
user-ids inside a linux (OCI) container. 

Each line consists of the *name* of a user on the host, the *starting host 
user-id* which maps to the (root) user-id `0` inside the container, 
followed by the *size* of the user-id mapping. It is very important that 
the user-ids used inside your container *never exceeds* the *size* of the 
corresponding namespace range associated to the host user running the 
container. In our case no user-id inside an container should be larger 
than `65536`. 

It is very useful if these ranges do not overlap, so note that:

- `100000 + 65536 = 165536`
- `165536 + 65536 = 231072`
- `231072 + 65536 = 296608`
- `296608 + 65536 = 362144`
- `362144 + 65536 = 427680`
- `427680 + 65536 = 493216`

To keep things simple my `/etc/subuid` and `/etc/subgid` files contain 
identical mappings, so an example `/etc/subgid` file is: 

```
user1:100000:65536
user2:165536:65536
user3:231072:65536
podder:296608:65536
admin:362144:65536
dev:427680:65536
```

---

For podman's introduction to the use of namespaces see: 

- [Podman and user 
namespaces](http://opensource.com/article/18/12/podman-and-user-namespaces) 

- [Rootless Podman makes 
sense](https://www.redhat.com/sysadmin/rootless-podman-makes-sense) 

- [Why can't rootless Podman pull my 
image](https://www.redhat.com/sysadmin/rootless-podman) 

The [`podman 
unshare`](http://docs.podman.io/en/latest/markdown/podman-unshare.1.html) 
command can be helpful debugging issues with namespace mappings between 
the inside of the OCI container and your host. 