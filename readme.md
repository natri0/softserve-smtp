# SMTP server project

TODO: fill in readme once we start actually writing the project

## Technologies used

- C++23
- Boost libraries
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

### Background process
To install the server as a background service (daemon) on Linux and start it automatically:
```bash
chmod +x install-service.sh
sudo ./install-service.sh
```
This will configure the systemd service and start the server immediately.

### Managing the Service Once installed
You can control the daemon using standard system commands:

- Check status and logs:
```Bash
sudo systemctl status smtp_server
```
- Stop the server
```Bash
sudo systemctl stop smtp_server
```

- Restart the server
```Bash
sudo systemctl restart smtp_server
```