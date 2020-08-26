#include "app_uart_fifo.h"

#if (defined APP_UART_FIFO_LIVE_DBG) && (APP_UART_FIFO_LIVE_DBG == 1)
static uint32_t live_dbg_fifo_write, live_dbg_fifo_read, live_dbg_uart_put;
#endif

static app_uart_evt_t tx_irq_event = {.evt_type = EVT_APP_UART_TX_EMPTY};

static inline uint32_t fifo_length(app_fifo_t * const fifo)
{
  uint32_t tmp = fifo->read_pos;
  return fifo->write_pos - tmp;
}

#define FIFO_LENGTH(F) fifo_length(&F)              /**< Macro to calculate length of a FIFO. */

/* Private functions ---------------------------------------------------------*/

uint32_t app_uartRx_is_available(app_uart_fifo_ctx_t *app_cxt)
{
  return FIFO_LENGTH(app_cxt->rx_fifo);
}

uint32_t app_uartTx_can_send(app_uart_fifo_ctx_t *app_cxt)
{
  uint32_t available_count = app_cxt->tx_fifo.buf_size_mask - FIFO_LENGTH(app_cxt->tx_fifo) + 1;
  return available_count;
}

static void app_uart_fifo_irq_event_handler(struct app_uart_fifo_ctx *app_cxt, app_uart_irq_t irq_event)
{
	app_uart_evt_t app_uart_event;
	app_fifo_status_t err_code;
	uint32_t rtx_size;
	uint32_t fifo_size;
	switch (irq_event)
    {
        case APP_UART_IRQ_RX:
        	// app_uart_put(app_cxt, app_cxt->rx_irq.buff[0]);	// add for debug rx echo
            // Write received byte to FIFO.
        	rtx_size = app_cxt->rx_irq.size;
            err_code = app_fifo_write(&app_cxt->rx_fifo, app_cxt->rx_irq.buff, &rtx_size);
            if (err_code != APP_FIFO_OK)
            {
                app_uart_event.evt_type = EVT_APP_UART_RX_FIFO_ERROR;
                app_cxt->fp_event(app_cxt, &app_uart_event);
            }
            // Notify that there are data available.
            else if (FIFO_LENGTH(app_cxt->rx_fifo) != 0)
            {
                app_uart_event.evt_type = EVT_APP_UART_DATA_READY;
                app_cxt->fp_event(app_cxt, &app_uart_event);
            }

            // Start new RX if size in buffer.
            fifo_size = app_fifo_available(&app_cxt->rx_fifo);
            if (fifo_size > 0)
            {
                if (fifo_size > app_cxt->rx_irq.size)
                {
                	app_cxt->fp_receive(app_cxt->rx_irq.buff, app_cxt->rx_irq.size);
                }
                else
                {
                	app_cxt->fp_receive(app_cxt->rx_irq.buff, fifo_size);
                }
            }
            else
            {
                // Overflow in RX FIFO.
            	app_cxt->rx_over = (uint8_t)EVT_APP_UART_RX_OVER;
            	app_uart_event.evt_type = EVT_APP_UART_RX_OVER;
            	app_cxt->fp_event(app_cxt, &app_uart_event);
            }

            break;

        case APP_UART_IRQ_TX:
            // Get next byte from FIFO.
        	rtx_size = app_cxt->tx_irq.size;
        	err_code = app_fifo_read(&app_cxt->tx_fifo, app_cxt->tx_irq.buff, &rtx_size);
            if (err_code == APP_FIFO_OK)
            {
#if (defined APP_UART_FIFO_LIVE_DBG) && (APP_UART_FIFO_LIVE_DBG == 1)
            	live_dbg_fifo_read += rtx_size;
#endif
            	app_cxt->fp_transmit(app_cxt->tx_irq.buff, rtx_size);

                // If FIFO was full new request to receive one byte was not scheduled. Must be done here.
                if (app_cxt->rx_over == (uint8_t)EVT_APP_UART_TX_OVER)
                {
                    app_cxt->tx_over = (uint8_t)EVT_APP_UART_TX_READY;
                    app_uart_event.evt_type = EVT_APP_UART_TX_READY;
		            app_cxt->fp_event(app_cxt, &app_uart_event);
                }
            }
            else
            {
            	tx_irq_event.evt_type = EVT_APP_UART_TX_EMPTY;
            	// Last byte from FIFO transmitted, notify the application.
                app_uart_event.evt_type = EVT_APP_UART_TX_EMPTY;
                app_cxt->fp_event(app_cxt, &app_uart_event);
            }
            break;

        case APP_UART_IRQ_ERROR:
            break;

        default:
            break;
    }
}


