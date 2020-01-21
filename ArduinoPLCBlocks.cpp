#include "ArduinoPLCBlocks.h"

#define G_MAX_MS_COUNTER 0xffffffff

#include <stdint.h>
#include <arduino.h>

TON::TON(uint64_t PresetTime)
{
    this->m_ActualTime = 0;
    this->m_ET = 0;
    this->m_IN = false;
    this->m_M = false;
    this->m_PT = PresetTime;
    this->m_Q = false;
    this->m_RESET = false;
    this->m_StartTime = 0;
}

void TON::IN(bool in){
    this->m_IN = in;
}

bool TON::Q(){
    return this->m_Q;
}

void TON::RESET(){
    this->m_RESET = true;
}

void TON::exec()
{
    this->m_ActualTime = millis();

    if (this->m_RESET && this->m_IN)
    {
        this->m_StartTime = this->m_ActualTime;
        this->m_RESET = false;
    }

    /* Rising Edge of IN should reset the TIME */
    if (this->m_IN && !(this->m_M))
    {
        this->m_StartTime = this->m_ActualTime;
    }

    /* Memory to detect the Rising Edge of IN */
    this->m_M = this->m_IN;

    /* Count When IN is true (TON timer) */
    if (this->m_IN)
    {
        if (this->m_ActualTime < this->m_StartTime)
        {
            this->m_ET = this->m_ActualTime + (G_MAX_MS_COUNTER - this->m_StartTime);
        }
        else
        {
            this->m_ET = this->m_ActualTime - this->m_StartTime;
        }
    }
    else
    {
        this->m_ET = false;
    }

    /* Q is on after Timer is done when IN is true */
    if (this->m_IN && ((((this->m_ET) >= (this->m_PT))) || this->m_Q))
    {
        this->m_Q = 1;
    }
    else
    {
        this->m_Q = 0;
    }
}

TOF::TOF(uint64_t PresetTime)
{
    this->m_ActualTime = 0;
    this->m_ET = 0;
    this->m_IN = false;
    this->m_M = false;
    this->m_TI = false;
    this->m_PT = PresetTime;
    this->m_Q = false;
    this->m_RESET = false;
    this->m_StartTime = 0;
}

void TOF::IN(bool in){
    this->m_IN = in;
}

bool TOF::Q(){
    return this->m_Q;
}

void TOF::RESET(){
    this->m_RESET = true;
}

void TOF::exec()
{
    this->m_ActualTime = millis();

    if (this->m_RESET)
    {
        this->m_StartTime = this->m_ActualTime;
        this->m_TI = 0;
        this->m_Q = 0;
        this->m_RESET = 0;
    }

    /* Falling Edge of IN should reset the TIME */
    if (!(this->m_IN) && this->m_M)
    {
        this->m_StartTime = this->m_ActualTime;
        this->m_TI = 1;
    }

    /* Memory to detect the Rising Edge of IN */
    this->m_M = this->m_IN;

    /* Count When IN is true (TOF timer) */
    if (this->m_TI)
    {
        if (this->m_ActualTime < this->m_StartTime)
        {
            this->m_ET = this->m_ActualTime + (G_MAX_MS_COUNTER - this->m_StartTime);
        }
        else
        {
            this->m_ET = this->m_ActualTime - this->m_StartTime;
        }
    }
    else
    {
        this->m_ET = 0;
    }

    /* Q is on after Timer is done when IN is true */
    if (this->m_IN || (this->m_TI && (this->m_ET) <= (this->m_PT)))
    {
        this->m_Q = 1;
    }
    else
    {
        this->m_Q = 0;
    }
}

R_TRIG::R_TRIG()
{
    this->m_CLK = false;
    this->m_M = false;
    this->m_Q = false;
}

void R_TRIG::exec()
{
    this->m_Q = this->m_CLK && !this->m_M;
    this->m_M = this->m_CLK;
}

void R_TRIG::CLK(bool clk){
    this->m_CLK=clk;
}

bool R_TRIG::Q(){
    return this->m_Q;
}

F_TRIG::F_TRIG()
{
    this->m_CLK = false;
    this->m_M = false;
    this->m_Q = false;
}

void F_TRIG::exec()
{
    this->m_Q = !this->m_CLK && this->m_M;
    this->m_M = this->m_CLK;
}

void F_TRIG::CLK(bool clk){
    this->m_CLK=clk;
}

bool F_TRIG::Q(){
    return this->m_Q;
}

SR_BLOC::SR_BLOC()
{
    this->m_S = false;
    this->m_R = false;
    this->m_Q = false;
}

void SR_BLOC::exec()
{
    this->m_Q = (this->m_S || this->m_Q) && !this->m_R;
}

void SR_BLOC::set(){
    this->m_S=true;
}

void SR_BLOC::reset(){
    this->m_R=true;
}

