/** \file shapemotion.c
 *  \brief This is a simple shape motion demo.
 *  This demo creates two layers containing shapes.
 *  One layer contains a rectangle and the other a circle.
 *  While the CPU is running the green LED is on, and
 *  when the screen does not need to be redrawn the CPU
 *  is turned off along with the green LED.
 */  
#include <msp430.h>
#include <libTimer.h>
#include <lcdutils.h>
#include <lcddraw.h>
#include <p2switches.h>
#include <shape.h>
#include <abCircle.h>
#include "buzzer.h"

#define GREEN_LED BIT6
#define RED_LED BIT0

#define P1LEFT BIT0
#define P1RIGHT BIT1

#define P2LEFT BIT2
#define P2RIGHT BIT3

#define BUTTONS p2sw_read()

char player1,player2 =0;

AbRect bottom_rec = {abRectGetBounds, abRectCheck, {8,2}}; /**< 10x10 rectangle */

AbRect top_rec = {abRectGetBounds, abRectCheck, {8,2}}; /**< 10x10 rectangle */

AbRectOutline fieldOutline = {	/* playing field */
  abRectOutlineGetBounds, abRectOutlineCheck,   
  {screenWidth/2 - 20, screenHeight/2 - 20}
};


Layer fieldLayer = {		/* playing field as a layer */
  (AbShape *) &fieldOutline,
  {screenWidth/2, screenHeight/2}, /**< center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_RED,
  0
};

Layer layer2 = {		/**< Layer with p1 rectangle */
  (AbShape *)&top_rec,
  {screenWidth/2, 25}, /**< top of the fence */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_RED,
  &fieldLayer,
};

Layer layer1 = {		/**< Layer with p2 rectangle */
  (AbShape *)&bottom_rec,
  {screenWidth/2, screenHeight-25}, /**< bottom of fence */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_RED,
  &layer2,
};

