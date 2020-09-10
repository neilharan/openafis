# This program is part of OpenAFIS (https://github.com/neilharan/openafis).
# Extracts minutiae from supplied tif raster files (for example the FVC example data).
#
# Install numpy, cffi and pillow.
# Copy sgfpamx, sgfplib & sgwsqlib (dll's or so's) from SecuGen FDx Pro SDK into current directory.

import argparse
import numpy
from cffi import FFI
from PIL import Image

# Following SDK constants and function rototypes from sgfplib.h (part of SecuGen FDx Pro SDK)...
ffi = FFI()
ffi.cdef(
    """
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef void* HSGFPM;

enum SGFDxDeviceName
{
   SG_DEV_UNKNOWN = 0,
   SG_DEV_FDP02 = 0x01,
   SG_DEV_FDU02 = 0x03,
   SG_DEV_FDU03 = 0x04,       // Hamster Plus
   SG_DEV_FDU04 = 0x05,       // Hamster IV
   SG_DEV_FDU05 = 0x06,       // HU20
   SG_DEV_FDU06 = 0x07,       // UPx
   SG_DEV_FDU07 = 0x08,       // U10
   SG_DEV_FDU07A = 0x09,	  // U10-AP(A)
   SG_DEV_FDU08 = 0x0A,	      // U20A
   SG_DEV_FDU08P =0x0B,	      // U20-AP, reserved
   SG_DEV_FDU06P = 0x0C,      // UPx-P
   SG_DEV_FDUSDA = 0x0D,      // U20-ASF-BT (SPP/Serial)
   SG_DEV_FDUSDA_BLE = 0x0E,  // U20-ASF-BT (BLE)
   SG_DEV_FDU08X = 0x0F,      // U20-ASFX (USB), reserved
   SG_DEV_FDU09 = 0x10,       // U30, reserved
   SG_DEV_FDU08A = 0x11,	  // U20-AP(A)
   SG_DEV_FDU09A = 0x12,	  // U30-AP(A)
   SG_DEV_AUTO = 0xFF,
};

enum SGFDxTemplateFormat 
{
   TEMPLATE_FORMAT_ANSI378 = 0x0100,
   TEMPLATE_FORMAT_SG400   = 0x0200,
   TEMPLATE_FORMAT_ISO19794 = 0x0300,
   TEMPLATE_FORMAT_ISO19794_COMPACT = 0x0400,
};

enum SGImpressionType 
{
   SG_IMPTYPE_LP =	0x00,		// Live-scan plain
   SG_IMPTYPE_LR =	0x01,		// Live-scan rolled
   SG_IMPTYPE_NP =	0x02,		// Nonlive-scan plain
   SG_IMPTYPE_NR =	0x03,		// Nonlive-scan rolled
};

enum SGFingerPosition
{ 
   SG_FINGPOS_UK = 0x00,		// Unknown finger
   SG_FINGPOS_RT = 0x01,		// Right thumb
   SG_FINGPOS_RI = 0x02,		// Right index finger
   SG_FINGPOS_RM = 0x03,		// Right middle finger
   SG_FINGPOS_RR = 0x04,		// Right ring finger
   SG_FINGPOS_RL = 0x05,		// Right little finger
   SG_FINGPOS_LT = 0x06,		// Left thumb
   SG_FINGPOS_LI = 0x07,		// Left index finger
   SG_FINGPOS_LM = 0x08,		// Left middle finger
   SG_FINGPOS_LR = 0x09,		// Left ring finger
   SG_FINGPOS_LL = 0x0A,		// Left little finger
};

typedef struct tagSGFingerInfo {
    WORD FingerNumber;           // FingerNumber. 
    WORD ViewNumber;             // Sample number
    WORD ImpressionType;         // impression type. Should be 0
    WORD ImageQuality;           // Image quality
} SGFingerInfo;

DWORD SGFPM_Create(HSGFPM* phFpm);
DWORD SGFPM_Terminate(HSGFPM hFpm);
DWORD SGFPM_Init(HSGFPM hFpm, DWORD devName);
DWORD SGFPM_SetTemplateFormat(HSGFPM hFpm, WORD format);
DWORD SGFPM_GetMaxTemplateSize(HSGFPM hFpm, DWORD* size);
DWORD SGFPM_CreateTemplate(HSGFPM hFpm, SGFingerInfo* fpInfo, BYTE* rawImage, BYTE* minTemplate);
DWORD SGFPM_GetTemplateSize(HSGFPM hFpm, BYTE* minTemplate, DWORD* size);
"""
)

parser = argparse.ArgumentParser(description='Extract minutiae from a tif raster file into an ISO 19794-2:2005 template file')
parser.add_argument('input_file', type=str, help='tif raster file to extract from')
parser.add_argument('output_file', type=str, help='ISO output file')
args = parser.parse_args()

input_file = args.input_file

img = Image.open(input_file)
raw = numpy.array(img)

lib = ffi.dlopen('sgfplib')
fpm = ffi.new("HSGFPM*")
r = lib.SGFPM_Create(fpm)
if r != 0:
    raise Exception("SGFPM_Create() returned {}".format(r))

r = lib.SGFPM_Init(fpm[0], lib.SG_DEV_FDU05)
if r != 0:
    raise Exception("SGFPM_Init() returned {}".format(r))

r = lib.SGFPM_SetTemplateFormat(fpm[0], lib.TEMPLATE_FORMAT_ISO19794)
if r != 0:
    raise Exception("SGFPM_SetTemplateFormat() returned {}".format(r))

max_size = ffi.new("DWORD*")
r = lib.SGFPM_GetMaxTemplateSize(fpm[0], max_size)
if r != 0:
    raise Exception("SGFPM_GetMaxTemplateSize() returned {}".format(r))

fp_info = ffi.new(
    "SGFingerInfo*", {"FingerNumber": lib.SG_FINGPOS_UK, "ViewNumber": 0, "ImpressionType": lib.SG_IMPTYPE_LP, "ImageQuality": 0}
)
raw_image = ffi.cast('BYTE*', raw.ctypes.data)
t = ffi.new("BYTE[]", max_size[0])
r = lib.SGFPM_CreateTemplate(fpm[0], fp_info, raw_image, t)
if r != 0:
    raise Exception("SGFPM_CreateTemplate() returned {}".format(r))

size = ffi.new("DWORD*")
r = lib.SGFPM_GetTemplateSize(fpm[0], t, size)
if r != 0:
    raise Exception("SGFPM_GetTemplateSize() returned {}".format(r))

data = ffi.buffer(t)[0 : size[0]]
lib.SGFPM_Terminate(fpm[0])

of = open(args.output_file, 'wb')
of.write(data)
of.close()
