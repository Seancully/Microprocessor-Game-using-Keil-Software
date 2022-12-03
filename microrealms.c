#include "realm.h"
#include "stm32l031lib.h"
// include musical notes into header file
#include "musical_notes.h"

// Find types: h(ealth),s(trength),m(agic),g(old),w(eapon)
static const char FindTypes[]={'h','s','m','g','w'};


// The following arrays define the bad guys and 
// their battle properies - ordering matters!
// Baddie types : O(gre),T(roll),D(ragon),H(ag)
static const char Baddies[]={'O','T','D','H'};
// The following is 4 sets of 4 damage types
static const byte WeaponDamage[]={10,10,5,25,10,10,5,25,10,15,5,15,5,5,2,10};
#define ICE_SPELL_COST 10
#define FIRE_SPELL_COST 20
#define LIGHTNING_SPELL_COST 30
static const byte FreezeSpellDamage[]={10,20,5,0};
static const byte FireSpellDamage[]={20,10,5,0};
static const byte LightningSpellDamage[]={15,10,25,0};
static const byte BadGuyDamage[]={10,10,15,5};
static int GameStarted = 0;
static tPlayer thePlayer;
static tRealm theRealm;

void playNote(uint32_t Frequency, uint32_t Duration);


// The following defines the frequency for the music
#define CPU_FREQ 16000000

// global variable for music
static volatile uint32_t SoundDuration = 0;

// run game function
__attribute__((noreturn)) void runGame(void)  
{
	char ch;
	
	 
	enable_interrupts();
	eputs("MicroRealms on the STM32L031\r\n");	
	showHelp();		
	while(GameStarted == 0)
	{
		
		showGameMessage("Press S to start a new game\r\n");
		ch = getUserInput();			
		
		if ( (ch == 'S') || (ch == 's') )
			GameStarted = 1;
		
		// Systick timer initialization code for music and enabling GPIOA's
		RCC->IOPENR |= 1;  // enable GPIOA
		pinMode (GPIOA, 0, 1); // Make PORTA Bit 0 output
		pinMode (GPIOA, 1, 1); // Make GPIOA_1 output
		pinMode (GPIOA, 3, 1);
		pinMode (GPIOA, 4, 1);
		pinMode (GPIOA, 5, 1);
		SysTick->LOAD = 15999;   // 16MHz / 16000 = 1kHz
		SysTick->CTRL = 7;     
		
	GPIOA -> ODR = 53;
		
		// call music function for start game
		playNote(E6,200);
    while(SoundDuration != 0); // Wait
		
	} // end while
	
	initRealm(&theRealm);	
	initPlayer(&thePlayer,&theRealm);
	showPlayer(&thePlayer);
	showRealm(&theRealm,&thePlayer);
	showGameMessage("Press H for help");
	
	
	
	while (1)
	{
		ch = getUserInput();
		ch = ch | 32; // enforce lower case
		switch (ch) {
			case 'h' : {
				
				// call music function for 'h'
				playNote(A2,200);
        while(SoundDuration != 0); // Wait
				
				showHelp();
				break;
			}
			case 'w' : {
				
				// call music function for 'w'
				playNote(A8,200);
        while(SoundDuration != 0); // Wait
				
				showGameMessage("North");
				step('w',&thePlayer,&theRealm);
				break;
			}
			case 's' : {
				
				// call music function for 's'
				playNote(A7,200);
        while(SoundDuration != 0); // Wait
				
				showGameMessage("South");
				step('s',&thePlayer,&theRealm);
				break;

			}
			case 'd' : {
				
				// call music function for 'd'
				playNote(A4,200);
        while(SoundDuration != 0); // Wait
				
				showGameMessage("East");
				step('d',&thePlayer,&theRealm);
				break;
			}
			case 'a' : {
				
				// call music function for 'a'
				playNote(A2,200);
        while(SoundDuration != 0); // Wait
				
				showGameMessage("West");
				step('a',&thePlayer,&theRealm);
				break;
			}
			case 'm' : {	
				
				if (thePlayer.wealth)		
				{
					// call music function for map
					playNote(D8,200);
					while(SoundDuration != 0); // Wait
					
					showRealm(&theRealm,&thePlayer);
					thePlayer.wealth--;
				}
				else
					showGameMessage("No gold!");
				break;
			}
			case 'p' : {		
				
				showPlayer(&thePlayer);
				// call music function for 'p'
				playNote(A3,200);
				while(SoundDuration != 0); // Wait
				
				// ascii code for player
				eputs("\r\n        ////^\\\\                 ");
				eputs("\r\n        | ^   ^ |                 ");
				eputs("\r\n       @ (o) (o) @                ");
				eputs("\r\n        |   <   |                 ");
				eputs("\r\n        |  ___  |                 ");
				eputs("\r\n       \\_______/                 ");
				eputs("\r\n      ____|  |____                ");
				eputs("\r\n     /   \\__/    \\              ");
				eputs("\r\n    /              \\             ");
				eputs("\r\n   /\\_/        \\_/\\            ");
				eputs("\r\n  / /  |        |  \\ \\          ");
				eputs("\r\n ( <   |        |   > )           ");
				eputs("\r\n \\ \\ |        |  / /            ");
				eputs("\r\n  \\ \\|________| / /             ");
				eputs("\r\n   \\ \\|                         ");
				eputs("\n");
				break;
				
			} // end case
			
		} // end switch
		
	} // end while
	
} // end run game(void)

