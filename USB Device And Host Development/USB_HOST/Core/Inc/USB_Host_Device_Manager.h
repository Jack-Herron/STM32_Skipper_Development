/*
 * USB_Host_Device_Manager.h
 *
 *  Created on: Jun 28, 2024
 *      Author: Jack Herron
 */

#ifndef CORE_INC_USB_HOST_DEVICE_MANAGER_H_
#define CORE_INC_USB_HOST_DEVICE_MANAGER_H_

// Dependencies

#include <stdint.h>
#include "USB_Host.h"
#include "../../Config/USB_Host_Config.h"

#define USB_Host_Device_Manager___GET_MIN(a,b)											(((a) > (b)) ? (b) : (a))


// Definitions
#define USB_Host_Device_Manager___DEVICE_POOL_SIZE									USB_Host_Config___PORT_DEVICE_LIMIT * USB_Host_Config___NUMBER_OF_HOST_PORTS_USED
#define USB_Host_Device_Manager___PORT_DEVICE_LIMIT									USB_Host_Config___PORT_DEVICE_LIMIT

#define USB_Host_Device_Manager___DEVICE_MAX_IN_ENDPOINTS							0x10
#define USB_Host_Device_Manager___DEVICE_MAX_OUT_ENDPOINTS							0x10

#define USB_Host_Device_Manager___LOW_SPEED_DEVICE									0x00
#define USB_Host_Device_Manager___FULL_SPEED_DEVICE									0x01
#define USB_Host_Device_Manager___HIGH_SPEED_DEVICE									0x02

#define USB_Host_Device_Manager___SETUP_STAGE_IDLE									0x00
#define USB_Host_Device_Manager___SETUP_STAGE_GET_FIRST_EIGHT_DEVICE_DESCRIPTOR 	0x01
#define USB_Host_Device_Manager___SETUP_STAGE_SET_ADDRESS 							0x02
#define USB_Host_Device_Manager___SETUP_STAGE_GET_FULL_DEVICE_DESCRIPTOR			0x03
#define USB_Host_Device_Manager___SETUP_STAGE_GET_SHORT_STRING_DESCRIPTOR 			0x04
#define USB_Host_Device_Manager___SETUP_STAGE_GET_FULL_STRING_DESCRIPTOR 			0x05
#define USB_Host_Device_Manager___SETUP_STAGE_GET_SHORT_CONFIGURATION_DESCRIPTOR 	0x06
#define USB_Host_Device_Manager___SETUP_STAGE_GET_FULL_CONFIGURATION_DESCRIPTOR		0x07
#define USB_Host_Device_Manager___SETUP_STAGE_COMPLETE	 							0x08

#define USB_Host_Device_Manager___STRING_TYPE_LANGUAGE_ID							0x01
#define USB_Host_Device_Manager___STRING_TYPE_MANUFACTURER							0x02
#define USB_Host_Device_Manager___STRING_TYPE_PRODUCT								0x03
#define USB_Host_Device_Manager___STRING_TYPE_SERIAL_NUMBER							0x04

#if USB_Host_Config___GET_STRINGS
	#define USB_Host_Device_Manager___DEVICE_MANUFACTURER_STRING_MAX_LENGTH				USB_Host_Config___DEVICE_MANUFACTURER_STRING_MAX_LENGTH
	#define USB_Host_Device_Manager___DEVICE_PRODUCT_STRING_MAX_LENGTH					USB_Host_Config___DEVICE_PRODUCT_STRING_MAX_LENGTH
	#define USB_Host_Device_Manager___DEVICE_SERIAL_NUMBER_STRING_MAX_LENGTH			USB_Host_Config___DEVICE_SERIAL_NUMBER_STRING_MAX_LENGTH
	#define USB_Host_Device_Manager___DEVICE_LANGUAGE_ID_STRING_MAX_LENGTH				USB_Host_Config___DEVICE_LANGUAGE_ID_STRING_MAX_LENGTH
#else
	#define USB_Host_Device_Manager___DEVICE_MANUFACTURER_STRING_MAX_LENGTH				0x00
    #define USB_Host_Device_Manager___DEVICE_PRODUCT_STRING_MAX_LENGTH					0x00
    #define USB_Host_Device_Manager___DEVICE_SERIAL_NUMBER_STRING_MAX_LENGTH			0x00
    #define USB_Host_Device_Manager___DEVICE_LANGUAGE_ID_STRING_MAX_LENGTH				0x00