//FLASHER
FLASHER::FLASHER(uint64_t OnTime, uint64_t OffTime):TimerOn(OnTime),TimerOff(OffTime){
    this->m_Q=false;
    this->m_IN=false;

    //this->TimerOn = this->TimerOn(OnTime);
    //this->TimerOff = this->TimerOff(OffTime);
}

void FLASHER::exec(){

    if(this->m_IN){

        this->TimerOn.IN(!this->TimerOff.Q());
        this->TimerOff.IN(this->TimerOn.Q());

        if(!this->TimerOn.Q()){
            this->m_Q=true;
        }else{
            this->m_Q=false;
        }

    }else{
        this->TimerOn.RESET();
        this->TimerOff.RESET();
        this->m_Q=false;
    }

    this->TimerOn.exec();
    this->TimerOff.exec();  


}

bool FLASHER::Q(){
    return this->m_Q;
}

void FLASHER::IN(bool in){
    this->m_IN=in;
}

//FIFO

//CODE_FLASHER

FLASH_COUNTER::FLASH_COUNTER(uint64_t Time_ON, uint64_t Time_Pause, uint64_t Time_OFF):m_pause_timer(Time_Pause), m_led_on_timer(Time_ON),m_led_off_timer(Time_OFF)
{
    this->m_sfc_step=0;
    this->m_flash_complete=false;
	this->m_start_flash_command=false;

	this->m_flash_count=1;
    this->m_led_output=false;

	this->m_current_index=0;

	this->m_X0=true;
	this->m_X1=false;
	this->m_X2=false;
	this->m_X3=false;
	this->m_X4=false;
	this->m_X5=false;

	//this->m_pause_timer = this->m_pause_timer(Time_Pause);
    //this->m_led_on_timer = this->m_led_on_timer(Time_ON);
	//this->m_led_off_timer = this->m_led_off_timer(Time_OFF);

    	
}

void FLASH_COUNTER::start(){
    this->m_start_flash_command=true;
}

bool FLASH_COUNTER::cplt(){
    return this->m_flash_complete;
}

bool FLASH_COUNTER::output(){
    return this->m_led_output;
}

void FLASH_COUNTER::cplt_ack(){
    this->m_flash_complete=false;
}

void FLASH_COUNTER::exec()
{


    bool Y0_1;
	bool Y1_2;
	bool Y4_2;
	bool Y2_3;
	bool Y3_4;
	bool Y4_5;
	bool Y5_0;


    /* Transitions */
	/* Yn = Xn * Rn */
	Y0_1=this->m_X0 && this->m_start_flash_command;
	Y1_2=this->m_X1 && this->m_pause_timer.Q();
	Y4_2=this->m_X4 && this->m_flash_count>this->m_current_index;
	Y2_3=this->m_X2 && this->m_led_on_timer.Q();
	Y3_4=this->m_X3 && this->m_led_off_timer.Q();
	Y4_5=this->m_X4 && (this->m_current_index>=this->m_flash_count) && !Y4_2;
	Y5_0=this->m_X5 && true;

	/* Steps */
	/* Xn = Y(n-1) + Xn * /Yn  */
	this->m_X0=(Y5_0||this->m_X0)&&!Y0_1;
	this->m_X1=(Y0_1||this->m_X1)&&!Y1_2;
	this->m_X2=(Y1_2||Y4_2||this->m_X2)&&!Y2_3;
	this->m_X3=(Y2_3||this->m_X3)&&!Y3_4;
	this->m_X4=(Y3_4||this->m_X4)&&!Y4_5&&!Y4_2;
	this->m_X5=(Y4_5||this->m_X5)&&!Y5_0;

	/* Actions */
	if(this->m_X0){
		this->m_sfc_step=0;
	}
	if(this->m_X1){
		this->m_sfc_step=1;
	}
	if(this->m_X2){
		this->m_sfc_step=2;
	}
	if(this->m_X3){
		this->m_sfc_step=3;
	}
	if(this->m_X4){
		this->m_sfc_step=4;
	}
	if(this->m_X5){
		this->m_sfc_step=5;
		this->m_flash_complete=true;
	}

	if(this->m_sfc_step==1){
		this->m_pause_timer.IN(true);
		this->m_current_index=0;
		this->m_start_flash_command=false;
	}else{
		this->m_pause_timer.IN(false);
	}

	if(this->m_sfc_step==2){
		this->m_led_on_timer.IN(true);
		this->m_led_output=true;
	}else{
		this->m_led_on_timer.IN(false);
		this->m_led_output=false;
	}

	if(this->m_sfc_step==3){
		this->m_led_off_timer.IN(true);
	}else{
		this->m_led_off_timer.IN(false);
	}

	if(this->m_sfc_step==4){
		this->m_current_index+=1;
	}

	this->m_pause_timer.exec();
	this->m_led_on_timer.exec();
	this->m_led_off_timer.exec();

}