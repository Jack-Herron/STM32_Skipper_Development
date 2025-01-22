/*
 * USB_HID_Host.h
 *
 *  Created on: Dec 27, 2024
 *      Author: jackh
 */

#ifndef INC_USB_HID_HOST_H_
#define INC_USB_HID_HOST_H_

#include <stdint.h>
#include <stdbool.h>

#define USB_HID_Host___MAX_APPLICATIONS							0x08
#define USB_HID_Host___MAX_SUBSEQUENT_TRANSFER_ERRORS			0xff
#define USB_HID_Host___STANDARD_NUMBER_OF_RETRIES				0xff
#define USB_HID_Host___DYNAMICALLY_ALLOCATE_HID_DEVICES			false
#define USB_HID_Host___NUMBER_OF_HID_INSTANCES					0x10
#define USB_HID_Host___HID_INTERFACE_CLASS						0x03
#define USB_HID_Host___COMPOSITE_DEVICE_CLASS					0x00
#define USB_HID_Host___HID_Descriptor_Base_Length				0x09
#define USB_HID_Host___PROTOCOL_BOOT							0x00
#define USB_HID_Host___PROTOCOL_REPORT							0x01
#define USB_HID_Host___REPORT_DESCRIPTOR_BUFFER_SIZE			0x100
#define USB_HID_Host___REPORT_BUFFER_SIZE						0x40
#define USB_HID_Host___SETUP_STAGE_GET_HID_DESCRIPTOR			0x00
#define USB_HID_Host___SETUP_STAGE_GET_HID_REPORT_DESCRIPTOR	0x01
#define USB_HID_Host___SETUP_STAGE_NOTIFY_APPLICATIONS			0x02
#define USB_HID_Host___bRequest_SET_PROTOCOL					0x0b
#define USB_HID_Host___DEVICE_TYPE_KEYBOARD						0x01
#define USB_HID_Host___DEVICE_TYPE_MOUSE						0x02
#define USB_HID_Host___DEVICE_TYPE_UNKNOWN						0x00
#define USB_HID_Host___PROTOCOL_BOOT_MODE						0x00
#define USB_HID_Host___PROTOCOL_REPORT_MODE						0x01
#define USB_HID_Host___REPORT_SIZE_BOOT_MOUSE                   0x03
#define USB_HID_Host___REPORT_SIZE_BOOT_KEYBOARD                0x08

typedef struct __attribute__((packed))
{
	uint8_t 	bLength;
	uint8_t 	bDescriptorType;
	uint16_t 	bcdHID;
	uint8_t 	bCountryCode;
	uint8_t 	bNumDescriptors;
	uint8_t 	bReportDescriptorType;
	uint16_t 	wDescriptorLength;
	uint8_t 	interface_Registered;
} USB_HID_Host___HID_Descriptor;

typedef struct
{
	void (*HID_Interface_Connected_Callback[USB_HID_Host___MAX_APPLICATIONS])(uint8_t port_Number, uint8_t device_Address, uint8_t interface_Number);
	void (*HID_Interface_Disconnected_Callback[USB_HID_Host___MAX_APPLICATIONS])(uint8_t port_Number, uint8_t device_Address, uint8_t interface_Number);
} USB_HID_Host___Application_Callbacks_TypeDef;

