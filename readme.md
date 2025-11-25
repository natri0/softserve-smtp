# SMTP server project

This project is a fully-functional SMTP server that handles email transmission protocols. Built with modern C++ practices, it features a multi-threaded architecture, database integration, and comprehensive logging capabilities.

## Technologies used

- **C++23**
- **Boost libraries**
- **OpenSSL**
- **CMake**

## 📁 Project Structure

```
softserve-smtp/
├── server/              # Main SMTP server implementation
├── client/              # SMTP client
├── SMTP/                # SMTP protocol implementation
├── networking/          # Network layer and SSL support
├── db/                  # Database integration
├── logger/              # Logging system
├── utils/               # ?
├── 3rdparty/            # ?
├── cmake/               # ?
└── config.example.json  # Configuration template
```

## How to run

```bash
git clone --recursive https://github.com/natri0/softserve-smtp
cd softserve-smtp
mkdir build
cd build
cmake -G "Ninja" ..
ninja smtp_server
./smtp_server
```

## 🐧 Linux Service Deployment

To run the SMTP server as a background service on Linux:

```bash
chmod +x install-service.sh
sudo ./install-service.sh
```

Once installed, you can manage the service using standard systemd commands:

```bash
sudo systemctl start/stop/restart/status smtp_server
```