#endif


// Structures

typedef struct USB_Host_Device_Manager___Polling_Device_Node{
	uint8_t 															enabled;
	uint8_t																device_Address;
	uint16_t																polling_Target;
	uint16_t 															polling_Counter;
	void 																(*callback)			(uint8_t port_Number , uint8_t device_Address);
	struct USB_Host_Device_Manager___Polling_Device_Node*				next_Node;
	struct USB_Host_Device_Manager___Polling_Device_Node*				previous_Node;
}USB_Host_Device_Manager___Polling_Device_Node_TypeDef;

typedef struct{
	USB_Host_Device_Manager___Polling_Device_Node_TypeDef* 				first_Node;
	USB_Host_Device_Manager___Polling_Device_Node_TypeDef* 				last_Node;
	uint8_t 															number_Of_Nodes;
}USB_Host_Device_Manager___Polling_Device_List_Typedef;

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
	uint16_t*                                       					p_Manufacturer_String;
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
	uint8_t																current_USB_Address;
	uint8_t																current_Configuration;
	uint8_t																connection_Flag;
	uint8_t																is_Connected;
	uint8_t																enumerated_Flag;
	uint8_t 															is_Enumerated;
	uint8_t																is_Root_Device;
	uint8_t                           									is_Low_Speed_Device;
	uint8_t																is_High_Speed_Device;
	uint8_t 															port_Number;
} USB_Host_Device_Manager___Device_Status_TypeDef;

typedef struct {
	int8_t 																(*device_Disconnected_Callback)			(uint8_t port_Number , uint8_t device_Address);
	int8_t 																(*device_Enumerated_Callback)			(uint8_t port_Number , uint8_t device_Address);
} USB_Host_Device_Manager___Device_Callbacks_TypeDef;

typedef struct {
	uint8_t                           									configuration_Descriptor_Buffer			[USB_Host_Config___DEVICE_MAX_NUMBER_OF_CONFIGURATIONS][USB_Host_Config___DEVICE_MAX_CONFIGURATION_DESCRIPTOR_LENGTH];
	uint8_t 															device_Descriptor_Buffer				[USB_Host___DEVICE_DESCRIPTOR_LENGTH];
	uint8_t																manufacturer_String_Descriptor_Buffer	[USB_Host_Device_Manager___DEVICE_MANUFACTURER_STRING_MAX_LENGTH];
	uint8_t																product_String_Descriptor_Buffer		[USB_Host_Device_Manager___DEVICE_PRODUCT_STRING_MAX_LENGTH];
	uint8_t																serial_Number_String_Descriptor_Buffer	[USB_Host_Device_Manager___DEVICE_SERIAL_NUMBER_STRING_MAX_LENGTH];
	uint8_t																language_ID_Descriptor_Buffer			[USB_Host_Device_Manager___DEVICE_LANGUAGE_ID_STRING_MAX_LENGTH];
} USB_Host_Device_Manager___Device_Descriptor_Buffers_TypeDef;

struct USB_Host_Device_Manager___Device_TypeDef{
	uint8_t																is_Allocated;
	USB_Host_Device_Manager___Device_Descriptor_Buffers_TypeDef 		descriptor_Buffers;
	USB_Host_Device_Manager___Device_Callbacks_TypeDef					callbacks;
	USB_Host_Device_Manager___Device_Status_TypeDef						status;
	USB_Host_Device_Manager___In_Endpoint_Status_Typedef				out_Endpoint_Status	[USB_Host_Device_Manager___DEVICE_MAX_IN_ENDPOINTS];
	USB_Host_Device_Manager___Out_Endpoint_Status_Typedef				in_Endpoint_Status	[USB_Host_Device_Manager___DEVICE_MAX_OUT_ENDPOINTS];
	USB_Host_Device_Manager___Device_Descriptors_TypeDef				descriptors;
	USB_Host_Device_Manager___Device_Strings_TypeDef					strings;
	USB_Host_Device_Manager___Polling_Device_Node_TypeDef				polling_Process;
};

typedef struct USB_Host_Device_Manager___Device_TypeDef USB_Host_Device_Manager___Device_TypeDef;

typedef struct {
	uint8_t 															top_Device_Address;
	uint8_t																root_Device_Address;
	uint8_t 															device_Enumerated_Flag;
	uint8_t																device_Connected_Or_Disconnected_Flag;
	uint8_t																is_Port_Open;
}USB_Host_Device_Manager___Port_Status_TypeDef;