void step(char Direction,tPlayer *Player,tRealm *Realm)
{
	uint8_t new_x, new_y;
	new_x = Player->x;
	new_y = Player->y;
	byte AreaContents;
	switch (Direction) {
		case 'w' :
		{
			if (new_y > 0)
				new_y--;
			break;
		}
		case 's' :
		{
			if (new_y < MAP_HEIGHT-1)
				new_y++;
			break;
		}
		case 'd' :
		{
			if (new_x <  MAP_WIDTH-1)
				new_x++;
			break;
		}
		case 'a' :
		{
			if (new_x > 0)
				new_x--;
			break;
		}		
	} // end switch
	
	AreaContents = Realm->map[new_y][new_x];
	if ( AreaContents == '*')
	{
		showGameMessage("A rock blocks your path.");
		return;
	} // end if
	
	Player->x = new_x;
	Player->y = new_y;
	int Consumed = 0;
	switch (AreaContents)
	{
		
		// const char Baddies[]={'O','T','B','H'};
		case 'O' :{
			showGameMessage("A smelly green Ogre appears before you");
			Consumed = doChallenge(Player,0);
			break;
		}
		case 'T' :{
			showGameMessage("An evil troll challenges you");
			Consumed = doChallenge(Player,1);
			break;
		}
		case 'D' :{
			
			// call music function for dragon
			playNote(A5,150);
			while(SoundDuration != 0); // Wait
			
			showGameMessage("A smouldering Dragon blocks your way !");
			
			// ascii code for dragon
			eputs("\r\n     (  )   /\\   _                 (                                              ");
			eputs("\r\n    \\ |  ( \\ (\\.(               )                      _____                    ");
			eputs("\r\n  \\ \\\\  `  `   )\\             (  ___                 / _  \\                   ");
			eputs("\r\n  (_`   \\+   . x  ( .\\           \\/  \\____-----------/ (o)  \\_                ");
			eputs("\r\n - .-              \\+  ;          (  O                          \\____            ");
			eputs("\r\n                           )       \\_____________  `             \\  /            ");
			eputs("\r\n (__                +- .( -'.- <. - _  VVVVVVV VV V\\               \\/            ");
			eputs("\r\n (_____            ._._: <_ - <- _  (--  _AAAAAAA__A_/                  |          ");
			eputs("\r\n   .    /./.+-  . .- /  +--  - .    \\______________//_             \\_______      "); 
			eputs("\r\n   (__ ' /x  / x _/ (                                  \___'         \\     /      ");
			eputs("\r\n  , x / ( '  . / .  /                                      |          \\   /       ");
			eputs("\r\n     /  /  _/ /    +                                      /            \\/         ");
			eputs("\r\n    '  (__/                                             /                 \\ 	    ");
			
			Consumed = doChallenge(Player,2);
			
			break;
		}
		case 'H' :{
			showGameMessage("A withered hag cackles at you wickedly");
			Consumed = doChallenge(Player,3);
			break;
		}
		case 'h' :{
			
			// call music function for health elixir
			playNote(A4,150);
			while(SoundDuration != 0); // Wait
			
			showGameMessage("You find an elixer of health");
			setHealth(Player,Player->health+10);
			Consumed = 1;		
			break;
			
		}
		case 's' :{
			showGameMessage("You find a potion of strength");
			Consumed = 1;
			setStrength(Player,Player->strength+1);
			break;
		}
		case 'g' :{
			showGameMessage("You find a shiny golden nugget");
			Player->wealth++;			
			Consumed = 1;
			break;
		}
		case 'm' :{
			showGameMessage("You find a magic charm");
			Player->magic++;						
			Consumed = 1;
			break;
		}
		case 'w' :{
			Consumed = addWeapon(Player,(int)random(MAX_WEAPONS-1)+1);
			showPlayer(Player);
			break;			
		}
		case 'X' : {
			// Player landed on the exit
			eputs("A door! You exit into a new realm");
			setHealth(Player,100); // maximize health
			initRealm(&theRealm);
			showRealm(&theRealm,Player);
		}
	} // end switch
	
	if (Consumed)
		Realm->map[new_y][new_x] = '.'; // remove any item that was found
}
int doChallenge(tPlayer *Player,int BadGuyIndex)
{
	char ch;
	char Damage;
	const byte *dmg;
	int BadGuyHealth = 100;
	eputs("Press F to fight");
	ch = getUserInput() | 32; // get user input and force lower case
	if (ch == 'f')
	{
		eputs("\r\nChoose action");
		while ( (Player->health > 0) && (BadGuyHealth > 0) )
		{
			eputs("\r\n");
			// Player takes turn first
			if (Player->magic > ICE_SPELL_COST)
				eputs("(I)CE spell");
			if (Player->magic > FIRE_SPELL_COST)
				eputs("(F)ire spell");
			if (Player->magic > LIGHTNING_SPELL_COST)
				eputs("(L)ightning spell");
			if (Player->Weapon1)
			{
				eputs("(1)Use ");
				eputs(getWeaponName(Player->Weapon1));
			}	
			if (Player->Weapon2)
			{
				eputs("(2)Use ");
				eputs(getWeaponName(Player->Weapon2));
			}
			eputs("(P)unch");
			ch = getUserInput();
			switch (ch)
			{
				case 'i':
				case 'I':
				{
					eputs("FREEZE!");
					Player->magic -= ICE_SPELL_COST;
					BadGuyHealth -= FreezeSpellDamage[BadGuyIndex]+random(10);
					zap();
					break;
				}
				case 'f':
				case 'F':
				{
					eputs("BURN!");
					Player->magic -= FIRE_SPELL_COST;
					BadGuyHealth -= FireSpellDamage[BadGuyIndex]+random(10);
					zap();
					break;
				}
				case 'l':
				case 'L':
				{
					eputs("ZAP!");
					Player->magic -= LIGHTNING_SPELL_COST;
					BadGuyHealth -= LightningSpellDamage[BadGuyIndex]+random(10);
					zap();
					break;
				}
				case '1':
				{
					dmg = WeaponDamage+(Player->Weapon1<<2)+BadGuyIndex;
					eputs("Take that!");
					BadGuyHealth -= *dmg + random(Player->strength);
					setStrength(Player,Player->strength-1);
					break;
				}
				case '2':
				{
					dmg = WeaponDamage+(Player->Weapon2<<2)+BadGuyIndex;
					eputs("Take that!");
					BadGuyHealth -= *dmg + random(Player->strength);
					setStrength(Player,Player->strength-1);
					break;
				}
				case 'p':
				case 'P':
				{
					
					// call music function for punch
					playNote(G5,150);
					while(SoundDuration != 0); // Wait
					
					eputs("Thump!");
					BadGuyHealth -= 1+random(Player->strength);
					setStrength(Player,Player->strength-1);
					break;
				}
				default: {
					eputs("You fumble. Uh oh");
				}
			}
			// Bad guy then gets a go 
			
			if (BadGuyHealth < 0)
				BadGuyHealth = 0;
			Damage = (uint8_t)(BadGuyDamage[BadGuyIndex]+(int)random(5));
			setHealth(Player,Player->health - Damage);
			eputs("Health: you "); printDecimal(Player->health);
			eputs(", them " );printDecimal((uint32_t)BadGuyHealth);
			eputs("\r\n");
			
		} // end while
		
		if (Player->health == 0)
		{ // You died
			eputs("You are dead. Press Reset to restart");
			while(1);
		}
		else
		{ // You won!
			Player->wealth = (uint8_t)(50 + random(50));			
			showGameMessage("You win! Their gold is yours");			
			return 1;
		}
		
	} // end outer if
	else
	{
		showGameMessage("Our 'hero' chickens out");
		return 0;
	} // end else
}
int addWeapon(tPlayer *Player, int Weapon)
{
	char c;
	eputs("You stumble upon ");
	switch (Weapon)
	{
		case 1:
		{	
			eputs("a mighty axe");
			break;
		}
		case 2:
		{	
			eputs("a sword with mystical runes");
			break;
		}
		case 3:
		{	
			eputs("a bloody flail");
			break;
		}		
		default:
			printDecimal((uint32_t)Weapon);
	}
	if ( (Player->Weapon1) && (Player->Weapon2) )
	{
		// The player has two weapons already.
		showPlayer(Player);
		eputs("You already have two weapons\r\n");		
		eputs("(1) drop Weapon1, (2) for Weapon2, (0) skip");
		c = getUserInput();
		eputchar(c);
		switch(c)
		{
			case '0':{
				return 0; // don't pick up
			}
			case '1':{
				Player->Weapon1 = (uint8_t)Weapon;
				break;
			}
			case '2':{
				Player->Weapon2 = (uint8_t)Weapon;
				break;
			}
		}
	}
	else
	{
		if (!Player->Weapon1)
		{
			Player->Weapon1 = (uint8_t)Weapon;	
		}
		else if (!Player->Weapon2)
		{
			Player->Weapon2 = (uint8_t)Weapon;
		}
	}	
	return 1;
}// end addweapon function

