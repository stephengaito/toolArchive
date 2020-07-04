# Developing code inside a OCI container

In this example I will show you how a build temporary OCI containers which 
I use to control what libraries my code has access to.

Among a number of other projects, I currently help maintain the
[pdf2htmlEX](https://github.com/pdf2htmlEX/pdf2htmlEX) tool.
My development fork is 
[stephengaito/pdf2htmlEX](https://github.com/stephengaito/pdf2htmlEX). 

As I work, I need to install packages at release versions very different 
from what might be installed on my main machine. 

To do this, I develop my code *inside* an OCI container built by podman. 



