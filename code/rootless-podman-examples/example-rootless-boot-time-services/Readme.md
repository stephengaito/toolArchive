# Running rootless Podman pod based services at boot time

In this example I will show you how to create podman pods to run services 
at boot time. 

The really tricky bit (or the bit that is less widely discussed) is the 
actual configuration required to run a rootless podman pod at boot-time. 

This is covered in 
[`step2-running-pods-at-boot-time`](step2-running-pods-at-boot-time). 

However, just to be complete, in 
[`step1-create-a-pod`](step1-create-a-pod) I sketch an example of how to 
run a podman pod of a nginx reverse proxy to a gitea instance, both 
running in their own OCI container. 
