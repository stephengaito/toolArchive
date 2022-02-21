# How to setup a linux based lms Compilation Farm

Our lms based Compilation farm consists of a code server (which physically 
hosts the source code to be compiled) together with a collection of 
compilation servers (which will collectively do all of the work).

Unlike [distcc](https://github.com/distcc/distcc) (which is an alternate 
way to build a compilation farm), we distribute the code using the Linux 
based [Network File System 
(NFS)](https://en.wikipedia.org/wiki/Network_File_System). Since distcc is 
specialized to [C/C++](https://en.wikipedia.org/wiki/C%2B%2B) code, distcc 
can distribute the source code to a single TCP/IP process on each 
compilation server which "knows" how to perform the compilation.

A typical lms "build", consists of the compilation of "source code" using a 
diverse set of "languages" (such as 
[ConTeXt](https://wiki.contextgarden.net/Main_Page), 
[JoyLoL](https://github.com/stephengaito/joylol) as well as 
[C/C++](https://en.wikipedia.org/wiki/C%2B%2B)). While distcc is excellent 
at compiling "C/C++" using [gcc](https://gcc.gnu.org/) based languages, it 
can *only* compile gcc based languages, which for our purposes is not 
enough.

While we could write custom TCP/IP processes to distribute and receive the 
various types of code in an lms build, we prefer to simply re-use the Linux 
based Network File System (NFS).

Our compilation farm makes the following assumptions:

1. **Security** is sufficient in the **local area network**.
2. Your local area network is behind a NAT-ing firewall (and not directly 
   on the greater internet).
3. The person who sets up the compilation farm has sudo admin privileges.
4. The users (UIDs/GIDs) are identical on all machines.
5. All users can (automatically) ssh between any pair of machines.
6. The user who runs the "main build" also "runs" all "sub-builds".
7. The suite of compilers is identical on all machines.
8. The suite of compilers can be found in the "login path" on all machines.
9. The source code is located in a single subhierarchy of the file system.
10. The souce code subhierarchy is NFS mounted **read-write**.
11. The source code subhierarchy is mounted at the *same* location on all 
    machines.
12. The DMUCS loadavg process is running on all compile machines.
13. The DMUCS request server process is running on a well known machine in 
    the local area network.
14. lms is setup on all compile servers.
15. ConTeXt (including $HOME/texmf) is setup on all compile servers.

On each compile server, the NFS exported source subhierarchy  are located 
at:

```
    /home/compileFarm
```

## Setting up NFS

We will follow Ubuntu's [Setting up NFS how 
to](https://help.ubuntu.com/community/SettingUpNFSHowTo), [NFSv4 how 
to](https://help.ubuntu.com/community/NFSv4Howto) and the rather too simple 
[Network File 
System](https://help.ubuntu.com/stable/serverguide/network-file-system.html)


### On the **code server**

On the code server you need to install `nfs-kernel-server`:

```
    sudo apt install nfs-kernel-server
```

Make the `/home/compileFarm` directory:

```
    sudo mkdir -p /home/compileFarm
```

Add the following line to the `/etc/fstab` file:


```
    <real location of the code subhierarchy> /home/compileFarm none bind 0 0
```

where `<real location of the code subhierarchy>` is the path to your code 
subhierarchy on the code server.

Then mount the compileFarm by typing:

```
    sudo mount /home/compileFarm
```

Ensure that the `/etc/default/nfs-kernel-support` file contains the line:

```
    NEED_SVCGSSD="no"
```

*This turns off the NFSv4 Kerboeros based "user-level" security -- We are 
trusting our local area network and its users! We are also assuming all 
users have the same UID/GID on all compilation servers!*

Add one or other of the two following lines to the `/etc/exports` file:

```
    /home/compileFarm 192.168.0.1(rw,subtree_check) 192.168.0.2(rw,subtree_check)
```

to allow a select few machines on your (private) local area network, OR

```
    /home/compileFarm 192.168.0.0/16(rw,subtree_check)

```

to allow all machines on your (private) local area network. ***Note:** you 
will need to substitute your own IPv4 ip addresses or ip address ranges as 
appropriate for your local area network. **Note:** there are NO spaces in 
each of the export machine/options section. Spaces are used to delimit 
successive machine/option section.*

IF you are using a firewall on the code server, THEN you need to *allow 
in-bound traffic* on the TCP port 2049 (nfs).

Finally to start exporting your NFS mounts type:

```
    sudo service nfs-kernel-server restart
```

### On each **comilation server**

On each of the compilation servers you need to install `nfs-common`:

```
    sudo apt install nfs-common
```

(Note that the code server's installation of `nfs-kernel-server` 
automatically installs the `nfs-common` as well so the code server can also 
be a compilation server).

Make the `/home/compileFarm` directory:

```
    sudo mkdir -p /home/compileFarm
```

Add the following line to the `/etc/fstab` file:


```
    192.168.0.1:/home/compileFarm /home/compileFarm nfs auto 0 0
```

where `<real location of the code subhierarchy>` is the path to your code
subhierarchy on the code server.

Then mount the compileFarm by typing:

```
    sudo mount /home/compileFarm
```

## Setting up lms

On each compile server and for each user who wants to use `lms` to compile 
a project, the user needs to have the `.luaMakeSystem` directory in their 
home directory.

The `.luaMakeSystem` directory must contain the `lms` subdirectory of the 
`luaMakeSystem/luaLibs` directory. You can either make a complete copy, or 
make the `.luaMakeSystem/lms` directory a symbolic link, as desired.

The `.luaMakeSystem` directory must contain a copy of the `lmsDMUCSrun` 
executable appropriate for each compile server. You can build a copy of 
`lmsDMUCSrun` on your machine by using the:

```
   lms -v install
```

command in a git clone of the 
[`luaMakeSystem`](https://github.com/stephengaito/luaMakeSystem) github 
project. You will, of course, need the standard `gcc` or `clang` build 
tools to be installed on your computers.

The `.luaMakeSystem/lmsConfig.lua` file must contain the lines:

```
    lms.useLUV      = true
    lms.dmucsServer = '<DMUCS request server>'
    lms.logDir      = '/home/compileFarm/logs'

    return lms
```

where `<DMUCS request server>` is the name of the host machine on which the 
DMUCS request server is running.

The `/home/compileFarm/logs` directory (as specified as the `lms.logDir` 
above) needs to exist and be writable by any user of the compilation farm.

You need to install the [luarocks](https://luarocks.org/) package 
[`luv`](https://luarocks.org/modules/creationix/luv) in a location in which 
`lms` can find it. (Usually this is correct if you use the luarocks 
associated lua v5.3)

## Running lms builds

To ensure all compile servers know where to find the appropriate source and 
object files, you *must* start an `lms` build in a subdirectory of the 
`/home/compileFarm` directory.

## Monitoring builds

You can use the DMUCS `monitor` command to monitor the changing avaiablity 
of the compile servers in your compilation farm.

You can use the DMUCS `remhost` and `addhost` commands on a given compile 
server to remove or (re)add that server to the list of available servers.

For all three commands you must specify the host name of the DMUCS request 
server using the `-s <requestServerHostName>` command line option.
