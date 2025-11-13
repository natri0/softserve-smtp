# SMTP server project

TODO: fill in readme once we start actually writing the project

## Technologies used

- C++23
- Boost libraries
- OpenSSL
- Qt6
- CMake
- SQLite

## Project structure

| directory  | usage                                                  |
|------------|--------------------------------------------------------|
| `server`   | the server code                                        |
| `3rdparty` | third-party libraries we wish to include as submodules |
| `client`   | client and UI                                          |
| `logger`   | logger with examples and include                       |

## How to run

```bash
git clone --recursive https://github.com/natri0/softserve-smtp
cd softserve-smtp
mkdir build
cd build
cmake -G Ninja ..
ninja smtp_server
./smtp_server
```

## Updates from UI

Use vcpkg to install boost, qt and openSSL
```bash
vcpkg install boost-lockfree boost-beast
vcpkg install openssl
vcpkg install boost-asio boost-thread
vcpkg install qtbase
```
