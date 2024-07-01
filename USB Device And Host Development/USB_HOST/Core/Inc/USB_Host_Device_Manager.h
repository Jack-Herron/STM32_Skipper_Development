/*
 * USB_Host_Device_Manager.h
 *
 *  Created on: Jun 28, 2024
 *      Author: Jack Herron
 */

#ifndef CORE_INC_USB_HOST_DEVICE_MANAGER_H_
#define CORE_INC_USB_HOST_DEVICE_MANAGER_H_

// Definitions

#define USB_Host_Device_Manager___DEVICE_MAX_IN_ENDPOINTS				0x10
#define USB_Host_Device_Manager___DEVICE_MAX_OUT_ENDPOINTS				0x10

// Structures

typedef struct{
	USB_Host___Endpoint_Descriptor_TypeDef*								p_Endpoint_Descriptor;
	uint16_t															configuration_Buffer_Index;
}USB_Host_Device_Manager___Endpoint_Descriptor_Info_TypeDef;

typedef struct{
	USB_Host___Interface_Descriptor_TypeDef*							p_Interface_Descriptor;
	uint16_t															configuration_Buffer_Index;
	USB_Host_Device_Manager___Endpoint_Descriptor_Info_TypeDef			endpoint[USB_Host_Config___DEVICE_MAX_NUMBER_OF_ENDPOINT_DESCRIPTORS];
}USB_Host_Device_Manager___Interface_Descriptor_Info_TypeDef;

typedef struct{
	USB_Host___Configuration_Descriptor_TypeDef*						p_Configuration_Descriptor;
	uint16_t															configuration_Buffer_Index;
	USB_Host_Device_Manager___Interface_Descriptor_Info_TypeDef			interface[USB_Host_Config___DEVICE_MAX_NUMBER_OF_INTERFACE_DESCRIPTORS];
}USB_Host_Device_Manager___Configuration_Descriptor_Info_TypeDef;

typedef struct{
	uint16_t*                                       					p_Manufaturer_String;
	uint16_t*                                       					p_Product_String;
	uint16_t*                                       					p_Serial_Number_String;
	uint16_t*                                       					p_Language_ID_List;

	uint8_t																manufacturer_String_Length;
	uint8_t																product_String_Length;
	uint8_t																serial_Number_String_Length;
	uint8_t																language_ID_List_Length;
} USB_Host_Device_Manager___Device_Strings_TypeDef;

typedef struct{
	USB_Host_Device_Manager___Configuration_Descriptor_Info_TypeDef		configuration[USB_Host_Config___DEVICE_MAX_NUMBER_OF_CONFIGURATIONS];
	USB_Host___Device_Descriptor_TypeDef*								p_Device_Descriptor;
} USB_Host_Device_Manager___Device_Descriptors_TypeDef;

typedef struct{
	uint8_t																current_Packet_ID;
	uint16_t															max_Packet_Size;
} USB_Host_Device_Manager___Out_Endpoint_Status_Typedef;

typedef struct{
	uint8_t																current_Packet_ID;
	uint16_t															max_Packet_Size;
} USB_Host_Device_Manager___In_Endpoint_Status_Typedef;

typedef struct{
	uint8_t																start_Of_Frame_Polling_Frequency;
	uint8_t 															start_Of_Frame_Polling_Counter;
	uint8_t																current_USB_Address;
	uint8_t																current_Configuration;
	uint8_t 															is_Connected;
	uint8_t 															is_Configured;
	uint8_t                           									is_Low_Speed_Device;
	uint8_t																is_High_Speed_Device;
} USB_Host_Device_Manager___Device_Status_TypeDef;

typedef struct {
	int8_t 																(*device_Polling_Interval_Callback)		(uint8_t port_Number , uint8_t device_Address);
	int8_t 																(*device_Disconnected_Callback)			(uint8_t port_Number , uint8_t device_Address);
	int8_t 																(*device_Enumerated_Callback)			(uint8_t port_Number , uint8_t device_Address);
} USB_Host_Device_Manager___Device_Callbacks_TypeDef;

typedef struct {
	uint8_t																setup_Stage;
	uint8_t 															current_Index;
} USB_Host_Device_Manager___Device_Setup_Status_TypeDef;

typedef struct {
	uint8_t                           									configuration_Descriptor_Buffer			[USB_Host_Config___DEVICE_MAX_NUMBER_OF_CONFIGURATIONS][USB_Host_Config___DEVICE_MAX_CONFIGURATION_DESCRIPTOR_LENGTH];
	uint8_t 															device_Descriptor_Buffer				[USB_Host___DEVICE_DESCRIPTOR_LENGTH];
	uint8_t																manufacturer_String_Descriptor_Buffer	[USB_Host_Config___DEVICE_MANUFACTURER_STRING_MAX_LENGTH];
	uint8_t																product_String_Descriptor_Buffer		[USB_Host_Config___DEVICE_PRODUCT_STRING_MAX_LENGTH];
	uint8_t																serial_Number_String_Descriptor_Buffer	[USB_Host_Config___DEVICE_SERIAL_NUMBER_STRING_MAX_LENGTH];
	uint8_t																language_ID_Descriptor_Buffer			[USB_Host_Config___DEVICE_LANGUAGE_ID_STRING_MAX_LENGTH];
} USB_Host_Device_Manager___Device_Descriptor_Buffers_TypeDef;

struct USB_Host_Control___Device_TypeDef{
	USB_Host_Device_Manager___Device_Descriptor_Buffers_TypeDef 		descriptor_Buffers;
	USB_Host_Device_Manager___Device_Callbacks_TypeDef					callbacks;
	USB_Host_Device_Manager___Device_Status_TypeDef						status;
	USB_Host_Device_Manager___In_Endpoint_Status_Typedef				out_Endpoint_Status	[USB_Host_Device_Manager___DEVICE_MAX_IN_ENDPOINTS];
	USB_Host_Device_Manager___Out_Endpoint_Status_Typedef				in_Endpoint_Status	[USB_Host_Device_Manager___DEVICE_MAX_OUT_ENDPOINTS];
	USB_Host_Device_Manager___Device_Descriptors_TypeDef				descriptors;
	USB_Host_Device_Manager___Device_Strings_TypeDef					strings;
};

#endif /* CORE_INC_USB_HOST_DEVICE_MANAGER_H_ */