uint32_t app_uart_fifo_init(app_uart_fifo_ctx_t *app_cxt, app_uart_buffers_t *p_buffers)
{
    if (p_buffers == NULL)
    {
        return APP_UART_FIFO_ERROR;
    }

    app_cxt->irq_callback = app_uart_fifo_irq_event_handler;

    // Configure buffer RX buffer.
    app_fifo_init(&app_cxt->rx_fifo, p_buffers->fifo.rx_buf, p_buffers->fifo.rx_buf_size);

    // Configure buffer TX buffer.
    app_fifo_init(&app_cxt->tx_fifo, p_buffers->fifo.tx_buf, p_buffers->fifo.tx_buf_size);

    app_cxt->rx_irq.buff = p_buffers->irq.rx_buf;
    app_cxt->rx_irq.size = p_buffers->irq.rx_buf_size;

    app_cxt->tx_irq.buff = p_buffers->irq.tx_buf;
    app_cxt->tx_irq.size = p_buffers->irq.tx_buf_size;

    app_cxt->rx_over = (uint8_t)EVT_APP_UART_RX_EMPTY;;

    // Turn on receiver
    if (app_cxt->rx_disable == 0)
    {
    	app_cxt->fp_receive(app_cxt->rx_irq.buff, app_cxt->rx_irq.size);
    }

    tx_irq_event.evt_type = EVT_APP_UART_TX_EMPTY;

    return APP_UART_FIFO_OK;
}


uint32_t app_uart_flush(app_uart_fifo_ctx_t *app_cxt)
{
    app_fifo_flush(&app_cxt->rx_fifo);

    app_fifo_flush(&app_cxt->rx_fifo);

    return APP_UART_FIFO_OK;
}


uint32_t app_uart_get(app_uart_fifo_ctx_t *app_cxt, uint8_t *p_byte)
{
    uint8_t rx_ovf = app_cxt->rx_over;

    app_fifo_get(&app_cxt->rx_fifo, p_byte);

    // If FIFO was full new request to receive one byte was not scheduled. Must be done here.
    if (rx_ovf == (uint8_t)EVT_APP_UART_RX_OVER)
    {
        app_uart_evt_t app_uart_event;

    	app_cxt->rx_over = (uint8_t)EVT_APP_UART_RX_READY;
    	app_cxt->fp_receive(app_cxt->rx_irq.buff, app_cxt->rx_irq.size);
        
		app_uart_event.evt_type = EVT_APP_UART_RX_READY;
		app_cxt->fp_event(app_cxt, &app_uart_event);
    }

    return APP_UART_FIFO_OK;
}


uint32_t app_uart_put(app_uart_fifo_ctx_t *app_cxt, uint8_t byte)
{
    app_uart_evt_t app_uart_event;
    uint32_t err_code;
    uint32_t rtx_size;

#if (defined APP_UART_FIFO_LIVE_DBG) && (APP_UART_FIFO_LIVE_DBG == 1)
    live_dbg_uart_put++;
#endif
    // wait util tx fifo available
    while(0 == app_fifo_available(&app_cxt->tx_fifo)) {};    

    err_code = app_fifo_put(&app_cxt->tx_fifo, byte);
    if (APP_FIFO_OK == err_code)
    {
#if (defined APP_UART_FIFO_LIVE_DBG) && (APP_UART_FIFO_LIVE_DBG == 1)
    	live_dbg_fifo_write++;
#endif
    	// The new byte has been added to FIFO. It will be picked up from there
        // (in 'uart_event_handler') when all preceding bytes are transmitted.
        // But if UART is not transmitting anything at the moment, we must start
        // a new transmission here.
        if (EVT_APP_UART_TX_EMPTY == tx_irq_event.evt_type)
        {
        	tx_irq_event.evt_type = EVT_APP_UART_DATA_SENDING;
        	// This operation should be almost always successful, since we've
            // just added a byte to FIFO, but if some bigger delay occurred
            // (some heavy interrupt handler routine has been executed) since
            // that time, FIFO might be empty already.
        	rtx_size = app_cxt->tx_irq.size;
			err_code = app_fifo_read(&app_cxt->tx_fifo, app_cxt->tx_irq.buff, &rtx_size);
			if (APP_FIFO_OK == err_code)
			{
#if (defined APP_UART_FIFO_LIVE_DBG) && (APP_UART_FIFO_LIVE_DBG == 1)
				live_dbg_fifo_read += rtx_size;
#endif
				app_cxt->fp_transmit(app_cxt->tx_irq.buff, rtx_size);

                app_uart_event.evt_type = EVT_APP_UART_DATA_SENDING;
		        app_cxt->fp_event(app_cxt, &app_uart_event);
			}
        }

        // make event callback to notify TX fifo is full
        if (0 == app_fifo_available(&app_cxt->tx_fifo))
        {    	
            // Overflow in TX FIFO.
            app_cxt->tx_over = (uint8_t)EVT_APP_UART_TX_OVER;
            app_uart_event.evt_type = EVT_APP_UART_TX_OVER;
            app_cxt->fp_event(app_cxt, &app_uart_event);
        }
    }
    else
    {
        app_uart_event.evt_type = EVT_APP_UART_TX_FIFO_ERROR;
        app_cxt->fp_event(app_cxt, &app_uart_event);
    }
    
    return ((APP_FIFO_OK == err_code) ? APP_UART_FIFO_OK : APP_UART_FIFO_ERROR);
}
