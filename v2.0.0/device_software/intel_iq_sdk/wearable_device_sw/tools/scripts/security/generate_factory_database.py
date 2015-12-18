from hashlib import sha256
from ecdsa import SigningKey
from ecdsa import VerifyingKey
import sys
import ConfigParser
import subprocess
import shlex
import sqlite3
import uuid
import argparse

def generate_private_key(cmd):
    return subprocess.Popen(shlex.split(cmd),
                            stdout=subprocess.PIPE, stderr=None).communicate()[0]

def generate_public_key(cmd, private_key):
    p = subprocess.Popen(shlex.split(cmd),
                         stdout=subprocess.PIPE, stdin=subprocess.PIPE)
    out, err = p.communicate(private_key)
    return out

#def ecc_compress(in_data):
    #out_data = bytearray(33)
    #for i in range(32):
        #out_data[i+1] = in_data[i]
    #out_data[0] = 2 + (bytearray(in_data[63])[0] & 1)
    #return out_data

def ecc_compress(in_data):
    return in_data


def main(argv):

    """
        Main procedure to generate C array with keys
    """

    parser = argparse.ArgumentParser(description="Generates SQLite database with"
        " keys and serial number.")
    parser.add_argument('config_file', help="project specific configuration file"
        )
    args = parser.parse_args()

    config = ConfigParser.ConfigParser()
    config.read(args.config_file)
    con = sqlite3.connect(config.get('database', 'filename'))

    while 1:
        serialno = uuid.uuid4().bytes
        oem_factory_token = '%s%s' %(serialno, '\xFF')
        customer_factory_token = '%s%s' %(serialno, '\x00\xFF')

        private_key = generate_private_key(config.get('tools', 'private_key_cmd'))
        public_key = generate_public_key(config.get('tools', 'public_key_cmd'), private_key)
        sk = SigningKey.from_der(private_key)
        pk = VerifyingKey.from_der(public_key)

        embedded_private_key = generate_private_key(config.get('tools', 'private_key_cmd'))
        embedded_public_key = generate_public_key(config.get('tools', 'public_key_cmd'), embedded_private_key)
        embedded_sk = SigningKey.from_der(embedded_private_key)
        embedded_pk = VerifyingKey.from_der(embedded_public_key)

        embedded_private_key_sig = embedded_sk.sign(oem_factory_token, hashfunc=sha256)
        assert embedded_pk.verify(embedded_private_key_sig, oem_factory_token, hashfunc=sha256)

        oem_factory_token_sig = sk.sign(oem_factory_token, hashfunc=sha256)
        assert pk.verify(oem_factory_token_sig, oem_factory_token, hashfunc=sha256)

        customer_factory_token_sig = sk.sign(customer_factory_token, hashfunc=sha256)
        assert pk.verify(customer_factory_token_sig, customer_factory_token, hashfunc=sha256)

        debug_token_sig = sk.sign(serialno, hashfunc=sha256)
        assert pk.verify(debug_token_sig, serialno, hashfunc=sha256)

        public_key_compressed = ecc_compress(pk.to_string())

        with con:
            cur = con.cursor()
            cur.execute(config.get('database', 'create'))
            cur.execute(config.get('database', 'insert'),
                        (sqlite3.Binary(private_key),
                         sqlite3.Binary(public_key),
                         sqlite3.Binary(public_key_compressed),
                         sqlite3.Binary(embedded_private_key),
                         sqlite3.Binary(embedded_public_key),
                         sqlite3.Binary(embedded_sk.to_string()),
                         sqlite3.Binary(serialno),
                         sqlite3.Binary(oem_factory_token_sig),
                         sqlite3.Binary(customer_factory_token_sig),
                         sqlite3.Binary(debug_token_sig)))

if __name__ == "__main__":
    main(sys.argv[1:])
