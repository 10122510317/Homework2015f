#include "xparameters.h"
#include "xgpio.h"
#include "xbasic_types.h"
#include "xstatus.h"
#include "xil_io.h"
#include "xutil.h"
#include "XScuTimer.h"
//====================================================
XScuTimer Timer;		/* Cortex A9 SCU Private Timer Instance */

#define ONE_SECOND 333000000 // half of the CPU clock speed

int main (void)
{

   XGpio dip, push, light ,my_input;
   int psb_check, dip_check, dip_check_prev, count, Status;
   int my_flag,instance,light_flag,temp_count,input_check,my_diff_time,distance;



   // PS Timer related definitions
   XScuTimer_Config *ConfigPtr;
   XScuTimer *TimerInstancePtr = &Timer;

   xil_printf("-- Start of the Program --\r\n");

   XGpio_Initialize(&push, XPAR_BTNS_4BIT_DEVICE_ID);
   XGpio_SetDataDirection(&push, 1, 0xffffffff);

   XGpio_Initialize(&light, XPAR_LIGHT_4BIT_DEVICE_ID);
   XGpio_SetDataDirection(&light, 1, 0x00000000);

   XGpio_Initialize(&dip, XPAR_SW_4BIT_DEVICE_ID);
   XGpio_SetDataDirection(&dip, 1, 0xffffffff);

   XGpio_Initialize(&my_input, XPAR_INPUT_4BIT_DEVICE_ID);
   XGpio_SetDataDirection(&my_input, 1, 0xffffffff);

   count = 0;

   my_flag=0;


   // Initialize the timer
   ConfigPtr = XScuTimer_LookupConfig(XPAR_PS7_SCUTIMER_0_DEVICE_ID);
   Status = XScuTimer_CfgInitialize(TimerInstancePtr, ConfigPtr,ConfigPtr -> BaseAddr);
   if (Status != XST_SUCCESS)
   {
	   return XST_FAILURE;
   }





   // Read dip switch values
//   dip_check_prev = XGpio_DiscreteRead(&dip, 1);
   // Load timer with delay in multiple of ONE_SECOND
   XScuTimer_LoadTimer(TimerInstancePtr, ONE_SECOND);
   // Set AutoLoad mode
   XScuTimer_EnableAutoReload(TimerInstancePtr);
   // Start the timer
   XScuTimer_Start(TimerInstancePtr);
   xil_printf("hahaha \r\n");
 /*  	   	   while(1)
   	   	   {
				  // Read push buttons and break the loop if Center button pressed
			psb_check = XGpio_DiscreteRead(&push, 1);
				if(psb_check&0x1)
			   {
					  xil_printf("1Push Button Status %x\r\n", psb_check);
					  break;
			   }
//				xil_printf("1Push Button Status %x\r\n", psb_check);
		   }*/
		   count=0;
		   XScuTimer_LoadTimer(TimerInstancePtr, ONE_SECOND);
		   xil_printf("hihihi \r\n");
		   XGpio_DiscreteWrite(&light,1,0x1);
		   light_flag=0;
		   xil_printf("hehehe \r\n");
		   while(1)
		   {

			   psb_check = XGpio_DiscreteRead(&push, 1);
//			   xil_printf("6Push Button Status %x\r\n", psb_check);
			   if(psb_check==0x8)
			   {
				   dip_check = XGpio_DiscreteRead(&dip, 1);
//				   xil_printf("2Dip Switch, Push Button Status %x,%x\r\n", psb_check,dip_check);
				   Xil_Out32(XPAR_LED_4BIT_BASEADDR,dip_check);
				   // Load timer with delay in multiple of ONE_SECOND
				   instance=dip_check;
/*				   if(dip_check&0x1)instance+=1;
				   if(dip_check&0x100)instance+=2;
				   if(dip_check&0x10000)instance+=4;
				   if(dip_check&0x1000000)instance+=8;*/
				   xil_printf("instance %d \r\n",instance);
				   XScuTimer_LoadTimer(TimerInstancePtr, ONE_SECOND);
				   count=0;
				   temp_count=0;
				   dip_check_prev=dip_check;
				   XGpio_DiscreteWrite(&light,1,0x2);
				   light_flag=1;
			   }

			   if(psb_check==0x4)
			   {
				   dip_check = XGpio_DiscreteRead(&dip, 1);
//				   xil_printf("3Dip Switch, Push Button Status %x,%x\r\n", psb_check,dip_check);
				   Xil_Out32(XPAR_LED_4BIT_BASEADDR,dip_check);
				   distance = dip_check;
/*				   if(dip_check&0x1)distance+=1;
				   if(dip_check&0x100)distance+=2;
				   if(dip_check&0x10000)distance+=4;
				   if(dip_check&0x1000000)distance+=8;*/
				   xil_printf("distance %d \r\n",distance);
			   }

			   input_check = XGpio_DiscreteRead(&my_input, 1);
//			   xil_printf("7input_check %x \r\n",input_check);
			   if(input_check==0xE)
			   {
				   my_flag = 0;
				   if(distance<instance-count)
				   {
					   my_diff_time=instance-count-distance;
					   my_flag=1;
				   }
			   }

			   input_check = XGpio_DiscreteRead(&my_input, 1);
			   psb_check = XGpio_DiscreteRead(&push, 1);
			   if(input_check==0xB||psb_check==0x2)
			   {
//				  xil_printf("4Input Button, Push Button Status %x,%x\r\n",input_check, psb_check);
			      my_flag=0;
			   }

			   if(XScuTimer_IsExpired(TimerInstancePtr)) {
						  // clear status bit
				   count++;
				   xil_printf("pupupu\r\n");
				   XScuTimer_ClearInterruptStatus(TimerInstancePtr);
					  	  // output the count to LED and increment the count
				   Xil_Out32(XPAR_LED_4BIT_BASEADDR,count);
				   xil_printf("5count %d\r\n", count);
				   xil_printf("9pupupu\r\n");

				   if(my_flag==0)
				   {
					   if(count==instance)
					   {
						   count=0;
						   if(light_flag==1)
						   {
							   light_flag=0;
							   XGpio_DiscreteWrite(&light,1,0x1);
						   }
						   else
						   {
							   light_flag=1;
							   XGpio_DiscreteWrite(&light,1,0x2);
						   }
					   }
				   }
				   else
				   {
					   if(count==instance-my_diff_time)
					   {
						   my_flag=0;
					   		count=0;
					   		if(light_flag==1)
					   		{
					   			light_flag=0;
					   			XGpio_DiscreteWrite(&light,1,0x1);
					   		}
					   		else
					   		{
					   			light_flag=1;
					   			XGpio_DiscreteWrite(&light,1,0x2);
					   		}
					   	}

				   }

			   }

		   }


   return 0;
}
