/*
 * VICE_USB_Driver.c
 *
 *  Created on: Mar 3, 2024
 *      Author: Jack Herron
 */
#include <stdint.h>
#include <stdlib.h>
#include <stm32f1xx.h>										// include MCU specific definitions
#include <VICE_USB_Driver.h>
#include <USB_VICE_Descriptor.h>

#define quick_Div_Roof(number, diviser) ((number + (diviser-1)) / diviser)

#define USB_wValue_Descriptor_Type_Pos 		(0x08)
#define USB_DTOG_DATA1						(0x01)
#define USB_DTOG_DATA0						(0x01)
#define USB_RX_Valid_Status					(0x03)
#define USB_TX_Valid_Status					(0x03)
#define USB_RX_Nak_Status					(0x02)
#define USB_TX_Nak_Status					(0x02)
#define USB_RX_Stall_Status					(0x01)
#define USB_TX_Stall_Status					(0x01)
#define USB_RX_Disabled_Status				(0x00)
#define USB_TX_Disabled_Status				(0x00)
#define USB_Num_Endpoints					(0x08)
#define Endpoint_Zero						(0x00)
#define Endpoint_One						(0x01)
#define Endpoint_Two						(0x02)
#define Endpoint_Three						(0x03)
#define Endpoint_Four						(0x04)
#define Endpoint_Five						(0x05)
#define Endpoint_Six						(0x06)
#define Endpoint_Seven						(0x07)
#define Direction_RX						(0x00)
#define Direction_TX						(0x01)
#define USB_PMA_REGISTER_Siz				(0x10)//20

#define USB_Device_Descriptor_Number		(0x01)
#define USB_Configuration_Descriptor_Number	(0x02)
#define USB_String_Descriptor_Number		(0x03)

#define USB_Setup_Get_Status				(0x00)
#define USB_Setup_Clear_Feature				(0x01)
#define USB_Setup_Set_Feature				(0x03)
#define USB_Setup_Set_Address				(0x05)
#define USB_Setup_Get_Descriptor			(0x06)
#define USB_Setup_Set_Descriptor			(0x07)
#define USB_Setup_Get_Configuration			(0x08)
#define USB_Setup_Set_Configuration			(0x09)
#define USB_Set_Frame_Rate					(0x25)
#define USB_Endpoint_Type_Bulk				(0x00)
#define USB_Endpoint_Type_Control			(0x01)
#define USB_Endpoint_Type_Isochronous		(0x02)
#define USB_Endpoint_Type_Interrupt			(0x03)

#define USB_Endpoint_Zero_Buffer_Size		(0x40)
#define USB_Endpoint_One_Buffer_Size		(0x08)

#define USB_GPIO_CR_OUT_50_MHZ 				(0x03)
#define USB_GPIO_CNF_Push_Pull 				(0x02)

#define USB_EPR_Toggle_Bits					(0x7070)

uint8_t EP0Buf[USB_Endpoint_Zero_Buffer_Size];

uint8_t EP1Buf[USB_Endpoint_One_Buffer_Size];

uint8_t* EPBufPtr[8] = {&EP0Buf[0], &EP1Buf[0]};

uint8_t current_Instruction = 0;
uint8_t device_Address;

void (*USB_Callback)(uint8_t*, uint16_t);
USB_VICE_Device_Typedef* VICE_Device;

USB_QUEUE_Typedef queue[USB_Num_Endpoints];

USB_BTable_Typedef * get_BTable(uint8_t pEndPoint){
	return(((USB_BTable_Typedef *) (USB_PMAADDR + (USB_PMA_REGISTER_Siz * pEndPoint))));
}

