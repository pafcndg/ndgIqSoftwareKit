#!/bin/sh


if [ $# -ne 1 ];
then
    echo "usage: $0 [path/key_name]"
    exit 1
else
    openssl ecparam -genkey -name secp256r1 -noout -out $1_private.der -outform der
    openssl ec -in $1_private.der -inform der -pubout -out $1_public.der -outform der
fi
