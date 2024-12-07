## Cpplighty
Cpplighty is basic tool simliar to [acpilight](https://gitlab.com/wavexx/acpilight) but in C++ for setting brightness of backlights via /sys filesystem.

## Usage
```sh
usage: cpplighty [--help] [--version] [--list] [--device DEVICE] [--get]
                 [--get-steps] [--set PERCENT] [--inc PERCENT] [--dec PERCENT] 
```

### Installation
First clone this repo and then do the following.
```sh
make install
```
It is recommended to copy udev rule 50-cpplighty.rules to give write access to backlight devices for any user in the video group to be able to set brightness correctly
to the following directory.
```
/etc/udev/rules.d/
```
