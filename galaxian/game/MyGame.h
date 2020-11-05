#pragma once

class CMyGame : public CGame
{
public:
	// Game variables
	int score; // player score
	int bonusThreshold; // used for level 2 asteroids bonus item
	int level; // current level
	int specialAmmo; // player top gun standard ammo item
	int superAmmo; // player super ammo item
	int healthPickup; // player health item
	int shieldPickup; // player shield item
	int burstshots; // control of enemy type 2 shooting frequency
	int firerate; // player main shots firerate for continuous shooting

	long transTimer; // player top cannon animation sequence timer
	long levelTimer; // stores a specified level time duration

	bool respawning; // player respawn sequence
	bool runTrans; // player animation for changing sprite images
	bool spawnOnce; // used to control spawning of single events during runtime
	bool startTime; // used to set call initial leveltimer duration function
	bool gameWon; // game over win condition

	//check level completed status
	bool levelOneCompleted; // for end game GUI to check if level 1 completed
	bool levelTwoCompleted; // for end game GUI to check if level 2 completed
	bool levelThreeCompleted; // for end game GUI to check if level 3 completed

	// used for menu
	bool helpscreenactive; // used to draw or not draw helpscreen image

	// Define sprites and other instance variables here
	CSprite player;	// Player spaceship
	CSprite startScreen; // menu background
	CSprite levelSelect; // menu image for levels
	CSprite nextButton; // menu next button
	CSprite prevButton; // menu previous button
	CSprite helpButton; // menu help button
	CSprite startButton; // menu start button
	CSprite helpscreen; // help screen image
	CSprite backButton; // back button to return to menu
	CSprite background; // background image
	CSprite backgroundPlanet; // background for end level squence
	CSprite gameoverScreen; // game over GUI
	CSprite healthbar; // black background
	CSprite healthback; // background of hud
	CSpriteRect health; // health bar

	// sprite lists
	CSpriteList items;
	CSpriteList shots;  // sprite list of shots
	CSpriteList enemies; // sprite list of all enemies
	CSpriteList enemyShots; // sprite list of enemy shots
	CSpriteList healthRegen; // sprite list of healing effect
	CSpriteList explosions; // explostions sprite list
	CSpriteList cleansweep; // clear screen after respawning player
	CSpriteList lives; // lives sprite list
	CSpriteList shields; // sprite list of player shield
	CSpriteList bossShields; //level 3 boss shield

	// sounds
	CSoundPlayer explosionsound;
	CSoundPlayer shotsound; // player standard shot sound
	CSoundPlayer shothit; // player standard shot impact sound
	CSoundPlayer supershotsound; // player super ammo shot sound
	CSoundPlayer supershothit; // player super ammo impact sound
	CSoundPlayer itemPickupSound; // hit test with items
	CSoundPlayer warningsound; // level 2 asteroid sequence
	CSoundPlayer playerHit; // player damaged by enemy shot
	CSoundPlayer healthincrease; // health item used
	CSoundPlayer lifelostsound; // sounds of losing life 
	CSoundPlayer buttonPressed; // menu buttons
	CSoundPlayer errorsound; // if player has no ammo
	CSoundPlayer rewardsound; // for level 2 after asteroid sequence
	CSoundPlayer endgamesound; // win or lose sound
	CSoundPlayer music; // in game music
	CSoundPlayer menumusic; // menu music

	// game functions
	void levelControl(); // level control
	void PlayerControl(); // player movement and behaviour
	void playerShotsControl(); // player shots damage and impact animations
	void enemyControl(); // enemy spawning control based on current level
	void enemyOne(); // for spawning enemy type 1
	void enemyTwo(); // for spawning enemy type 2
	void enemyThree(); // for spawning enemy type 3
	void enemyFour(); // for spawning enemy type 4
	void enemyBoss(); // for spawning enemy boss at end of level 3
	void enemyShotsControl(); // for spawning and behaviour of all enemy shots
	void enemyexplosionscontrol(); // explosions spawning condition for all enemies
	void livesControl(); // player lives function
	void firewallControl(); // for clearing screen of enemies during player respawn sequence
	void itemsControl(); // player items spawning
	long duration(long seconds); // duration timer used for level length and animation duration
	void updateHealth(); // player health function for healthbar hud
	void healthspritecontrol();  //control sprite postion
	void shieldControl(); // player shield function

public:
	CMyGame(void);
	~CMyGame(void);

	// Per-Frame Callback Funtions (must be implemented!)
	virtual void OnUpdate();
	virtual void OnDraw(CGraphics* g);

	// Game Life Cycle
	virtual void OnInitialize();
	virtual void OnDisplayMenu();
	virtual void OnStartGame();
	virtual void OnGameOver();
	virtual void OnTerminate();

	// Keyboard Event Handlers
	virtual void OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode);
	virtual void OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode);

	// Mouse Events Handlers
	virtual void OnMouseMove(Uint16 x, Uint16 y, Sint16 relx, Sint16 rely, bool bLeft, bool bRight, bool bMiddle);
	virtual void OnLButtonDown(Uint16 x, Uint16 y);
	virtual void OnLButtonUp(Uint16 x, Uint16 y);
	virtual void OnRButtonDown(Uint16 x, Uint16 y);
	virtual void OnRButtonUp(Uint16 x, Uint16 y);
	virtual void OnMButtonDown(Uint16 x, Uint16 y);
	virtual void OnMButtonUp(Uint16 x, Uint16 y);
};