void USB_VICE_Init(USB_VICE_Device_Typedef* device, void callback(uint8_t*, uint16_t)){
	VICE_Device = device;
	USB_Callback = callback;

	NVIC_SetPriority (USB_HP_IRQn, 0);																				// Set Interrupt Priority
	NVIC_EnableIRQ (USB_HP_CAN1_TX_IRQn);																					// Enable Interrupt
	NVIC_SetPriority (USB_LP_IRQn, 0);																				// Set Interrupt Priority
	NVIC_EnableIRQ (USB_LP_CAN1_RX0_IRQn);																					// Enable Interrupt

	RCC -> APB1ENR |= (RCC_APB1ENR_USBEN);																					// Enable Timer 4 Clock
	USB -> CNTR |= 	(USB_CNTR_ESOFM)   | (USB_CNTR_SOFM)  |
					(USB_CNTR_RESETM)  | (USB_CNTR_SUSPM) |
					(USB_CNTR_WKUPM)   | (USB_CNTR_ERRM)  |
					(USB_CNTR_PMAOVRM) | (USB_CNTR_CTRM);

	// USB 		- VICE USB
	GPIOA -> CRH &= ~(GPIO_CRH_MODE12_Msk) | (GPIO_CRH_CNF12_Msk);																			// clear CFG and Mode for PA4 (EXTI4, VICE interrupt)
	GPIOA -> CRH |= ((USB_GPIO_CR_OUT_50_MHZ<<GPIO_CRH_MODE12_Pos) | (USB_GPIO_CNF_Push_Pull<<GPIO_CRH_CNF12_Pos));																			// set PA4 (EXTI4, VICE interrupt) to Input mode

	GPIOA -> CRH &= ~((GPIO_CRH_MODE11_Msk) | (GPIO_CRH_CNF11_Msk));																			// clear CFG and Mode for PA4 (EXTI4, VICE interrupt)
	GPIOA -> CRH |= ((USB_GPIO_CR_OUT_50_MHZ<<GPIO_CRH_MODE11_Pos) | (USB_GPIO_CNF_Push_Pull<<GPIO_CRH_CNF11_Pos));																			// set PA4 (EXTI4, VICE interrupt) to Input mode

	USB -> CNTR &= ~(USB_CNTR_FRES);
	USB -> CNTR &= ~(USB_CNTR_PDWN);

}

void USB_HP_CAN_TX_IRQHandler(void){

}

void EP_Update_RX_Status(uint8_t pEndPoint, uint8_t status){
	USB_Endpoints->EPR[pEndPoint] &= ~(USB_EP0R_STAT_TX_Msk | USB_EP0R_DTOG_TX_Msk | USB_EP0R_DTOG_RX_Msk);
	USB_Endpoints->EPR[pEndPoint] = ((USB_Endpoints->EPR[pEndPoint] & ~(USB_EP0R_STAT_TX_Msk | USB_EP0R_DTOG_TX_Msk | USB_EP0R_DTOG_RX_Msk)) | (status << USB_EP0R_STAT_RX_Pos));
}

void EP_Update_TX_Status(uint8_t pEndPoint, uint8_t status){
	USB_Endpoints->EPR[pEndPoint] &= ~(USB_EP0R_STAT_RX_Msk | USB_EP0R_DTOG_RX_Msk | USB_EP0R_DTOG_TX_Msk);
	USB_Endpoints->EPR[pEndPoint] = ((USB_Endpoints->EPR[pEndPoint] & ~(USB_EP0R_STAT_RX_Msk | USB_EP0R_DTOG_RX_Msk | USB_EP0R_DTOG_TX_Msk)) | (status << USB_EP0R_STAT_TX_Pos));
}

void PMA_Transfer_Out(uint16_t* pFrom, uint8_t* pTo, uint16_t pNumBytes){
	uint16_t numWords = ((pNumBytes +1) / 2);
	for(uint16_t i = 0; i < numWords; i++){
		((uint16_t*)pTo)[i] = pFrom[i*2];
	}
}

void PMA_Transfer_In(uint8_t* pFrom, uint16_t* pTo, uint16_t pNumBytes){
	uint16_t numWords = ((pNumBytes +1) / 2);
	uint16_t Word_Array[numWords];

	for(uint16_t i = 0; i < pNumBytes; i++){
		((uint8_t*)&Word_Array)[i] = pFrom[i];
	}

	for(uint16_t i = 0; i < numWords; i++){
		pTo[i*2] = Word_Array[i];
	}
}

