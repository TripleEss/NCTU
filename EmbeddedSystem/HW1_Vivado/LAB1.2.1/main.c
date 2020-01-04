#include "xparameters.h"
#include "xgpio.h"

int main (void)
{

    XGpio dip, push, led;
	int i, psb_check, dip_check;

	//Initialize switches
    XGpio_Initialize(&dip, XPAR_SW_8BIT_DEVICE_ID);
	//Set switches GPIO as input direction
	XGpio_SetDataDirection(&dip, 1, 0xffffffff);

	//Initialize buttons
	XGpio_Initialize(&push, XPAR_BTN_5BIT_DEVICE_ID);
	//Set buttons GPIO as input direction
	XGpio_SetDataDirection(&push, 1, 0xffffffff);

	//Initialize LEDs
	XGpio_Initialize(&led, XPAR_LED_8BIT_DEVICE_ID);
	//Set LEDs GPIO as output direction
	XGpio_SetDataDirection(&led, 1, 0x0);

	while(1)
	{
		//Read from switches GPIO
		dip_check = XGpio_DiscreteRead(&dip, 1);
		//Write to LEDs
		XGpio_DiscreteWrite(&led, 1, dip_check);

		//Read from buttons GPIO
		if((psb_check = XGpio_DiscreteRead(&push, 1)))
		//Write to UART
		xil_printf("Push Buttons Status %x\r\n", psb_check);

		for(i=0;i<9999999;i++);
	};

	return 0;
}

