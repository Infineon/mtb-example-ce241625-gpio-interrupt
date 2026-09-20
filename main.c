/*******************************************************************************
* File Name:   main.c
*
* Description: This code example demonstrates the use of GPIO configured as an
*              input pin to generate interrupts.
*
* Related Document: See README.md
*
*
********************************************************************************
* (c) 2026, Infineon Technologies AG, or an affiliate of Infineon
* Technologies AG. All rights reserved.
* This software, associated documentation and materials ("Software") is
* owned by Infineon Technologies AG or one of its affiliates ("Infineon")
* and is protected by and subject to worldwide patent protection, worldwide
* copyright laws, and international treaty provisions. Therefore, you may use
* this Software only as provided in the license agreement accompanying the
* software package from which you obtained this Software. If no license
* agreement applies, then any use, reproduction, modification, translation, or
* compilation of this Software is prohibited without the express written
* permission of Infineon.
*
* Disclaimer: UNLESS OTHERWISE EXPRESSLY AGREED WITH INFINEON, THIS SOFTWARE
* IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* INCLUDING, BUT NOT LIMITED TO, ALL WARRANTIES OF NON-INFRINGEMENT OF
* THIRD-PARTY RIGHTS AND IMPLIED WARRANTIES SUCH AS WARRANTIES OF FITNESS FOR A
* SPECIFIC USE/PURPOSE OR MERCHANTABILITY.
* Infineon reserves the right to make changes to the Software without notice.
* You are responsible for properly designing, programming, and testing the
* functionality and safety of your intended application of the Software, as
* well as complying with any legal requirements related to its use. Infineon
* does not guarantee that the Software will be free from intrusion, data theft
* or loss, or other breaches ("Security Breaches"), and Infineon shall have
* no liability arising out of any Security Breaches. Unless otherwise
* explicitly approved by Infineon, the Software may not be used in any
* application where a failure of the Product or any consequences of the use
* thereof can reasonably be expected to result in personal injury.
*******************************************************************************/


/*******************************************************************************
* Header Files
*******************************************************************************/
#include "cy_pdl.h"
#include "cybsp.h"

/*******************************************************************************
* Macros
*******************************************************************************/
#define GPIO_INTERRUPT_PRIORITY 7
#define DELAY_SHORT_MS          (250)   /* milliseconds */
#define DELAY_LONG_MS           (500)   /* milliseconds */
#define LED_BLINK_COUNT         (4)

/*******************************************************************************
* Global Variables
*******************************************************************************/
volatile bool gpio_intr_flag = false;

/*******************************************************************************
* Function Prototypes
*******************************************************************************/
/* This structure initializes the Port5 interrupt for the NVIC */
cy_stc_sysint_t intrCfgBTN =
{
    .intrSrc = ioss_interrupts_sec_gpio_5_IRQn, /* Interrupt source is GPIO port 5 interrupt */
    .intrPriority = GPIO_INTERRUPT_PRIORITY /* Interrupt priority is 7 */
};

/*******************************************************************************
* Function Definitions
*******************************************************************************/
static void BTN_interrupt_handler_PDL();

/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary:
* This is the main function for CPU. This function configures and initializes the
* GPIO interrupt, update the delay on every GPIO interrupt, blinks the LED and
* enter in deepsleep mode.
*
* Parameters:
*  void
*
* Return:
*  int
*
*******************************************************************************/
int main(void)
{
    cy_rslt_t result;
    uint32_t count = 0;
    uint32_t delay_led_blink = DELAY_LONG_MS;

    /* Initialize the device and board peripherals */
    result = cybsp_init();

    /* Board init failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Configure GPIO Port 5 interrupt and register the ISR */
    Cy_SysInt_Init(&intrCfgBTN, BTN_interrupt_handler_PDL);
    NVIC_EnableIRQ(ioss_interrupts_sec_gpio_5_IRQn);

    /* Enable global interrupts */
    __enable_irq();

    for (;;)
    {
        /* Check if the GPIO interrupt flag was set by the ISR */
        if (true == gpio_intr_flag)
        {
            gpio_intr_flag = false;

            /* Toggle the blink speed on each button press:
               slow (500 ms) <-> fast (250 ms) */
            if (DELAY_LONG_MS == delay_led_blink)
            {
                delay_led_blink = DELAY_SHORT_MS;
            }
            else
            {
                delay_led_blink = DELAY_LONG_MS;
            }
        }
        /* Blink LED four times at the current speed before re-entering deep sleep */
        for (count = 0; count < LED_BLINK_COUNT; count++)
        {
            Cy_GPIO_Write(CYBSP_USER_LED2_PORT, CYBSP_USER_LED2_PIN, CYBSP_LED_STATE_ON);  /* Turn LED on  */
            Cy_SysLib_Delay(delay_led_blink);
            Cy_GPIO_Write(CYBSP_USER_LED2_PORT, CYBSP_USER_LED2_PIN, CYBSP_LED_STATE_OFF); /* Turn LED off */
            Cy_SysLib_Delay(delay_led_blink);
        }
        
        /* Enter deep sleep mode; the GPIO interrupt will wake the CPU */
        Cy_SysPm_CpuEnterDeepSleep(CY_SYSPM_WAIT_FOR_INTERRUPT);
    }
}

/*******************************************************************************
* Function Name: BTN_interrupt_handler_PDL
********************************************************************************
*
*  Summary:
*  GPIO interrupt handler for the PDL example.
*
*  Parameters:
*  None
*
*  Return:
*  None
*
**********************************************************************************/
static void BTN_interrupt_handler_PDL()
{
    /* Clear the GPIO port interrupt flag to allow the next interrupt to be detected */
    Cy_GPIO_ClearInterrupt(CYBSP_USER_BTN_PORT, CYBSP_USER_BTN_PIN);
    /* Set the flag so the main loop can react to the button press */
    gpio_intr_flag = true;

}
/* [] END OF FILE */