Layer layer0 = {		/**< Layer with an red ping pong ball */
  (AbShape *)&circle7,
  {(screenWidth/2), (screenHeight/2)}, /**< center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_RED,
  &layer1,
};

/** Moving Layer
 *  Linked list of layer references
 *  Velocity represents one iteration of change (direction & magnitude)
 */
typedef struct MovLayer_s {
  Layer *layer;
  Vec2 velocity;
  struct MovLayer_s *next;
} MovLayer;

/* initial value of {0,0} will be overwritten */
//MovLayer ml3 = { &layer3, {1,1}, 0 }; /**< not all layers move */
MovLayer ml2 = { &layer2, {0,0}, 0 }; 
MovLayer ml1 = { &layer1, {0,0}, &ml2 }; 
MovLayer ml0 = { &layer0, {1,2}, &ml1 }; 
void movLayerDraw(MovLayer *movLayers, Layer *layers)
{
  int row, col;
  MovLayer *movLayer;

  and_sr(~8);			/**< disable interrupts (GIE off) */
  for (movLayer = movLayers; movLayer; movLayer = movLayer->next) { /* for each moving layer */
    Layer *l = movLayer->layer;
    l->posLast = l->pos;
    l->pos = l->posNext;
  }
  or_sr(8);			/**< disable interrupts (GIE on) */


  for (movLayer = movLayers; movLayer; movLayer = movLayer->next) { /* for each moving layer */
    Region bounds;
    layerGetBounds(movLayer->layer, &bounds);
    lcd_setArea(bounds.topLeft.axes[0], bounds.topLeft.axes[1], 
		bounds.botRight.axes[0], bounds.botRight.axes[1]);
    for (row = bounds.topLeft.axes[1]; row <= bounds.botRight.axes[1]; row++) {
      for (col = bounds.topLeft.axes[0]; col <= bounds.botRight.axes[0]; col++) {
	Vec2 pixelPos = {col, row};
	u_int color = bgColor;
	Layer *probeLayer;
	for (probeLayer = layers; probeLayer; 
	     probeLayer = probeLayer->next) { /* probe all layers, in order */
	  if (abShapeCheck(probeLayer->abShape, &probeLayer->pos, &pixelPos)) {
	    color = probeLayer->color;
	    break; 
	  } /* if probe check */
	} // for checking all layers at col, row
	lcd_writeColor(color); 
      } // for col
    } // for row
  } // for moving layer being updated
}	  



//Region fence = {{10,30}, {SHORT_EDGE_PIXELS-10, LONG_EDGE_PIXELS-10}}; /**< Create a fence region */

/** Advances a moving shape within a fence
 *  
 *  \param ml The moving shape to be advanced
 *  \param fence The region which will serve as a boundary for ml
 */

void mlAdvance(MovLayer *ml, Region *fence)
{
  Vec2 newPos;
  u_char axis;
  Region shapeBoundary;
  for (; ml; ml = ml->next) {
    vec2Add(&newPos, &ml->layer->posNext, &ml->velocity);
    abShapeGetBounds(ml->layer->abShape, &newPos, &shapeBoundary);
    for (axis = 0; axis < 2; axis ++) {
      if ((shapeBoundary.topLeft.axes[axis] < fence->topLeft.axes[axis]) ||
	  (shapeBoundary.botRight.axes[axis] > fence->botRight.axes[axis]) ) {
	int velocity = ml->velocity.axes[axis] = -ml->velocity.axes[axis];
	newPos.axes[axis] += (2*velocity);
      }	/**< if outside of fence */
    } /**< for axis */
    ml->layer->posNext = newPos;
  } /**< for ml */
}


int bounce(MovLayer *ml, Region *fence)
 {
  Vec2 newPos;
  Vec2 *test;
  u_char axis;
  Region shapeBoundary;
  for (; ml; ml = ml->next) {
    vec2Add(&newPos, &ml->layer->posNext, &ml->velocity);
    abShapeGetBounds(ml->layer->abShape, &newPos, &shapeBoundary);
    
    buzzer_set_period(1);
      if(ml->layer->abShape == ml0.layer->abShape){

	if(abShapeCheck(ml2.layer->abShape,&ml2.layer->posNext, &newPos)){
	  //if the moving layer is touched then the direction of layer l0 changes in the axis[1] direction(upper paddle)
	  
	  int velocity  = ml0.velocity.axes[1] = - ml0.velocity.axes[1];
	  newPos.axes[1] += (2*velocity);        
	  
	  buzzer_set_period(1000);

	}
    

	if(abShapeCheck(ml1.layer->abShape,&ml1.layer->posNext, &newPos)){

	  //if the moving layer is touched then the direction of layer l0 changes in the axis[1] direction(lower paddle)

	  
	  int velocity  = ml0.velocity.axes[1] = - ml0.velocity.axes[1];
	  newPos.axes[1] += (2*velocity);
	  buzzer_set_period(1000);
	  
	
	}
	

      } //if outside fence


  }
  //if moving layer
 }//end function
//   /**< for ml */


/*if the moving laye touches the upper or bottom fence then it adds
  a point to the opposite player.*/

int addPoints(MovLayer *ml, Region *fence)
 {
  Vec2 newPos;
  Vec2 *test;
  u_char axis;
  Region shapeBoundary;
  for (; ml; ml = ml->next) {
    buzzer_set_period(1);
    vec2Add(&newPos, &ml->layer->posNext, &ml->velocity);
    abShapeGetBounds(ml->layer->abShape, &newPos, &shapeBoundary);
    
    
    if(ml->layer->abShape == ml0.layer->abShape){
      
      
      
      if (shapeBoundary.topLeft.axes[1] < fence->topLeft.axes[1]){
	
	player2++;
	
	if(player2<3){
	  
	  drawChar5x7(70,screenWidth+20,'0' + player2,COLOR_RED,COLOR_BLACK);
	  //	  buzzer_set_period(1000); 
	  toggle(1);
	  return 0;
	  
	}
	else{
	  
	  drawString5x7(70,screenWidth+20,"GAME WON",COLOR_RED,COLOR_BLACK);
	  //	  buzzer_set_period(D); 
	  return 1;
	  
	}
      }
    }
    
    if(ml->layer->abShape == ml0.layer->abShape){
      
      if (shapeBoundary.botRight.axes[1] >  fence->botRight.axes[1]){
	  
	player1++;
	
	  if(player1<3){
	    
	    drawChar5x7(70,0,'0' + player1,COLOR_RED,COLOR_BLACK);
	    toggle(1);
	    return 0;
	  }
	  else{
	    
	    drawString5x7(70,0,"GAME WON",COLOR_RED,COLOR_BLACK);
	    
	    return 1; 
	    
	  }
      }
      
      
    }//if outside fence
    
    return 0;
  }
  //if moving layer
 }//end function
//   /**< for ml */




u_int bgColor = COLOR_BLACK;     /**< The background color */
int redrawScreen = 1;           /**< Boolean for whether screen needs to be redrawn */

Region fieldFence;		/**< fence around playing field  */


/** Initializes everything, enables interrupts and green LED, 
 *  and handles the rendering for the screen
 */
void main()
{
  P1DIR |= GREEN_LED;		/**< Green led on when CPU on */		
  P1OUT |= GREEN_LED;

  configureClocks();
  lcd_init();
  shapeInit();
  buzzer_init();
  p2sw_init(15);
 
  shapeInit();

  layerInit(&layer0);
  layerDraw(&layer0);


  layerGetBounds(&fieldLayer, &fieldFence);
  
  
  enableWDTInterrupts();      /**< enable periodic interrupt */
  or_sr(0x8);	              /**< GIE (enable interrupts) */
  
  drawString5x7(0,0,"Player One:",COLOR_RED,COLOR_BLACK);
  drawString5x7(0,screenWidth+20,"Player Two:",COLOR_RED,COLOR_BLACK);
  
  for(;;) { 
    while (!redrawScreen) { /**< Pause CPU if screen doesn't need updating */
      P1OUT &= ~GREEN_LED;    /**< Green led off witHo CPU */
      or_sr(0x10); 	      /**< CPU OFF */
    }
    
   
    
    P1OUT |= GREEN_LED;       /**< Green led on when CPU on */
    redrawScreen = 0;
    movLayerDraw(&ml0, &layer0);
  }
}

/** Watchdog timer interrupt handler. 15 interrupts/sec */
void wdt_c_handler()
{
  static short count = 0;
  P1OUT |= GREEN_LED;		      /**< Green LED on when cpu on */
  count ++;
  
  if (count == 15) {
    mlAdvance(&ml0, &fieldFence);

    bounce(&ml0,&fieldFence);
    // buzzer_set_period(1000);
    if(addPoints(&ml0,&fieldFence)==1){
      buzzer_set_period(D);
      toggle(0);
      WDTCTL = WDTPW +WDTHOLD;
      //drawString5x7(70,0,"TESTING",COLOR_RED,COLOR_BLACK);
      
	}


    /* read the button inputto mode the paddles from left to right*/
    
    if ((~BUTTONS) & P1LEFT){   

      ml2.velocity.axes[0]=-2;
      
    }

    else if((~BUTTONS) & P1RIGHT){

      ml2.velocity.axes[0]=2;
      
    }
    else {
      ml2.velocity.axes[0]=0;
    }

    if  ((~BUTTONS) & P2LEFT){

      ml1.velocity.axes[0]=-2;
      
    }

    else if((~BUTTONS) & P2RIGHT){

      ml1.velocity.axes[0]=2;
      
    }


    else {
    
      ml1.velocity.axes[0]=0;
      
      
    }
      redrawScreen = 1;
    count = 0;
  } 
  P1OUT &= ~GREEN_LED;		    /**< Green LED off when cpu off */
}