void USB_Set_TX_Data_Toggle(uint8_t ep, uint8_t Data_Toggle){
	USB_Endpoints->EPR[ep] &= ~(USB_EP0R_STAT_RX_Msk | USB_EP0R_DTOG_RX_Msk | USB_EP0R_STAT_TX_Msk);
	USB_Endpoints->EPR[ep] = ((USB_Endpoints->EPR[ep] & ~(USB_EP0R_STAT_RX_Msk | USB_EP0R_DTOG_RX_Msk | USB_EP0R_STAT_TX_Msk)) | (Data_Toggle << USB_EP0R_DTOG_TX_Pos));
}

void USB_Set_RX_Data_Toggle(uint8_t ep, uint8_t Data_Toggle){
	USB_Endpoints->EPR[ep] &= ~(USB_EP0R_STAT_TX_Msk | USB_EP0R_DTOG_TX_Msk | USB_EP0R_STAT_RX_Msk);
	USB_Endpoints->EPR[ep] = ((USB_Endpoints->EPR[ep] & ~(USB_EP0R_STAT_TX_Msk | USB_EP0R_DTOG_TX_Msk | USB_EP0R_STAT_RX_Msk)) | (Data_Toggle << USB_EP0R_DTOG_RX_Pos));
}

void USB_Transmit(uint8_t pEndpoint, uint8_t* Data_Pointer, uint16_t transfer_Size){
	USB_BTable_Typedef * BTable = get_BTable(pEndpoint);
	BTable -> COUNT_TX = transfer_Size;
	uint16_t* BufferStart = &((uint16_t*)USB_PMAADDR)[(BTable -> ADDR_TX)];
	EP_Update_RX_Status(pEndpoint, USB_RX_Disabled_Status);
	EP_Update_TX_Status(pEndpoint, USB_TX_Valid_Status);
	PMA_Transfer_In(Data_Pointer, BufferStart  ,transfer_Size);
}

void USB_Setup_Transfer(uint8_t pEndpoint, uint8_t* Data_Pointer, uint16_t transfer_Size){
	queue[pEndpoint].Data = Data_Pointer;
	if(transfer_Size > 0){
		queue[pEndpoint].num_Packets = quick_Div_Roof(transfer_Size, queue[pEndpoint].max_Packet_Size);
	}else{
		queue[pEndpoint].num_Packets = 1;
	}

	queue[pEndpoint].transfer_Size = transfer_Size;
	queue[pEndpoint].num_Packets_Remaining = queue[pEndpoint].num_Packets;
	if(queue[pEndpoint].num_Packets_Remaining == 1){
		USB_Transmit(pEndpoint, Data_Pointer, transfer_Size);
	}else{
		USB_Transmit(pEndpoint, Data_Pointer, queue[pEndpoint].max_Packet_Size);
	}
}

void EP_init(uint8_t pEndPoint, uint8_t pType){
	USB_Endpoints->EPR[pEndPoint] = ((pEndPoint << USB_EP0R_EA_Pos) | (pType << USB_EP0R_EP_TYPE_Pos));
	EP_Update_RX_Status(pEndPoint, USB_RX_Valid_Status);
}

void Set_Address(USB_SETUP_Request pSetupPacket){
	USB_Set_TX_Data_Toggle(Endpoint_Zero, 1);
	USB_Setup_Transfer(0,0,0);
	current_Instruction = 1;
	device_Address = pSetupPacket.wValue;
}

void EP_Update_BTable(uint8_t pEndPoint, USB_BTable_Typedef pBTable){
	USB_BTable_Typedef * BTable = get_BTable(pEndPoint);
	BTable -> ADDR_RX = pBTable.ADDR_RX;
	BTable -> COUNT_RX = pBTable.COUNT_RX;
	BTable -> ADDR_TX = pBTable.ADDR_TX;
	BTable -> COUNT_TX = pBTable.COUNT_TX;
}

