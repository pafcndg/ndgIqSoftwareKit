Provisionning & Manufacturing {#provisionning_and_manufacturing}
=============================

## Manufacturing Specific

Internal version has an issue when connected on a non-Intel network.

Workaround is to add 2 dns entries in host file on local PC.

    127.0.0.1       psi-tools.intel.com
    127.0.0.1       mcg-depot.intel.com

### Provisioning script

Factory sequencer has to call generate_factory_bin.py script with factory data:
serial number, mac addresses...

A script will generate a script and a binary used by factory flashing sequence.

    usage: generate_factory_bin.py [-h] --database DATABASE --factory_sn
                                   FACTORY_SN --hardware_id HARDWARE_ID
                                   --flash_files_dir FLASH_FILES_DIR


### Flashing command

    cflasher -f factory.json -c (project specific) --device-detection-timeout 1000 --set-parameter-value external_factory_script=(project specific and optional)

## How to lock the OTP region ?

A script will soon be provided to allow that

    WARNING: Once done, the OTP region can't be anymore updated, be sure that
    everything is fully validated before using that script.