const char *getWeaponName(int index)
{
	switch (index)
	{
		case 0:return "Empty"; 
		case 1:return "Axe";
		case 2:return "Sword"; 
		case 3:return "Flail"; 
	} // end switch
	return "Unknown";
} // end getWeaponName function

void setHealth(tPlayer *Player,int health)
{
	if (health > 100)
		health = 100;
	if (health < 0)
		health = 0;
	Player->health = (uint8_t)health;
	
	// if statements to turn on / off led health bars depending on health ranges and sound for each
	if (Player->health >= 74)
	{
		GPIOA -> ODR = GPIOA -> ODR | (1u << 1);
	}
	else
	{
		GPIOA -> ODR = GPIOA -> ODR & ~(1u << 1);
		playNote (C2, 350);
		while (SoundDuration != 0);
	}
	
	if (Player->health >= 49)
	{
		GPIOA -> ODR = GPIOA -> ODR | (1u << 3);
	}
	else
	{
		GPIOA -> ODR = GPIOA -> ODR & ~(1u << 3);
		// call music function for health >= 50
		playNote (C3, 350);
		while (SoundDuration != 0);
	}
	
	if (Player->health >= 24)
	{
		GPIOA -> ODR = GPIOA -> ODR | (1u << 4);
	}
	else
	{
		GPIOA -> ODR = GPIOA -> ODR & ~(1u << 4);
		// call music function for health >= 25
		playNote (C4, 350);
		while (SoundDuration != 0);
	}
	
	
	
}	// end setHealth function

