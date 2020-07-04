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
systemctl --user enable pod.service
```

My example `pod.service` file is:

```
see: https://serverfault.com/a/906224

[Unit]
Description=rootless podman pod services -- services-up
After=dbus.socket
Wants=dbus.socket

[Service]
Type=forking
ExecStart=/home/podder/bin/pod-services-up
ExecStop=/home/podder/bin/pod-services-down
RemainAfterExit=yes

[Install]
WantedBy=default.target
```

It is important to note that this systemd/user service:

```
After=dbus.socket
Wants=dbus.socket
```

This means that *this* service will not start until the `podder` user's 
DBus is up and running. 

Equally important is the line:

``` RemainAfterExit=yes ``` This tells systemd that while the 
`pod-services-up` command will exit, the processes it starts will remain 
running and everything is OK. 

---

For more details see:

- [Starting systemd services sharing a session D-Bus on headless 
system](https://serverfault.com/questions/892465/starting-systemd-services-sharing-a-session-d-bus-on-headless-system) 

- [ArchLinx: systemd/User](https://wiki.archlinux.org/index.php/Systemd/User)

- [systemd.service — Service unit 
configuration](https://www.freedesktop.org/software/systemd/man/systemd.service.html) 
