/*
 * USB_VICE_Host.h
 *
 *  Created on: Dec 25, 2024
 *      Author: jackh
 */

#ifndef INC_USB_VICE_HOST_H_
#define INC_USB_VICE_HOST_H_

#include <stdint.h>

#define USB_VICE_Host___MAX_APPLICATIONS							0x08
#define USB_VICE_Host___MAX_SUBSEQUENT_TRANSFER_ERRORS				0xff
#define USB_VICE_Host___STANDARD_NUMBER_OF_RETRIES					0xff
#define USB_VICE_Host___DYNAMICALLY_ALLOCATE_VICE_DEVICES			false
#define USB_VICE_Host___NUMBER_OF_VICE_INSTANCES					0x10
#define USB_VICE_Host___COMPOSITE_DEVICE_CLASS						0x00
#define USB_VICE_Host___VICE_Descriptor_Length						0x09
#define USB_VICE_Host___REPORT_DESCRIPTOR_BUFFER_SIZE				0x100
#define USB_VICE_Host___SETUP_STAGE_GET_VICE_DESCRIPTOR				0x00
#define USB_VICE_Host___SETUP_STAGE_GET_VICE_REPORT_DESCRIPTOR		0x01
#define USB_VICE_Host___SETUP_STAGE_NOTIFY_APPLICATIONS				0x02
#define USB_VICE_Host___VICE_INTERFACE_CLASS						0x15
#define USB_VICE_Host___COMPOSITE_DEVICE_CLASS						0x00
#define USB_VICE_Host___INTERFACE_CONNECTED_CALLBACK_PARAMETERS 	uint8_t port_Number, uint8_t device_Address, uint8_t interface_Number
#define USB_VICE_Host___INTERFACE_DISCONNECTED_CALLBACK_PARAMETERS 	uint8_t port_Number, uint8_t device_Address, uint8_t interface_Number
typedef struct __attribute__((packed))
{
	uint8_t 	bLength;
	uint8_t 	bDescriptorType;
	uint16_t 	bcdVICE;
	uint8_t 	bCountryCode;
	uint8_t 	bNumDescriptors;
	uint8_t 	bReportDescriptorType;
	uint16_t 	wDescriptorLength;
	uint8_t 	interface_Registered;
} USB_VICE_Host___VICE_Descriptor;

typedef struct
{
	void (*VICE_Interface_Connected_Callback[USB_VICE_Host___MAX_APPLICATIONS])(uint8_t port_Number, uint8_t device_Address, uint8_t interface_Number);
	void (*VICE_Interface_Disconnected_Callback[USB_VICE_Host___MAX_APPLICATIONS])(uint8_t port_Number, uint8_t device_Address, uint8_t interface_Number);
} USB_VICE_Host___Application_Callbacks_TypeDef;

typedef struct
{
	uint8_t 						port_Number;
	uint8_t 						device_Address;
	uint8_t 						interface_Number;
	uint8_t							current_Protocol;
	uint8_t 						setup_Stage;
	uint8_t 						sub_Class;
	uint8_t 						interrupt_In_Endpoint_Number;
	uint8_t							interrupt_In_Endpoint_Interval;
	uint8_t 						interrupt_Out_Endpoint_Number;
	uint8_t							interrupt_Out_Endpoint_Interval;
	USB_VICE_Host___VICE_Descriptor 	VICE_Descriptor;
	uint8_t							VICE_Report_Descriptor_Buffer[USB_VICE_Host___REPORT_DESCRIPTOR_BUFFER_SIZE];
	uint16_t 						subsequent_Transfer_Error_Count;
	void 		(*set_Protocol_Callback)(uint8_t port_Number, uint8_t device_Address, uint8_t interface_Number);
	void 		(*report_Callback)(uint8_t port_Number, uint8_t device_Address, uint8_t interface_Number, uint8_t* report_Buffer, uint16_t report_Length);
} USB_VICE_Host___VICE_Interface_TypeDef;

typedef struct USB_VICE_Host___VICE_Node
{
	uint8_t is_Allocated;
	USB_VICE_Host___VICE_Interface_TypeDef   VICE_Device;
	struct USB_VICE_Host___VICE_Node* 	next_Node;
	struct USB_VICE_Host___VICE_Node* 	previous_Node;
} USB_VICE_Host___VICE_Interface_Node_TypeDef;

typedef struct {
	USB_VICE_Host___VICE_Interface_Node_TypeDef* 	first_Node;
	USB_VICE_Host___VICE_Interface_Node_TypeDef* 	last_Node;
}USB_VICE_Host___VICE_Interface_List_TypeDef;

uint8_t USB_VICE_Host___Is_Device_VICE_Device	(uint8_t port_Number, uint8_t device_Address);
void USB_VICE_Host___Init(uint8_t port_Number);
uint8_t USB_VICE_Host___Add_Interface_Disconnected_Callback(uint8_t port_Number, void (*callback)(uint8_t port_Number, uint8_t device_Address, uint8_t interface_Number));
uint8_t USB_VICE_Host___Add_Interface_Connected_Callback(uint8_t port_Number, void (*callback)(uint8_t port_Number, uint8_t device_Address, uint8_t interface_Number));
uint8_t USB_VICE_Host___Register_Interface(uint8_t port_Number, uint8_t device_Address, uint8_t interface_Number);

#endif /* INC_USB_VICE_HOST_H_ */
