# SMTP server project

TODO: fill in readme once we start actually writing the project

## Technologies used

- C++23
- (we'll probably use more stuff as time goes on)

## Project structure

| directory  | usage                                                  |
|------------|--------------------------------------------------------|
| `server`   | the server code                                        |
| `3rdparty` | third-party libraries we wish to include as submodules |

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
