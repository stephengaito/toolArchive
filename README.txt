= cook

home :: https://github.com/stephengaito/rGems-cook

== DESCRIPTION:

cook is a rake extension with:

1. configuration file,
1. the ability to retrieve passwords from encrypted configuration files,
1. the ability to create files using Erubis templates,
1. the ablity to interact with both local and remote shells.

Its main file is a traditional rake Rakefile, which has recipe 
commands.  Each recipe is a collection of rake task files and 
associated YAML configuration files, allowing tasks to make use of 
extensive configuration information.  The configuration is built up 
from the various fragments in the conf files assocaited with each set 
of rake tasks.

== FEATURES:

* Based on standard ruby rake 

* Adds the ability to build up configuration from a number of separate
  configuration files overlaying the more specific configuration
  parameters over the more general parameters.

* Allows collections of related rake tasks and configuration files in 
  the form of recipe directories.

* Uses Erubis to create files from templates.

* Uses Greenletters to enable interaction with both local and remote 
  shells.

* Uses GPGme to obtain passwords from GPG encrypted configuration 
  files.

== SYNOPSIS:

  cook --config dev update

or

  cook -c prod update

to cook the update task with the additional dev or prod
configurations.

== REQUIREMENTS:

* rake 
* construct
* erubis 
* greenletters
* gpgme
* highline (for requesting GPGme passphrase from command line)

== INSTALL:

To install the cook ruby gem

  $ gem install cook

== LICENSE:

(The MIT License)

Copyright (c) 2014 Stephen Gaito

Permission is hereby granted, free of charge, to any person obtaining a 
copy of this software and associated documentation files (the 
'Software'), to deal in the Software without restriction, including 
without limitation the rights to use, copy, modify, merge, publish, 
distribute, sublicense, and/or sell copies of the Software, and to 
permit persons to whom the Software is furnished to do so, subject to 
the following conditions:

The above copyright notice and this permission notice shall be included 
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS 
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY 
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
