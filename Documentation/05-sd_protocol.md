<img src="https://github.com/bjornbrodtkorb/strawberry/blob/master/Graphics/logo.png" width="100">

# SD Protocol

This is intented to be a short summary of the most important concepts.

# Speed grades

Two speed grades are avaliable

- Default speed - 25 MHz which gives 12.5 MB/s
- High speed - 50 MHz which gives 25 MB/s

# Packet format

There exists three different packet formats in the SD card protocol

- Command
- Response
- Data

Command packets are tokens that starts an operation. A command token is 48-bits, starting with a start bit and ending with a stop bit. Is also includes a 7-bit CRC checksum at the end. Response packets are sent from the SD card to the host as a responce to the previous command. Its length is either 48-bits or 136-bits. Data on the command line are transmitted MSB first. Finally we have the data packets, whcih can be either usual data (8-bits) or wide data (SD register). Wide data is transmitted MSB first and usual data is transmitted LSByte first and MSBit first.

# Registers

- CID - card ID number
- RCA - relative card address; suggested by the card, and approved by the host
- CSD - card spesific data; contains info about the cards operation conditions
- SCR - SD configuration register; contains info about the SD cards special features
- OCR - operating condition register
- SSD - SD status; contains info about the SD cards proprietary features
- CSR - card status

<img src="https://github.com/bjornbrodtkorb/BlackOS/blob/master/BlackOS%20Graphics/sd_arcitecture.png" width="400">

## States

- Inactive state
- Idle state
- Ready state
- Identifiaction state
- Stand-by state
- Transfer state
- Sending data state
- Receiving data state
- Programming state
- Disconnected state

The card is in identification mode in the three first states, while in data transfer mode in the reset. 

## Initialization and enumeration

The first command to be sent is the CMD0. This brings all the cards back to the idle state. In idle state all the command lines are treated as input, and the cards are waiting for the first command. The cards is initialized with RCA = 0x0000 and bus frequency 400 kHz.

After CMD0 has been sent the card does not know whether the operating voltage is supported. Therefore a command CMD8 is following the CMD0. The card will check if the current operating condition is supported. The supplied voltage can be found in the VHS field. If the card supports the voltage sent as the argument the card, it echoes back the message.This command must be sent prior to ACMD41.

After CMD8 has been sent the host will send ACMD41 to the card. The argument is the VDD range desired by the host. If the card cannot operate under these comditions the card will disconect itself and go into inactive state. The host should set a timeout on 1 second. 

After ACMD41 has succeeded the card is in ready state. 


<img src="https://github.com/bjornbrodtkorb/BlackOS/blob/master/BlackOS%20Graphics/card_identification_mode_states.png" width="800">

<img src="https://github.com/bjornbrodtkorb/BlackOS/blob/master/BlackOS%20Graphics/identification_flow.png" width="800">

