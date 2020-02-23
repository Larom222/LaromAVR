/*
 * rf_rx.h
 *
 * Created: 20.02.2016 11:56:18
 *  Author: Christian Wagner
 */


#ifndef RECEIVE_H_
#define RECEIVE_H_

#ifdef __cplusplus
extern "C" {
#endif

void rf_rx_irq();
void rf_rx_start(void* buffer, uint8_t size, uint8_t samples, uint8_t id);
void rf_rx_restart();
uint8_t rf_rx_done();
uint8_t rf_rx_wait();
void rf_rx_set_io(volatile uint8_t* reg, uint8_t pin);

#ifdef __cplusplus
}
#endif

#endif /* RECEIVE_H_ */