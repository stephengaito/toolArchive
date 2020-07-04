# Initial machine setup

To use Podman in rootless mode, your machine *must* be first setup to allow:

- the use of namespaces

- the use of cgroups *version 2* (hybrid cgoups v1/v2 setups are not 
  sufficient; unfortunately Ubuntu 20.04, out-of-the-box, still comes
  with a hybrid cgroups v1/v2 setup which Podman does not really like).

