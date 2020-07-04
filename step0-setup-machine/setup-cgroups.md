# Setting up cgroups

[cgroups](https://en.wikipedia.org/wiki/Cgroups) is one of the "pillars" 
of containerization on Linux. cgroups allow a collection of software to be 
run in its own separate environment ('control group') on a Linux kernel. 

In order to run correctly in rootless mode, Podman, needs to use cgroups 
version 2. To check this `podman` looks at the `/sys/fs/cgroup` directory 
to see if it is a version 2 hierarchy. If not then `podman` assumes it is 
only able to use cgroups v1. In this cgroups v1 mode, `podman`, produces a 
number of errors, and basically does not work. 

For modern Debian/Ubuntu distributions, you can force your system to run 
in cgroups v2 only mode by setting the 
`systemd.unified_cgroup_hierarchy=1` variable ... **at boot time**. 

See: [ArchLinux: Switching to cgroups 
v2](https://wiki.archlinux.org/index.php/Cgroups#Switching_to_cgroups_v2) 

On a machine which uses grub to boot, edit the file `/etc/defaults/grub` 
so that the `GRUB_CMDLINE_LINUX_DEFAULT` environment variable contains 
`systemd.unified_cgroup_hierarchy=1`. When done my `/etc/defaults/grub` 
file had the following line: 

```
GRUB_CMDLINE_LINUX_DEFAULT="quiet splash systemd.unified_cgroup_hierarchy=1"
```

---

```diff
- Attention to detail here is ***critical***.

- You are messing with your machine at ***boot-time***.

- Any errors here *will* leave you with a broken machine! 
```

---

See: [ArchLinux: Kernel 
Parameters](https://wiki.archlinux.org/index.php/Kernel_parameters) 

