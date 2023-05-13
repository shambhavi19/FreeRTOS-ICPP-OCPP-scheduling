#include "scheduler.h"

TaskHandle_t xHandle1 = NULL;
TaskHandle_t xHandle2 = NULL;
TaskHandle_t xHandle3 = NULL;
TaskHandle_t xHandle4 = NULL;
TaskHandle_t xHandle5 = NULL;

TickType_t xWCET1 = 1300;//300;
TickType_t xWCET2 = 1500;//300;
TickType_t xWCET3 = 1600;//200;
TickType_t xWCET4 = 1800;//600;
TickType_t xWCET5 = 2000;//600;


UBaseType_t CPR1 = 5;
UBaseType_t CPR2 = 4;
UBaseType_t CP[2] = {5,4};


Res* R1;
Res* R2;

// the loop function runs over and over again forever
void loop() {}

static void nonCSFunc(unsigned long delayMs)
{

  TickType_t xTickCountStart = xTaskGetTickCount();
  TickType_t xTickCountEnd = xTickCountStart;
  TickType_t xTickCountInterval = pdMS_TO_TICKS(delayMs - 7);

  while ((xTickCountEnd - xTickCountStart) < xTickCountInterval) {
    for (int i = 0; i < 1000; i++) {}
    xTickCountEnd = xTaskGetTickCount();
  }

volatile int i;
while ((xTickCountEnd - xTickCountStart) < xTickCountInterval) {
    for (i = 0; i < 1000; i++) {
    }
    xTickCountEnd = xTaskGetTickCount();
}
}


static void testFunc1( void *pvParameters )
{
	(void) pvParameters;
  // Serial.println("Task 1 starts !!");
  nonCSFunc(100);
  vSchedulerResourceWait(R1, &xHandle1, xWCET1);
  //Critical section starts
  Serial.println("Task 1 in CS 1 !!");
  nonCSFunc(100);
  //Critical section ends
  vSchedulerResourceSignal(R1, &xHandle1);
  // Serial.println("Task 1 exits CS !!");
  nonCSFunc(100);
  // Serial.println("Task 1 ends !!");

}

static void testFunc2( void *pvParameters )
{ 
	(void) pvParameters;
  // Serial.println("Task 2 starts !!");
  nonCSFunc(100);
  vSchedulerResourceWait(R2, &xHandle2, xWCET2);
  //Critical section starts
  Serial.println("Task 2 in CS 2 !!");
  nonCSFunc(100);
  //Critical section ends
  vSchedulerResourceSignal(R2, &xHandle2);
  // Serial.println("Task 2 exits CS !!");
  nonCSFunc(100);
  // Serial.println("Task 2 ends !!");
}

static void testFunc3( void *pvParameters )
{
	(void) pvParameters;
  // Serial.println("Task 3 starts !!");
  nonCSFunc(200);
  // Serial.println("Task 3 ends !!");

}

static void testFunc4( void *pvParameters )
{ 
	(void) pvParameters;
  // Serial.println("Task 4 starts !!");
  nonCSFunc(100);
  vSchedulerResourceWait(R1, &xHandle4, xWCET4);
  //Critical section starts for resource 1
  Serial.println("Task 4 in CS 1 !!");
  nonCSFunc(150);

  vSchedulerResourceWait(R2, &xHandle4, xWCET4);
  //Critical section starts for resource 2
  Serial.println("Task 4 in CS 2 !!");
  nonCSFunc(150);
  //Critical section ends for resource 2
  vSchedulerResourceSignal(R2, &xHandle4);
  // Serial.println("Task 4 exits CS 2!!");
  
  nonCSFunc(100);
  //Critical section ends for resource 1
  vSchedulerResourceSignal(R1, &xHandle4);
  // Serial.println("Task 4 exits CS 1!!");

  nonCSFunc(100);
  // Serial.println("Task 4 ends !!");

}

static void testFunc5( void *pvParameters )
{ 
	(void) pvParameters;
  // Serial.println("Task 5 in CS !!");
  nonCSFunc(100);
  vSchedulerResourceWait(R2, &xHandle5, xWCET5);

  //Critical section starts
  Serial.println("Task 5 in CS 2 !!");
  nonCSFunc(400);
  //Critical section ends
  vSchedulerResourceSignal(R2, &xHandle5);
  // Serial.println("Task 5 exits CS !!");

  nonCSFunc(100);
  // Serial.println("Task 5 ends !!");

}

int main( void )
{
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB, on LEONARDO, MICRO, YUN, and other 32u4 based boards.
  }
	char c1 = 'a';
	char c2 = 'b';			
	char c3 = 'c';
	char c4 = 'd';
  char c5 = 'e';

  vInitResources(2, CP);

	vSchedulerInit();

	vSchedulerPeriodicTaskCreate(testFunc1, "t1", configMINIMAL_STACK_SIZE, &c1, 5, &xHandle1, pdMS_TO_TICKS(800), pdMS_TO_TICKS(2000), pdMS_TO_TICKS(xWCET1), pdMS_TO_TICKS(2100));
	vSchedulerPeriodicTaskCreate(testFunc2, "t2", configMINIMAL_STACK_SIZE, &c2, 4, &xHandle2, pdMS_TO_TICKS(500), pdMS_TO_TICKS(2200), pdMS_TO_TICKS(xWCET2), pdMS_TO_TICKS(2300));
  vSchedulerPeriodicTaskCreate(testFunc3, "t3", configMINIMAL_STACK_SIZE, &c3, 3, &xHandle3, pdMS_TO_TICKS(400), pdMS_TO_TICKS(2400), pdMS_TO_TICKS(xWCET3), pdMS_TO_TICKS(2500));
	vSchedulerPeriodicTaskCreate(testFunc4, "t4", configMINIMAL_STACK_SIZE, &c4, 2, &xHandle4, pdMS_TO_TICKS(200), pdMS_TO_TICKS(2500), pdMS_TO_TICKS(xWCET4), pdMS_TO_TICKS(2600));
	vSchedulerPeriodicTaskCreate(testFunc5, "t5", configMINIMAL_STACK_SIZE, &c5, 1, &xHandle5, pdMS_TO_TICKS(0), pdMS_TO_TICKS(2700), pdMS_TO_TICKS(xWCET5), pdMS_TO_TICKS(2800));

	vSchedulerStart();

  //5,6,7,8

	/* If all is well, the scheduler will now be running, and the following line
	will never be reached. */
	
	for( ;; );
}