void setStrength(tPlayer *Player, byte strength)
{
	if (strength > 100)
		strength = 100;
	Player->strength = strength;
} // end setStrength function
void initPlayer(tPlayer *Player,tRealm *Realm)
{
	// get the player name
	int index=0;
	byte x,y;
	char ch=0;
	
	// Initialize the player's attributes
	eputs("Enter the player's name: ");
	while ( (index < MAX_NAME_LEN) && (ch != '\n') && (ch != '\r'))
	{
		ch = getUserInput();
		if ( ch > '0' ) // strip conrol characters
		{
			
			Player->name[index++]=ch;
			eputchar(ch);
		}
	} // end while
	
	Player->name[index]=0; // terminate the name
	setHealth(Player,100);
	Player->strength=(uint8_t)(50+random(50));
	Player->magic=(uint8_t)(50+random(50));	
	Player->wealth=(uint8_t)(10+random(10));
	Player->Weapon1 = 0;
	Player->Weapon2 = 0;
	// Initialize the player's location
	// Make sure the player does not land
	// on an occupied space to begin with
	do {
		x=(uint8_t)random(MAP_WIDTH);
		y=(uint8_t)random(MAP_HEIGHT);
		} // end do
	while(Realm->map[y][x] != '.');
	Player->x=x;
	Player->y=y;
} // end initPlayer function

