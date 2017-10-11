# EmPOWER Agent project

The EmPOWER Agent (EMAge for friends) is an integrable piece of software which provides the 'slave' counterpart in the controller-agent communication of EmPOWER. EMAge is technology agnostic: this means that it does not depends on a particular implementation of LTE stack, but can be integrated with any custom software. Just a minimal modification of the stack itself is necessary in order to have the agent working (see documentation for more information).

### Compatibility
This software has been developed and tested for Linux.

### Pre-requisites
In order to successfully build EMAge you need:
* Linux standard build suite (GCC, LD, AR, etc...).
* Pthread library, necessary to handle multithreading.

### Organization of the software
The software comes separated in two parts:
* **The agent library:** this is the logic of the project and provides the abstraction layer used by the LTE stack to communicate with the controller. To include the agent in a project you also need the next component.
* **The protocols library**: this is the syntax of how agent and controller communicate between each others. 

### Build instructions
In order to use EMAge the necessary steps to do are:
* Fork the repository on your account.
* Clone your repository.
* Build the protocols used for network communication by invoking the `make` command inside the proto directory (will be built as a shared library by default).
* Install the protocols library in your system by invoking the `make install` command. This will install both the library and the necessary headers; libraries will be located in `/usr/lib`, headers in `/usr/include/emage`.
* Build the agent by invoking the `make` command inside the agent directory (will be built as shared library by default).
* Install the agent library in your system by invoking the `make install` command. This will install both the library and the necessary headers; libraries will be located in `/usr/lib`, headers in `/usr/include/emage`.
* Include both the libraries when you are embedding the agent in your project, by extending the linking phase with `-lemagent` and `-lemproto`.

### License
The code is released under the Apache License, Version 2.0.
