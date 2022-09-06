#!/usr/bin/env python3

import sys
import argparse
from os import geteuid
from socket import inet_aton
from scapy.all import Ether, IP, ICMP, Raw, sendp


banner = '''\
kdo-ping [options] [payload]
    -h, --help           this banner
    -t, --target [IP]    send packet to [IP]
    -i, --iface [iface]  send packet with [iface]
'''


def parse_args():
    if len(sys.argv) < 2:
        print(banner)
        sys.exit(1)

    parser = argparse.ArgumentParser(
        usage = banner,
        add_help = False,
        description = 'Send an ICMP echo-request with the specified payload'
    )

    parser.add_argument('cmd', type = str, default = '')

    parser.add_argument('-i', '--iface',  type = str, default = '')
    parser.add_argument('-t', '--target', type = str, default = '')

    args = parser.parse_args()

    if not args.target:
        raise ValueError('No target [IP] provided')

    try:
        inet_aton(args.target)
    except:
        raise ValueError(f'Invalid target IP: {args.host}')

    return args


if __name__ == '__main__':    
    if geteuid() != 0:
        print('[!] Root privileges are necessary to send the crafted packet')
        exit(0)

    try:
        args = parse_args()

        pkt = Ether(type = 2048) / IP(dst = args.target) / \
            ICMP(type = 8) / Raw(load = f'{args.cmd}\x00'.encode())

        if not args.iface:
            sendp(pkt, verbose = False)
        else:
            sendp(pkt, verbose = False, iface = args.iface)

    except Exception as e:
        print(f'{type(e).__name__}: {e}')