void showPlayer(tPlayer *Player)
{
	eputs("\r\nName: ");
	eputs(Player->name);
	eputs("health: ");
	printDecimal(Player->health);
	eputs("\r\nstrength: ");
	printDecimal(Player->strength);
	eputs("\r\nmagic: ");
	printDecimal(Player->magic);
	eputs("\r\nwealth: ");
	printDecimal(Player->wealth);	
	eputs("\r\nLocation : ");
	printDecimal(Player->x);
	eputs(" , ");
	printDecimal(Player->y);	
	eputs("\r\nWeapon1 : ");
	eputs(getWeaponName(Player->Weapon1));
	eputs(" Weapon2 : ");
	eputs(getWeaponName(Player->Weapon2));
} // end showPlayer function

void initRealm(tRealm *Realm)
{
	unsigned int x,y;
	unsigned int Rnd;
	// clear the map to begin with
	for (y=0;y < MAP_HEIGHT; y++)
	{
		for (x=0; x < MAP_WIDTH; x++)
		{
			Rnd = random(100);
			
			if (Rnd >= 98) // put in some baddies
				Realm->map[y][x]=	Baddies[random(sizeof(Baddies))];
			else if (Rnd >= 95) // put in some good stuff
				Realm->map[y][x]=	FindTypes[random(sizeof(FindTypes))];
			else if (Rnd >= 90) // put in some rocks
				Realm->map[y][x]='*'; 
			else // put in empty space
				Realm->map[y][x] = '.';	
		} // end inner for
	} // end outer for
	
	// finally put the exit to the next level in
	x = random(MAP_WIDTH);
	y = random(MAP_HEIGHT);
	Realm->map[y][x]='X';
} // end initRealm function

void showRealm(tRealm *Realm,tPlayer *Player)
{
	int x,y;
	eputs("\r\nThe Realm:\r\n");	
	for (y=0;y<MAP_HEIGHT;y++)
	{
		for (x=0;x<MAP_WIDTH;x++)
		{
			
			if ( (x==Player->x) && (y==Player->y))
				eputchar('@');
			else
				eputchar(Realm->map[y][x]);
		} // end inner for
		
		eputs("\r\n");
		
	} // end outer for
	
	eputs("\r\nLegend\r\n");
	eputs("(T)roll, (O)gre, (D)ragon, (H)ag, e(X)it\r\n");
	eputs("(w)eapon, (g)old), (m)agic, (s)trength\r\n");
	eputs("@=You\r\n");
} // end function showRealm

void showHelp()
{

	eputs("Help\r\n");
	eputs("w,s,a,d : go North, South, East, West\r\n");
	eputs("M : show map (cost: 1 gold piece)\r\n");
	eputs("(H)elp\r\n");
	eputs("(P)layer details\r\n");
} // end function showHelp

void showGameMessage(char *Msg)
{
	eputs(Msg);
	eputs("\r\nReady\r\n");	
} // end function showGameMessage

char getUserInput()
{
	char ch = 0;
	
	while (ch == 0)
		ch = egetchar();
	return ch;
} // end function getUserInput

unsigned random(unsigned range)
{
	// Implementing my own version of modulus
	// as it is a lot smaller than the library version
	// To prevent very long subtract loops, the
	// size of the value returned from prbs has been
	// restricted to 8 bits.
	unsigned Rvalue = (prbs()&0xff);
	while (Rvalue >= range)
		Rvalue -= range; 
	return Rvalue;
} // end function random

void zap()
{

} // end function zap
uint32_t prbs()
{

	static unsigned long shift_register=0xa5a5a5a5;
	unsigned long new_bit=0;
	static int busy=0; // need to prevent re-entrancy here
	if (!busy)
	{
		busy=1;
		new_bit= ((shift_register & (1<<27))>>27) ^ ((shift_register & (1<<30))>>30);
		new_bit= ~new_bit;
		new_bit = new_bit & 1;
		shift_register=shift_register << 1;
		shift_register=shift_register | (new_bit);
		busy=0;
	}
	return shift_register & 0x7ffffff; // return 31 LSB's
}

// systick interrupt handler function
void SysTick_Handler(void)
{
	if (SoundDuration > 0)
	{
		GPIOA->ODR ^= 1; // Toggle Port A bit 0
		SoundDuration --;
	}
} // end Systick handler function

// last music function
void playNote(uint32_t Frequency, uint32_t Duration)
{
SysTick->LOAD = CPU_FREQ / (2 * Frequency);
SoundDuration = (Duration * 2 * Frequency)  / 1000;
while(SoundDuration != 0); // Wait
} // end playNote function
