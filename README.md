QtBitCoindRPC
=============

A thread safe asynchronous BitCoind RPC wrapper written in C++ Qt framework.

## Usage

The following RPC calls are supported at the moment

- getBlockCount()
- getBalance(int minconf)
- listAccounts(int minconf)

There are two ways to use the wrapper. 

First, use the QNetworkReply pointer returned by each call to determine when the
request has been completed (e.g. using QNetworkReply::isFinished()), and then
parse the reply by sending it to a wrapper method with the corresponding name.
Here is an example.

Second, make the desired call on the wraper, and then connect the corresponding signal on
the wrapper to a suitable processing slot. Here is an example.

## Use

The easiest way to use the library is just to copy lib/Client.cpp and lib/Client.hpp. 

Alternatively, copy the lib folder as a subdirs project into a preexisting Qt project.

Lastly, build as a static library by simply running qmake.

## Testing

Each supported RPC call has a corresponding test case, although the tests are not very useful at present.

## License
See LICENSE file.

## Author
* Bedeho Mender <<bedeho.mender@gmail.com>>
  Original author, main developer and maintainer
