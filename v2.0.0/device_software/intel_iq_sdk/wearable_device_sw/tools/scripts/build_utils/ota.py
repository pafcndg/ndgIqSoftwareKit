#!

import ctypes
import sys, os, argparse
import zlib
import struct

class OtaHeader(ctypes.Structure):
    _fields_ = [
        ("magic", ctypes.c_char * 3),
        ("header_version", ctypes.c_byte),
        ("header_length", ctypes.c_int16),
        ("platform", ctypes.c_int16),
        ("crc", ctypes.c_int32),
        ("payload_length", ctypes.c_int32),
        # version of the current package
        ("version", ctypes.c_int32),
        # mininimum version on which it can be applied
        ("min_version", ctypes.c_int32),
        # minimum version of the phone application
        ("app_min_version", ctypes.c_int32)
    ]

class OtaBinary(ctypes.Structure):
    _fields_ = [
        ("magic", ctypes.c_char * 3),
        ("type", ctypes.c_byte),
        ("version", ctypes.c_int32),
        ("offset", ctypes.c_int32),
        ("length", ctypes.c_int32)
    ]

def main(argv):
    output_file = ""

    # default values
    output_file = sys.stdout



    parser = argparse.ArgumentParser(description='tool to generate ota package')
    parser.add_argument('-o','--output_file', help='Output file', required=True)
    parser.add_argument('-p','--procedure', help='Procedure', required=True)
    parser.add_argument('-b','--build_version', help='Build version',default=0)
    parser.add_argument('-m','--build_min_version', help='Minimum version',default=0)
    parser.add_argument('-v','--binary_version', help='Version',default=0)
    parser.add_argument('-a','--app_min_version', help='Application minimum version',default=0)
    parser.add_argument('-c','--crc', help='crc',default=0)
    parser.add_argument('-i','--input_files', help='Input file : -i file1 file2 file3 ...', nargs='*', required=True)
    args = parser.parse_args()

    output_file = args.output_file
    procedure = args.procedure
    build_version= int(args.build_version)
    build_min_version = int(args.build_min_version)
    binary_version = int(args.binary_version)
    app_min_version = int(args.app_min_version)
    input_files = args.input_files


    if procedure != "package":
            print("This procedure type is not supported: " + procedure)
            sys.exit(-1)

    # loop on the input files
    part_indexes = [
             "arc.bin",
             "quark.signed.bin",
             "image.bin",
             "bootloader_quark.bin"]
    parts = [{"magic":"ARC", "type":0},
             {"magic":"QRK", "type":1},
             {"magic":"BLE", "type":2},
             {"magic":"BLD", "type":3}]

    for infile in input_files:
        (name, ext) = os.path.splitext(os.path.basename(infile))
        try:
            part_index = part_indexes.index(name)
        except ValueError:
            print("This file name is not supported: " + infile)
            sys.exit(-1)
        part = parts[part_index]
        part["size"] = os.path.getsize(infile)
        part["header"] = OtaBinary(part["magic"], part["type"], binary_version, 0, part["size"])
        part["data"] = open(infile).read()
        print("Adding file : " + infile)

    # filter the partitions that are not empty
    nonemptyparts = filter(lambda x: x.has_key("size"), parts)

    # build the full package content
    # initialize the package with CRC 0
    otaheader = OtaHeader("OTA", 0x01,
                           ctypes.sizeof(OtaHeader) + ctypes.sizeof(OtaBinary) * len(input_files),
                           0x0100, 0,
                           sum(map(lambda x: len(x["data"]), nonemptyparts)),
                           build_version,
                           build_min_version, app_min_version)

    def Pack(ctype_instance):
        buf = ctypes.string_at(ctypes.byref(ctype_instance), ctypes.sizeof(ctype_instance))
        return buf
    package = Pack(otaheader)

    # update the offset in the file
    offset = 0
    for part in nonemptyparts:
        part["header"].offset = offset
        offset += part["size"]
        package += Pack(part["header"])
    for part in nonemptyparts:
        package += part["data"]

    # udpate the CRC
    crc = zlib.crc32(package) & 0xFFFFFFFF
    package = package[:8] + struct.pack("<L", crc) + package[12:]

    out_file = open(output_file, 'wb')
    out_file.write(package)
    out_file.close()

    return 0

if __name__ == "__main__":
    main(sys.argv[1:])

