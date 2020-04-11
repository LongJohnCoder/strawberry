# Progress

This document are mainly used to write down problems, experiences, keynotes etc. 

## 22 March 2020 - Real-time test of the kernel under stress

I have done some testing with the oscillscope. I added six threads, each toogling a pin every millisecond. The picture below shows how long time it takes to  switch between threads. With a -O3 optimization level, this switch takes about 2.5 us. With no optimization the switch takes about 5 us. This means that a CPU @ 300 MHz uses around 750 clock cycles to switch between threads. I will investigate this further. 

<img src="https://github.com/bjornbrodtkorb/BlackOS/blob/master/BlackOS%20Graphics/thread_switch_1ms_delay.PNG" width="600">

This means that a normal running thread with a 1 ms timeslice will be using 2.3 ‰ of its runtime inside the scheduler / context switch. Even though the switch time may be long it is still real-time. I tested with the six waveform threads, runtime stat thread, file thread and four computing threads. CPU was working 100 % but the waveforms every millisecond. Under rare situations a waveform entry was delayes 10 us. Newertheless this is accepted as real-time.  
