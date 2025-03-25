#!/usr/bin/env python3

import sys
import os
import time
import asyncio

from aiocoap import *

async def send_update(update_file, ip):
    ctx = await Context.create_client_context()

    with open(update_file, 'rb') as f:
        payload = f.read()

    request = Message(mtype=CON, code=PUT, payload=payload, uri=f'coap://{ip}/wasm')

    try:
        response = await ctx.request(request).response
    except Exception as e:
        print("Failed to fetch resource:")
        print(e)
    else:
        print("Result: %s\n%r" % (response.code, response.payload))

async def main():
    if len(sys.argv) < 2:
        print("usage: update_wasm.py <pico IP> [update_file]")
        sys.exit(1)

    dir_path = os.path.dirname(os.path.realpath(__file__))
    update_file = os.path.join(dir_path, 'wasm_src/main.aot')

    if len(sys.argv) == 3:
        update_file = sys.argv[2]

    ip = sys.argv[1]
    await send_update(update_file, ip)

if __name__ == "__main__":
    asyncio.run(main())
