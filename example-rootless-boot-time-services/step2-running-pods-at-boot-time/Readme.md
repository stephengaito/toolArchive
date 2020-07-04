# Running a rootless podman pod at *boot* *time*

Traditionally to run a service at boot time, you need to add a run level 
script or, on a systemd based machine, a systemd unit file, ***as root***. 

This run level script or systemd unit file will, at boot time, be run 
***as root***. 

In either case, you *could*, *if you remembered*, run the actual service 
itself as some non-priviledged user. 

Rootless Podman works differently.

Internally (rootless) Podman expects to find the user's instance of DBus. 
I suspect `podman` uses DBus to help it locate its various 'running 
parts', pods, containers, etc. 

However, at boot time, DBus is not typically running. DBus only runs for a 
user once they are 'logged in'. 

This means that in order to run a podman pod in rootless mode, your 
designated pod running user, must be able to 'linger'. 

From the `loginctl` `enable-linger` entry:

  Enable/disable user lingering for one or more users. If enabled for a 
  specific user, a (systemd) user manager is spawned for the user at boot 
  and kept around after logouts. This allows users who are not logged in 
  to *run long-running services*. 

---

As `admin` type:

```
  sudo loginctl enable-linger podder
```

(where `podder` is the name of your designated "standard" pod running 
user). 

---

As `podder` you need to setup the systemd user directory:

```
  mkdir -p ~/.config/systemd/user
```

and then copy your `pod.service` file into this systemd user directory:

```
  cp pod.service ~/.config/systmd/user
```

You need to enable this service *at boot time* for the `podder` user:

```

```

