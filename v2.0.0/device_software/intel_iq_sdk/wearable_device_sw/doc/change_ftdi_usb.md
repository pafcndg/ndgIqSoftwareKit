How to Change FTDI USB Descriptor on Quark SE for Intel&reg; Curie&trade; Test Board Hardware ? {#change_ftdi_usb}
=======

## What is Here:

To flash Quark SE board with FlashTool, you first need to change two
parameters with FT_Prog (must be done on Windows):
 - iManufacturer : Intel
 - iInterface : FireStarter


For doing that you need to :
 - Install [ftdi driver](http://www.ftdichip.com/Drivers/CDM/CDM%20v2.12.00%20WHQL%20Certified.exe)
 - Install [FT Prog](http://www.ftdichip.com/Support/Utilities/FT_Prog_v3.0.56.245.zip)
 - Instructions to follow : [FT Prog](@ref ft_prog)
