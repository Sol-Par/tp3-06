#ifndef INC_DISPLAY_H_
#define INC_DISPLAY_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/
#include <stdint.h>
/********************** macros ***********************************************/

/********************** typedef **********************************************/

/********************** external data declaration ****************************/
typedef enum {
     DISPLAY_CONNECTION_GPIO_4BITS,
     DISPLAY_CONNECTION_GPIO_8BITS,
     DISPLAY_CONNECTION_I2C_PCF8574_IO_EXPANDER,
} displayConnection_t;

typedef struct {
   displayConnection_t connection;
} display_t;

/********************** external functions declaration ***********************/
void displayInit( displayConnection_t connection );

void displayCharPositionWrite( uint8_t charPositionX, uint8_t charPositionY );

void displayStringWrite( const char * str );

#endif /* INC_DISPLAY_H_ */