void Set_Configuration(USB_SETUP_Request pSetupPacket){
	USB_Set_TX_Data_Toggle(Endpoint_Zero, 1);
	queue[Endpoint_Zero].max_Packet_Size = 0x40;
	USB_BTable_Typedef BTable;
	BTable.ADDR_TX = 0x60;
	BTable.COUNT_TX = 0;
	BTable.ADDR_RX = 0x40;
	BTable.COUNT_RX = ((1 << 10) | (1 << 15));
	EP_Update_BTable(Endpoint_One, BTable);
	EP_init(Endpoint_One, USB_Endpoint_Type_Bulk);

	USB_Setup_Transfer(0,0,0);
}

void f_Assemble_String_Descriptor(uint8_t* buffer, USB_String_Index String, uint8_t length)
{
	buffer[0] = length;
	buffer[1] = 0x03;
	for(uint8_t i = 2; i < length; i++)
	{
		buffer[i] = ((uint8_t*)String.ptr)[i-2];
	}
}

void getDescriptor(USB_SETUP_Request pSetupPacket){
	switch(pSetupPacket.wValue >> USB_wValue_Descriptor_Type_Pos){
	case(USB_Device_Descriptor_Number):
	{
		USB_Setup_Transfer(0, &Descriptor[0], pSetupPacket.wLength);
		break;
	}
	case(USB_Configuration_Descriptor_Number):
	{
		USB_Setup_Transfer(0, &Descriptor[18], pSetupPacket.wLength);
		break;
	}
	case(USB_String_Descriptor_Number):
	{
		uint8_t string_Descriptor_Size = ((sIndex[pSetupPacket.wValue & 0xff].length * 2) + 2);
		uint8_t buffer[string_Descriptor_Size];
	 	f_Assemble_String_Descriptor(buffer, sIndex[pSetupPacket.wValue & 0xff], string_Descriptor_Size);
		USB_Setup_Transfer(0, &buffer, pSetupPacket.wLength);
		break;
	}
	default:
		break;
	}

}

void Set_Frame_Rate(USB_SETUP_Request pSetupPacket){
	USB_Set_TX_Data_Toggle(Endpoint_Zero, 1);

	VICE_Device->frame_Rate = pSetupPacket.wValue;

	USB_Setup_Transfer(0,0,0);
}

void handleClassSetupPacket(USB_SETUP_Request pSetupPacket){
	switch(pSetupPacket.bRequest){
	case(USB_Set_Frame_Rate):
		Set_Frame_Rate(pSetupPacket);
		break;
	default:
		break;
	}
}

void handleSetupPacket(USB_SETUP_Request pSetupPacket){
	switch(pSetupPacket.bRequest){
	case(USB_Setup_Get_Status):

		break;
	case(USB_Setup_Clear_Feature):

		break;
	case(USB_Setup_Set_Feature):

		break;
	case(USB_Setup_Set_Address):
		Set_Address(pSetupPacket);
		break;
	case(USB_Setup_Get_Descriptor):
		getDescriptor(pSetupPacket);
		break;
	case(USB_Setup_Set_Descriptor):
		// remove this. it is for testing skipper
		USB_Set_RX_Data_Toggle(Endpoint_Zero, 0);
		EP_Update_RX_Status(Endpoint_Zero, 2);
		break;
	case(USB_Setup_Get_Configuration):

		break;
	case(USB_Setup_Set_Configuration):
		Set_Configuration(pSetupPacket);
		break;
	default:
		handleClassSetupPacket(pSetupPacket);
		break;
	}
}