typedef struct
{
	uint8_t 						port_Number;
	uint8_t 						device_Address;
	uint8_t 						interface_Number;
	uint8_t							current_Protocol;
	uint8_t							boot_Mode_Supported;
	uint8_t 						setup_Stage;
	uint8_t 						device_Type;
	uint8_t 						interrupt_In_Endpoint_Number;
	uint8_t							interrupt_In_Endpoint_Interval;
	uint8_t 						interrupt_Out_Endpoint_Number;
	uint8_t							interrupt_Out_Endpoint_Interval;
	USB_HID_Host___HID_Descriptor 	HID_Descriptor;
	uint8_t							HID_Report_Descriptor_Buffer[USB_HID_Host___REPORT_DESCRIPTOR_BUFFER_SIZE];
	uint8_t	                        HID_Report_Buffer[USB_HID_Host___REPORT_BUFFER_SIZE];
	uint16_t 						HID_IN_Report_Size;
	uint16_t 						subsequent_Transfer_Error_Count;
	void 		(*set_Protocol_Callback)(uint8_t port_Number, uint8_t device_Address, uint8_t interface_Number);
	void 		(*report_Callback)(uint8_t port_Number, uint8_t device_Address, uint8_t interface_Number, uint8_t* report_Buffer, uint16_t report_Length);
} USB_HID_Host___HID_Interface_TypeDef;

typedef struct USB_HID_Host___HID_Node
{
	uint8_t is_Allocated;
	USB_HID_Host___HID_Interface_TypeDef   HID_Device;
	struct USB_HID_Host___HID_Node* 	next_Node;
	struct USB_HID_Host___HID_Node* 	previous_Node;
} USB_HID_Host___HID_Interface_Node_TypeDef;

typedef struct {
	USB_HID_Host___HID_Interface_Node_TypeDef* 	first_Node;
	USB_HID_Host___HID_Interface_Node_TypeDef* 	last_Node;
}USB_HID_Host___HID_Interface_List_TypeDef;

void 	USB_HID_Host___Setup_Device								(uint8_t port_Number, uint8_t device_Address);
uint8_t USB_HID_Host___Is_Device_HID_Device						(uint8_t port_Number, uint8_t device_Address);
void 	USB_HID_Host___Setup_HID_Device							(uint8_t port_Number, uint8_t device_Address);
void 	USB_HID_Host___Init										(uint8_t port_Number);
uint8_t USB_HID_Host___Add_Interface_Connected_Callback			(uint8_t port_Number, void (*callback)(uint8_t port_Number, uint8_t device_Address, uint8_t interface_Number));
uint8_t USB_HID_Host___Add_Interface_Disconnected_Callback		(uint8_t port_Number, void (*callback)(uint8_t port_Number, uint8_t device_Address, uint8_t interface_Number));
void	USB_HID_Host___Remove_Interface_Connected_Callback		(uint8_t port_Number, void (*callback)(uint8_t port_Number, uint8_t device_Address, uint8_t interface_Number));
void 	USB_HID_Host___Remove_Interface_Disconnected_Callback	(uint8_t port_Number, void (*callback)(uint8_t port_Number, uint8_t device_Address, uint8_t interface_Number));
void 	USB_HID_Host___Set_Report_Callback						(uint8_t port_Number, uint8_t device_Address, uint8_t interface_Number, void (*callback)(uint8_t port_Number, uint8_t device_Address, uint8_t interface_Number, uint8_t *report_Buffer, uint16_t report_Length));
uint8_t USB_HID_Host___Interface_Is_Boot_Mode_Supported			(uint8_t port_Number, uint8_t device_Address, uint8_t interface_Number);
uint8_t USB_HID_Host___Interface_Get_Device_Type				(uint8_t port_Number, uint8_t device_Address, uint8_t interface_Number);
void 	USB_HID_Host___Set_Protocol								(uint8_t port_Number, uint8_t device_Address, uint8_t interface_Number, uint8_t protocol, void callback(uint8_t port_Number, uint8_t device_Address, uint8_t interface_Number));
void 	USB_HID_Host___Start_Reporting							(uint8_t port_Number, uint8_t device_Address, uint8_t interface_Number, void callback(uint8_t port_Number, uint8_t device_Address, uint8_t interface_Number, uint8_t *report_Buffer, uint16_t report_Length));
uint8_t USB_HID_Host___Register_Interface						(uint8_t port_Number, uint8_t device_Address, uint8_t interface_Number);
#endif /* INC_USB_HID_HOST_H_ */
