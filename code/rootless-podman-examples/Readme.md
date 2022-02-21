# Rootless [Podman](https://podman.io/) examples

Using rootless Podman to run containerized services ...

&nbsp; &nbsp; ... no root or sudo needed!

This repository contains an extended discussion of

- [how to setup and run containerized services using Podman without using 
  root (or even sudo (except to prepare your 
  machine))](example-rootless-boot-time-services). 

- [using a podman container as a light weight KVM to develop 
  code](example-development-in-a-container). 

***Warning:*** I am a Podman newbie... I have only been using it for three 
days... so while what I have to show you might work... there might be 
better ways to do this... 

I have been running containerized services for about 5-10 years, first by 
rolling my own LXC based containers, then (for the last 6 months) using 
Docker. 

Rolling my own LXC containers worked... but was rather harder than I 
wanted. Early this year, I decided to switch to using OCI containers. The 
"obvious" choice, I thought, was Docker.

Unfortunately, Docker takes over your machine in ways I did not like.

While you *can* use Docker and KVM, there are bridge/network/firewall 
issues which more or less stop you using KVM on the same machine as 
Docker. 

To run, Docker takes over and "manages" your firewall (well IPTables 
underneath *your* firewall). This makes it *very* difficult to control 
access to any KVM bridge you might want to use. 

Docker also runs a daemon *as root*. You have NO choice in the matter.

To make it safer, I ran my docker in namespaces mode, but this in turn 
stoped me using docke-out-of-docker....

(Little) Problems upon problems... 

Podman, is easy to use and is similar enough to Docker that, once your 
machine is setup to run Podman in rootless mode, the transition from 
Docker to Podman is almost unnoticed... you really can [`alias 
docker=podman`](https://podman.io/whatis.html).

More importantly, Podman 'manages pods'. For a small shop, you do not need 
to run any complex OCI containter orchestration systems, such as Docker 
swarm, Kubernets or cri-o... you just need Podman pods.

---

These examples are not complete, but they should work. I try, as far as I 
can possibly remember, to provide you with the links that I found helpful 
while I was learning how to do this. 

I use XUbuntu, so *my* discussion may contain Debian/Ubuntu-isms. However 
it should be easy enough to translate what I have to say here, into almost 
any GNU/Linux or even BSD based distribution (YMMV)... 

Throughout my discussion, I use a couple of "standard" users who have *no 
root or sudo priviledges*, as well as an "admin" user who does. You will 
need both types of user and *you will want to keep them as separate 
users*! DO NOT make them the same user! I call my "standard users" 
`podder` (to run a rootless podman service at boot-time) and `dev` (to 
develop code inside a containe), my "admin user" I call `admin`. 

I have broken up my discusion into a number of examples each with a number 
of steps, each with their own Readmes and code. 

Choose your example, and start with step1 ... and go as far as you want, 
or need. 

Enjoy!

All of the code is released using the [MIT license](LICENSE), so you are 
welcome to use this code as you like, and even fork and improve my 
(initial) discussion. (There is, of course, NO WARANTY that what I have to 
say will work in your case. What you do to your machine is your choice... 
be careful! Think before you sudo! ) 