void USBReceiveHandler(uint8_t pEndpoint){
	//USB_BTable_Typedef * BTable = get_BTable(pEndPoint);
	if(pEndpoint == 0){
		USB_SETUP_Request setupPacket = *((USB_SETUP_Request*)EPBufPtr[0]);
		handleSetupPacket(setupPacket);
	}if(pEndpoint == 1){
		USB_Callback(EPBufPtr[1], 4);
		USB_Set_RX_Data_Toggle(Endpoint_One, 0);
		EP_Update_RX_Status(Endpoint_One, 3);
	}
}

void USB_RX_Success(uint8_t pEndpoint){
	USB_BTable_Typedef * BTable = get_BTable(pEndpoint);
	USB_Endpoints->EPR[pEndpoint] &= ~(USB_EP0R_CTR_RX | USB_EPR_Toggle_Bits);
	PMA_Transfer_Out((&((uint16_t*)USB_PMAADDR)[(BTable -> ADDR_RX)]), EPBufPtr[pEndpoint], ((BTable -> COUNT_RX) & USB_COUNT0_RX_COUNT0_RX_Msk));
	USBReceiveHandler(pEndpoint);
}

void USB_TX_Success(uint8_t pEndpoint){
	queue[pEndpoint].num_Packets_Remaining--;
	USB_Endpoints->EPR[pEndpoint] &= ~(USB_EP0R_CTR_TX);
	if(queue[pEndpoint].num_Packets_Remaining > 1){
		USB_Transmit(pEndpoint, queue[pEndpoint].Data, queue[pEndpoint].max_Packet_Size);
	}else if(queue[pEndpoint].num_Packets_Remaining == 1){
		uint16_t size = queue[pEndpoint].transfer_Size % queue[pEndpoint].max_Packet_Size;
		if(size == 0){size = queue[pEndpoint].max_Packet_Size;};
		USB_Transmit(pEndpoint, queue[pEndpoint].Data, size);
	}else{
		if(pEndpoint == Endpoint_Zero){
			if(current_Instruction == 1){
				current_Instruction = 0;
				USB -> DADDR = (device_Address | USB_DADDR_EF);
			}
			EP_Update_RX_Status(Endpoint_Zero, USB_RX_Valid_Status);
		}
	}
}

void USB_Reset(){
	USB -> DADDR = (0);
	USB -> DADDR |= (USB_DADDR_EF);
	queue[Endpoint_Zero].max_Packet_Size = 0x40;
	USB_BTable_Typedef BTable;
	BTable.ADDR_TX = 0x60;
	BTable.COUNT_TX = 0;
	BTable.ADDR_RX = 0x40;
	BTable.COUNT_RX = ((1 << 10) | (1 << 15));
	EP_Update_BTable(Endpoint_Zero, BTable);
	EP_init(Endpoint_Zero, USB_Endpoint_Type_Control);
}

void USB_LP_CAN_RX0_IRQHandler(void){
	if(USB -> ISTR & (USB_ISTR_SUSP)){
		USB -> ISTR &= ~(USB_ISTR_SUSP);
	}
	if(USB -> ISTR & (USB_ISTR_RESET)){
		USB -> ISTR &= ~(USB_ISTR_RESET);
		USB_Reset();
	}
	if(USB -> ISTR & (USB_ISTR_ESOF)){
		USB -> ISTR &= ~(USB_ISTR_ESOF);
	}
	if(USB -> ISTR & (USB_ISTR_ERR)){
		USB -> ISTR &= ~(USB_ISTR_ERR);
	}
	if(USB -> ISTR & (USB_ISTR_SOF)){
		USB -> ISTR &= ~(USB_ISTR_SOF);
	}
	if(USB -> ISTR & (USB_ISTR_CTR)){
		if(USB -> ISTR & USB_ISTR_DIR){
			USB_RX_Success(get_Bit_Segment(USB->ISTR, USB_ISTR_EP_ID_Msk, USB_ISTR_EP_ID_Pos));
		}else{
			USB_TX_Success(get_Bit_Segment(USB->ISTR, USB_ISTR_EP_ID_Msk, USB_ISTR_EP_ID_Pos));
		}
	}
}
