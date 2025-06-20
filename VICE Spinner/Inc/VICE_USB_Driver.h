/*
 * VICE_USB_Driver.h
 *
 *  Created on: Mar 3, 2024
 *      Author: Jack Herron
 */

#ifndef VICE_USB_DRIVER_H_
#define VICE_USB_DRIVER_H_

#define get_Bit_Segment(v, msk, pos) ((v&msk) >> pos)

typedef struct
{
  __IO uint32_t ADDR_TX;
  __IO uint32_t COUNT_TX;
  __IO uint32_t ADDR_RX;
  __IO uint32_t COUNT_RX;
} USB_BTable_Typedef;

typedef struct {
    uint8_t bmRequestType;
    uint8_t bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
    uint8_t* data;
    uint8_t dataLength;
} USB_SETUP_Request;

typedef struct {
    uint16_t max_Packet_Size;
    uint16_t num_Packets;
    uint16_t num_Packets_Remaining;
    uint8_t* Data;
    uint16_t transfer_Size;
} USB_QUEUE_Typedef;

typedef struct {
    uint8_t* ptr;
    uint16_t size;
    uint8_t endpoint;
} USB_TX_TypeDef;

typedef struct {
	uint8_t i;
	uint16_t* ptr;
	uint8_t length;
} USB_String_Index;

typedef struct
{
  __IO uint32_t EPR[8];
} USB_Endpoints_TypeDef;

typedef struct
{
  uint8_t enabled;
  uint8_t frame_Rate;
} USB_VICE_Device_Typedef;

#define USB_Endpoints               ((USB_Endpoints_TypeDef *)USB_BASE)

#define USB_BTable                 ((USB_BTable_Typedef *)USB_PMAADDR)

void USB_VICE_Init(USB_VICE_Device_Typedef* device, void callback(uint8_t*, uint16_t));

#endif /* VICE_USB_DRIVER_H_ */
