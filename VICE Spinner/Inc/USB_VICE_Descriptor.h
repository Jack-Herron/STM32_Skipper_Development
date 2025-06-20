/*
 * USB_VICE_Descriptor.h
 *
 *  Created on: Mar 22, 2024
 *      Author: Jack Herron
 */

#ifndef USB_VICE_DESCRIPTOR_H_
#define USB_VICE_DESCRIPTOR_H_



uint16_t sLang[] = 			{0x0409};
uint16_t sManufacturer[] = 	{'V', 'I', 'K', 'I' , 'N', 'G', ' ', 'L', 'A', 'B', 'S'};
uint16_t sProduct[] = 		{'S', 'p' ,'i', 'n', 'n' , 'e', 'r', ' ', '1', '.','0'};
uint16_t sSerialNumber[] = 	{'0', '1', '1', '0'};

USB_String_Index sIndex[] = {
		{0,&sLang[0],1},
		{1,&sManufacturer[0],11},
		{2,&sProduct[0],11},
		{3,&sSerialNumber[0],4}
};

uint8_t Descriptor[] = {
		0x12, 			// Device_Descriptor.bLength
		0x01, 			// Device_Descriptor.bDescriptorType
		0x00, 			// Device_Descriptor.bcdUSB[L]
		0x02, 			// Device_Descriptor.bcdUSB[H]
		0x00, 			// Device_Descriptor.bDeviceClass
		0x00, 			// Device_Descriptor.bDeviceSubClass
		0x00, 			// Device_Descriptor.bDeviceProtocol
		0x40, 			// Device_Descriptor.bMaxPacketSize0
		0x83, 			// Device_Descriptor.idVendor[L]
		0x04, 			// Device_Descriptor.idVendor[H]
		0x40, 			// Device_Descriptor.idProduct[L]
		0x57, 			// Device_Descriptor.idProduct[H]
		0x00, 			// Device_Descriptor.bcdDevice[L]
		0x02, 			// Device_Descriptor.bcdDevice[H]
		0x01, 			// Device_Descriptor.iManufacturer
		0x02, 			// Device_Descriptor.iProduct
		0x03, 			// Device_Descriptor.iSerialNumber
		0x01,  			// Device_Descriptor.bNumConfigurations

		0x09,			//configurations.bLength
		0x02,			//configurations.bDescriptorType
		0x19,			//configurations.wTotalLength[L]
		0x00,			//configurations.wTotalLength[H]
		0x01,			//configurations.bNumInterfaces
		0x01,			//configurations.bConfigurationValue
		0x00,			//configurations.iConfiguration
		0xc0,			//configurations.bmAttributes
		0x32,			//configurations.bMaxPower

		0x09,			//Interface.bLength
		0x04,			//Interface.bDescriptorType
		0x00,			//Interface.bInterfaceNumber
		0x00,			//Interface.bAlternateSetting
		0x01,			//Interface.bNumEndpoints
		0x15,			//Interface.bInterfaceClass
		0x00,			//Interface.bInterfaceSubClass
		0x00,			//Interface.bInterfaceProtocol
		0x00,			//Interface.iInterface

		0x07,			//Endpoint.bLength
		0x05,			//Endpoint.bDescriptorType
		0x81,			//Endpoint.bEndpointAddress
		0x03,			//Endpoint.bmAttributes
		0x08,			//Endpoint.wMaxPacketSize[L]
		0x00,			//Endpoint.wMaxPacketSize[H]
		0x01,			//Endpoint.bInterval
};

uint8_t* configOffset[] = {&Descriptor[18]};



#endif /* USB_VICE_DESCRIPTOR_H_ */
