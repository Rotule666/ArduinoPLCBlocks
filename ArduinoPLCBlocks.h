
#ifndef ARDUINOPLCBLOCKS_H
#define ARDUINOPLCBLOCKS_H

#include <stdint.h>
#include <arduino.h>




class TON{

    public:

    TON(uint64_t PresetTime);

    void exec();

    void IN(bool in);

    bool Q();

    void RESET();

    private:

    bool m_IN;		// Input
    bool m_Q;			// Output

	uint64_t m_PT;	// Preset Time in mS
	bool m_RESET; 	// Reset Command

	uint64_t m_ET;	// Elapsed Time in mS

	bool m_M;		    // Memory

	uint64_t m_StartTime;	    //
	uint64_t m_ActualTime;	//
};

class TOF{

    public:
 
    TOF(uint64_t PresetTime);

    void exec();

    void IN(bool in);

    bool Q();

    void RESET();

    private:

    bool m_IN;		// Input
    bool m_Q;			// Output

	uint64_t m_PT;	// Preset Time in mS
	bool m_RESET; 	// Reset Command

	
	uint64_t m_ET;	// Elapsed Time in mS

	bool m_M;		    // Memory
    bool m_TI;        // Timer Input

	uint64_t m_StartTime;	    //
	uint64_t m_ActualTime;	//
    
};

class R_TRIG{

    public:
    
    R_TRIG();
    void exec();
    void CLK(bool clk);
    bool Q();


    private:

    bool m_CLK;
    bool m_Q;
    bool m_M;
};

class F_TRIG{

    public:
    F_TRIG();
    void exec();
    void CLK(bool clk);
    bool Q();

    private:

    bool m_CLK;
    bool m_Q;
    bool m_M;
};


class SR_BLOC{

    public:
    SR_BLOC();
    void exec();
    void set();
    void reset();

    private:

    bool m_S;
    bool m_R;
    bool m_Q;
};

//FLASHER
class FLASHER{
    public:

    FLASHER(uint64_t OnTime, uint64_t OffTime);
    void exec();
    bool Q();
    void IN(bool in);

    private:
    bool m_Q;
    bool m_IN;

    TON TimerOn;
    TON TimerOff;
};
//FIFO

//CODE_FLASHER


class FLASH_COUNTER{

    public:

    FLASH_COUNTER(uint64_t Time_ON, uint64_t Time_Pause, uint64_t Time_OFF);
    void exec();
    void start();
    bool cplt();
    void cplt_ack();
    bool output();

    private:

    uint8_t m_sfc_step;
    bool m_flash_complete;
	bool m_start_flash_command;
	uint8_t m_flash_count;
    bool m_led_output;
	uint8_t m_current_index;

	bool m_X0;
	bool m_X1;
	bool m_X2;
	bool m_X3;
	bool m_X4;
	bool m_X5;

	TON m_pause_timer;
    TON m_led_on_timer;
	TON m_led_off_timer;
};

#endif
