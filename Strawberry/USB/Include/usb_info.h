// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#ifndef USB_INFO_H
#define USB_INFO_H


//--------------------------------------------------------------------------------------------------//


/*

SPDCONF sets the USB transceiver speed

DATA in DPRAM is accessed in big-endian

CLOCK

When the clock is frozen the only interrupt that can trigger an exception is wake-up USBHS_HSTISR.HWUPI


ALLOCATION OF PIPE
Write the USBHS_DEVEPTCFGx.ALLOC bit


Control transfers

Control transfers consists of minimum two transactions
Setup, data (optional), and Status
Every transaction consists of packets: often Token packet, data packet and handshake packet

The transaction start with a setup translation
Token packet SETUP PID
Device address and endpoint
Device must ACK
0b10000000
0b00000110
0b00000000
0b00000001
0b00000000
0b00000000
0b00010010

1000 0000   0000 0110   0000 0000   0000 0001   0000 0000   0000 0000   0001 0010

Data packet 8 bytes

0000 0000   

usb_h_pipe.x.ctrl.setup

*/


//--------------------------------------------------------------------------------------------------//


#endif