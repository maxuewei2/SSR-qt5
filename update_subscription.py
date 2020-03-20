#!/usr/bin/python
# -*- coding: UTF-8 -*-
# 参考 https://github.com/shadowsocksrr/shadowsocksr-libev/blob/master/src/ssrlink.py
import json
import base64
import requests
import os
import psutil
import subprocess
import sys


class SSR:
    def __init__(self, url, socks_port, http_port):
        self.ssr_config_filename = 'ssr_config.json'
        self.ssr_subscription_url = url
        self.ssr_dict = {}
        self.local_address = '127.0.0.1'
        self.local_port = socks_port
        self.local_http_port = http_port
        self.last = ''

    def load_config(self):
        if not os.path.exists(self.ssr_config_filename):
            return False
        try:
            config = json.load(open(self.ssr_config_filename))
            self.ssr_subscription_url = config.get('ssr_subscription_url')
            self.ssr_dict = config.get('ssr_dict')
            self.last = config.get('last')
            self.local_address = config.get('local_address')
            self.local_port = config.get('local_port')
            self.local_http_port = config.get('local_http_port')
            return True
        except Exception:
            return False

    def write_config(self):
        try:
            with open(self.ssr_config_filename, 'w') as f:
                config = {
                    'ssr_subscription_url': self.ssr_subscription_url,
                    'ssr_dict': self.ssr_dict,
                    'last': self.last,
                    'local_address': self.local_address,
                    'local_port': self.local_port,
                    'local_http_port': self.local_http_port
                }
                json.dump(config, f, indent=4)
                return True
        except Exception:
            return False
            
    def to_bytes(self, s):
        if bytes != str:
            if type(s) == str:
                return s.encode('utf-8')
        return s

    def to_str(self, s):
        if bytes != str:
            if type(s) == bytes:
                return s.decode('utf-8')
        return s

    def b64decode(self, data):
        if b':' in data:
            return data
        if len(data) % 4 == 2:
            data += b'=='
        elif len(data) % 4 == 3:
            data += b'='
        return base64.urlsafe_b64decode(data)

    def parse_ssrlink(self, ssr_link):
        link = self.to_bytes(ssr_link[6:])
        link = self.to_str(self.b64decode(link))
        params_dict = {}
        if '/' in link:
            datas = link.split('/', 1)
            link = datas[0]
            param = datas[1]
            pos = param.find('?')
            if pos >= 0:
                param = param[pos + 1:]
            params = param.split('&')
            for param in params:
                part = param.split('=', 1)
                if len(part) == 2:
                    if part[0] in ['obfsparam', 'protoparam']:
                        params_dict[part[0]] = self.to_str(self.b64decode(self.to_bytes(part[1])))
                    else:
                        params_dict[part[0]] = part[1]

        datas = link.split(':')
        if len(datas) == 6:
            host = datas[0]
            port = datas[1]
            protocol = datas[2]
            method = datas[3]
            obfs = datas[4]
            passwd = self.to_str(self.b64decode(self.to_bytes(datas[5])))

            result = {}
            result['server'] = host
            result['server_port'] = port
            result['local_address'] = self.local_address
            result['local_port'] = self.local_port
            result['password'] = passwd
            result['protocol'] = protocol
            result['method'] = method
            result['obfs'] = obfs
            result['timeout'] = 300
            if 'obfsparam' in params_dict:
                result['obfs_param'] = params_dict['obfsparam']
            if 'protoparam' in params_dict:
                result['protocol_param'] = params_dict['protoparam']
            result['name'] = host.split('.')[0].replace('-server', '')
        return result

    def update(self):
        try:
            r = requests.get(self.ssr_subscription_url)
            if r.status_code != 200:
                return 'error with status code {}'.format(r.status_code)
            else:
                text_ = self.b64decode(self.to_bytes(r.text))
                texts_ = text_.split(b'\n')
                self.ssr_dict = {}
                for _ in texts_:
                    one_config = self.parse_ssrlink(_)
                    name = one_config['name']
                    self.ssr_dict[name] = one_config
                if not self.last:
                    self.last = sorted(list(self.ssr_dict.keys()))[0]
                self.write_config()
                return 'success'
        except Exception:
            return 'error'


if __name__ == '__main__':
    cmd= sys.argv[1]
    url = sys.argv[2]
    socks_port = sys.argv[3]
    http_port = sys.argv[4]
    ssr = SSR(url, socks_port, http_port)
    if cmd=='update_config':
        if not ssr.load_config():
            exit(1)
        ssr.ssr_subscription_url=url
        ssr.local_port=socks_port
        ssr.local_http_port=http_port
        if not ssr.write_config():
            exit(1)
    if cmd=='update_subscription':
        result=ssr.update()
        print(result)
        if result.startswith('error'):
            exit(1)
    