typedef struct {
	USB_Host_Device_Manager___Device_TypeDef*							p_Device[USB_Host_Device_Manager___PORT_DEVICE_LIMIT+1];
	uint8_t 															number_Of_Devices_Connected;
	USB_Host_Device_Manager___Port_Status_TypeDef						port_Status;
} USB_Host_Device_Manager___Port_TypeDef;

int8_t 											USB_Host_Device_Manager___Allocate_Device_At_Address_Zero						(uint8_t port_Number, uint8_t device_Speed, uint8_t is_Root_Device);
uint8_t 										USB_Host_Device_Manager___Device_Connection_Flag								(uint8_t port_Number, uint8_t device_Address);
uint8_t 										USB_Host_Device_Manager___Is_Device_Connected									(uint8_t port_Number, uint8_t device_Address);
uint8_t 										USB_Host_Device_Manager___Port_Get_Root_Device_Address							(uint8_t port_Number);
void 											USB_Host_Device_Manager___Device_Disconnected									(uint8_t port_Number, uint8_t device_Address);
uint8_t 										USB_Host_Device_Manager__Port_Is_Device_Connected_Or_Disconnected_Flag			(uint8_t port_Number);
void 											USB_Host_Device_Manager___Port_Clear_Device_Connected_Or_Disconnected_Flag		(uint8_t port_Number);
void 											USB_Host_Device_Manager___Clear_Device_Connection_Flag							(uint8_t port_Number, uint8_t device_Address);
void 											USB_Host_Device_Manager___Port_Remove_Device									(uint8_t port_Number, uint8_t device_Address);
void											USB_Host_Device_Manager___Device_Set_Setup_Stage								(uint8_t port_Number, uint8_t device_Address, uint8_t setup_Stage);
void 											USB_Host_Device_Manager___Device_Set_Out_Endpoint_Max_Packet_Size				(uint8_t port_Number, uint8_t device_Address, uint8_t endpoint_Number, uint16_t max_Packet_Size);
void 											USB_Host_Device_Manager___Device_Set_In_Endpoint_Max_Packet_Size				(uint8_t port_Number, uint8_t device_Address, uint8_t endpoint_Number, uint16_t max_Packet_Size);
uint32_t										USB_Host_Device_Manager___Device_Get_Out_Endpoint_Max_Packet_Size				(uint8_t port_Number, uint8_t device_Address, uint8_t endpoint_Number);
uint32_t 										USB_Host_Device_Manager___Device_Get_In_Endpoint_Max_Packet_Size				(uint8_t port_Number, uint8_t device_Address, uint8_t endpoint_Number);
uint8_t 										USB_Host_Device_Manager___Device_Is_Low_Speed_Device							(uint8_t port_Number, uint8_t device_Address);
void 											USB_Host_Device_Manager___Device_Set_Endpoint_Current_Packet_ID					(uint8_t port_Number, uint8_t device_Address, uint8_t endpoint_Number, uint8_t endpoint_Direction, uint8_t Packet_ID);
uint8_t 										USB_Host_Device_Manager___Device_Get_Endpoint_Current_Packet_ID					(uint8_t port_Number, uint8_t device_Address, uint8_t endpoint_Direction, uint8_t endpoint_Number);
uint8_t* 										USB_Host_Device_Manager___Get_Device_Descriptor_Buffer							(uint8_t port_Number, uint8_t device_Address);
uint8_t 										USB_Host_Device_Manager___Port_Get_Free_Device_Address							(uint8_t port_Number);
void 											USB_Host_Device_Manager___Port_Copy_Device_To_Address							(uint8_t port_Number, uint8_t device_Address, uint8_t new_Device_Address);
uint8_t 										USB_Host_Device_Manager___Reserve_New_Device_Address							(uint8_t port_Number, uint8_t current_Device_Address);
uint8_t 										USB_Host_Device_Manager___Device_Get_Setup_Stage								(uint8_t port_Number, uint8_t device_Address);
USB_Host___Device_Descriptor_TypeDef 			USB_Host_Device_Manager___Device_Get_Device_Descriptor							(uint8_t port_Number, uint8_t device_Address);
void 											USB_Host_Device_Manager___Device_Change_Current_USB_Address						(uint8_t port_Number, uint8_t current_Device_Address, uint8_t new_Device_Address);
uint8_t* 										USB_Host_Device_Manager___Get_Language_ID_Descriptor_Buffer						(uint8_t port_Number, uint8_t device_Address);
uint8_t* 										USB_Host_Device_Manager___Device_Get_Manufacturer_String_Descriptor_Buffer		(uint8_t port_Number, uint8_t device_Address);
uint8_t* 										USB_Host_Device_Manager___Device_Get_Product_String_Descriptor_Buffer			(uint8_t port_Number, uint8_t device_Address);
uint8_t* 										USB_Host_Device_Manager___Device_Get_Serial_Number_String_Descriptor_Buffer		(uint8_t port_Number, uint8_t device_Address);
uint16_t* 										USB_Host_Device_Manager___Get_Language_ID_List									(uint8_t port_Number, uint8_t device_Address);
uint16_t* 										USB_Host_Device_Manager___Get_Manufacturer_String								(uint8_t port_Number, uint8_t device_Address);
uint16_t*				 						USB_Host_Device_Manager___Get_Product_String									(uint8_t port_Number, uint8_t device_Address);
uint16_t* 										USB_Host_Device_Manager___Get_Serial_Number_String								(uint8_t port_Number, uint8_t device_Address);
uint8_t 										USB_Host_Device_Manager___Get_Serial_Number_String_Length						(uint8_t port_Number, uint8_t device_Address);
uint8_t						 					USB_Host_Device_Manager___Get_Manufacturer_String_Length						(uint8_t port_Number, uint8_t device_Address);
uint8_t 										USB_Host_Device_Manager___Get_Product_String_Length								(uint8_t port_Number, uint8_t device_Address);
uint8_t 										USB_Host_Device_Manager___Get_Language_ID_List_Length							(uint8_t port_Number, uint8_t device_Address);
void 											USB_Host_Device_Manager___Update_String_Length									(uint8_t port_Number, uint8_t device_Address, uint8_t string_Type);
uint16_t 										USB_Host_Device_Manager___Device_Get_Language_ID								(uint8_t port_Number, uint8_t device_Address, uint8_t language_Index);
uint8_t 										USB_Host_Device_Manager___Get_String_Descriptor_Length							(uint8_t port_Number, uint8_t device_Address, uint8_t string_Type);
uint8_t* 										USB_Host_Device_Manager___Get_Configuration_Descriptor_Buffer					(uint8_t port_Number, uint8_t device_Address, uint8_t configuration_Idex);
uint16_t 										USB_Host_Device_Manager___Device_Get_Configuration_Descriptor_Total_Length		(uint8_t port_Number, uint8_t device_Address, uint8_t configuration_Index);
void 											USB_Host_Device_Manager___Device_Update_Configuration_Descriptor				(uint8_t port_Number, uint8_t device_Address, uint8_t configuration_Index);
uint8_t 										USB_Host_Device_Manager___Get_Device_Class										(uint8_t port_Number, uint8_t device_Address);
void 											USB_Host_Device_Manager___Add_Polling_Device									(uint8_t port_Number, uint8_t device_Address, uint16_t polling_Period, void callback(uint8_t port_Number , uint8_t device_Address));
USB_Host___Configuration_Descriptor_TypeDef*	USB_Host_Device_Manager___Device_Get_Configuration_Descriptor					(uint8_t port_Number, uint8_t device_Address, uint8_t configuration_Index);
USB_Host___Interface_Descriptor_TypeDef* 		USB_Host_Device_Manager___Device_Get_Interface_Descriptor						(uint8_t port_Number, uint8_t device_Address, uint8_t configuration_Index, uint8_t interface_Index);
USB_Host___Endpoint_Descriptor_TypeDef* 		USB_Host_Device_Manager___Device_Get_Endpoint_Descriptor						(uint8_t port_Number, uint8_t device_Address, uint8_t configuration_Index, uint8_t interface_Index, uint8_t endpoint_Index);
void 											USB_Host_Device_Manager___Set_Configuration										(uint8_t port_Number, uint8_t device_Address, uint8_t configuration_Index);
uint8_t 										USB_Host_Device_Manager___Is_Port_Open											(uint8_t port_Number);
void 											USB_Host_Device_Manager___Enable_Device											(uint8_t port_Number, uint8_t device_Address);
USB_Host_Device_Manager___Device_TypeDef* 		USB_Host_Device_Manager___Get_Device_Pointer									(uint8_t port_Number, uint8_t device_Address);

#endif /* CORE_INC_USB_HOST_DEVICE_MANAGER_H_ */
