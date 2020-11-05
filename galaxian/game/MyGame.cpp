#include "stdafx.h"
#include "MyGame.h"
#include <sstream>

CMyGame::CMyGame(void) :
	startScreen(400, 300, "startScreen.bmp", CColor::Magenta(), 0),
	levelSelect(400, 345, "level1.bmp", CColor::Magenta(), 0),
	nextButton(669, 334, "btnNextUnpressed.bmp", CColor::Magenta(), 0),
	prevButton(125, 334, "btnPrevUnpressed.bmp", CColor::Magenta(), 0),
	helpButton(395, 170, "btnHelp.bmp", CColor::Magenta(), 0),
	startButton(395, 100, "btnStart.bmp", CColor::Magenta(), 0),
	helpscreen(400, 300, "helpscreen.bmp", CColor::Magenta(), 0),
	backButton(115, 450, "btnBack.bmp", CColor::Magenta(), 0),
	healthbar(400, 555, "hud2.bmp", CColor::Magenta(), 0),
	healthback(650, 585, "healthback.bmp", CColor::White(), 0),
	health(650, 585, 285, 45, CColor::Green(), CColor::Black(), 0),
	background(1280, 300, "background.bmp", CColor::Black(), 0),
	backgroundPlanet(1200, 270, "planetOne.bmp", CColor::Black(), 0),
	gameoverScreen(400, 300, "gameover.bmp", CColor::Magenta(), 0)
	// to initialise more sprites here use a comma-separated list
{
	// TODO: add initialisation here

}

CMyGame::~CMyGame(void)
{
	// TODO: add destruction code here
}

/////////////////////////////////////////////////////
// Per-Frame Callback Funtions (must be implemented!)

void CMyGame::OnUpdate()
{
	if (IsMenuMode() || IsGameOver()) return;

	long t = GetTime();

	// update sprites
	player.Update(t);
	background.Update(t);
	backgroundPlanet.Update(t);

	// repeat background 
	if (background.GetX() <= -640) background.SetPosition(1280, 300);

	// update sprite lists
	for (CSprite* shot : shots) shot->Update(t);
	for (CSprite* explode : explosions) explode->Update(t);
	for (CSprite* item : items) item->Update(t);
	for (CSprite* regen : healthRegen) regen->Update(t);
	for (CSprite* firewall : cleansweep) firewall->Update(t);
	for (CSprite* enemy : enemies) enemy->Update(t);
	for (CSprite* eShot1 : enemyShots) eShot1->Update(t);
	for (CSprite* shield : shields) shield->Update(t);
	for (CSprite* bossShield : bossShields) bossShield->Update(t);

	// end game lose condition
	if (lives.size() <= 0 && player.GetHealth() <= 0 || score < 0)
	{
		endgamesound.Play("youlose.wav");
		GameOver();
	}

	//Level control function
	levelControl();
	// enemy control function
	enemyControl();
	// enemy shots control function
	enemyShotsControl();
	// player control function
	PlayerControl();
	// player shots control function
	playerShotsControl();
	// player lives control function
	livesControl();
	// clear screen control function
	firewallControl();
	// item spawn and behaviour function
	itemsControl();
	//health sprite position
	healthspritecontrol();
	// shield control
	shieldControl();
	// control of enemy explosions and deletion
	enemyexplosionscontrol();

	// end of level scene change for final battle
	if (t < levelTimer && t > levelTimer - 5000)
	{
		backgroundPlanet.SetMotion(-100, 0);
		if (backgroundPlanet.GetX() <= 800)
		{
			background.SetMotion(0, 0);
			backgroundPlanet.SetMotion(0, 0);
		}
	}
	// reset planet position
	if (backgroundPlanet.GetX() <= -300)
	{
		backgroundPlanet.SetPosition(1200, 270);
		backgroundPlanet.SetMotion(0, 0);
		// switch planet image ready for next level end sequence
		if (level == 2) backgroundPlanet.SetImage("planetTwo");
		else if (level == 3) backgroundPlanet.SetImage("planetThree");
	}
}

void CMyGame::PlayerControl()
{
	// disable respawn sequence
	if (player.GetX() >= 65)
	{
		respawning = false;
		player.SetSpeed(0);
	}

	// screen boundaries
	if (player.GetX() < 65 && respawning == false)
	{
		player.SetPosition(65, player.GetY());
	}
	if (player.GetX() > 730 && GetTime() < levelTimer - 5000)
	{
		player.SetPosition(730, player.GetY());
	}
	// restrict x axis play area during end level battle
	else if (player.GetX() > 400 && GetTime() > levelTimer - 5000)
	{
		player.SetPosition(400, player.GetY());
	}
	if (player.GetY() < 32)
	{
		player.SetPosition(player.GetX(), 32);
	}
	if (player.GetY() > 485)
	{
		player.SetPosition(player.GetX(), 485);
	}
}

void CMyGame::playerShotsControl()
{
	// continuous shots at a set fire rate, or repeated manual click for faster fire rate.

	if (IsLButtonDown() && GetTime() > levelTimer - 79000 && respawning == false)
	{
		firerate -= 3;
		if (firerate == 27)
		{
			// static image has been used to avoid Nullptr error
			CSprite* newShot = new CSprite(player.GetX() + 55, player.GetY() - 27, "bottomGunSINGLE.bmp", CColor::Black(), GetTime());
			//CSprite* newShot = new CSprite(player.GetX() + 55, player.GetY() - 27, 0, 0, GetTime());
			newShot->AddImage("bottomGun.bmp", "basicShot", 4, 1, 0, 0, 3, 0, CColor::Black());
			newShot->SetAnimation("basicShot");
			newShot->SetHealth(20);
			newShot->SetDirection(90);
			newShot->SetSpeed(1000);
			shotsound.Play("shot.wav");
			shots.push_back(newShot);
		}
		if (firerate == 0)
		{
			firerate = 30;
		}
	}


	for (CSprite* shot : shots)
	{
		// delete a shot if outside of the screen area
		if (shot->GetY() < 0 || shot->GetY() > 600 || shot->GetX() < 0 || shot->GetX() > 800)
		{
			shot->Delete();
		}

		for (CSprite* enemy : enemies)
		{
			// different animation to play depending on which shot used, differenciated by assigning shots a health equal to their damage done.
			if (shot->HitTest(enemy) && bossShields.size() == 0)
			{
				CSprite* bulletOneExplosion = new CSprite(shot->GetX(), shot->GetY(), 0, 0, GetTime());

				if (shot->GetHealth() == 20)
				{
					bulletOneExplosion->AddImage("bottomGunSplash.bmp", "bulletExplode", 3, 1, 0, 0, 2, 0, CColor::Black());
					enemy->SetHealth(enemy->GetHealth() - 20);
					shothit.Play("stdshothit.wav");
				}
				if (shot->GetHealth() == 100)
				{
					bulletOneExplosion->AddImage("topGunSplash.bmp", "bulletExplode", 3, 1, 0, 0, 2, 0, CColor::Black());
					enemy->SetHealth(enemy->GetHealth() - 100);
				}
				if (shot->GetHealth() == 200)
				{
					bulletOneExplosion->AddImage("superShotSplash.bmp", "bulletExplode", 3, 1, 0, 0, 2, 0, CColor::Black());
					enemy->SetHealth(enemy->GetHealth() - 200);
					supershothit.Play("supershothit.wav");
				}

				bulletOneExplosion->SetAnimation("bulletExplode");
				bulletOneExplosion->Die(300);
				explosions.push_back(bulletOneExplosion);
				shot->Delete();
			}
		}
		// player shots with level 3 boss shields - no damage to enemy while active
		for (CSprite* bossshield : bossShields)
		{
			if (shot->HitTest(bossshield))
			{
				CSprite* bulletOneExplosion = new CSprite(shot->GetX(), shot->GetY(), 0, 0, GetTime());

				if (shot->GetHealth() == 20)
				{
					bulletOneExplosion->AddImage("bottomGunSplash.bmp", "bulletExplode", 3, 1, 0, 0, 2, 0, CColor::Black());
					shothit.Play("stdshothit.wav");
				}
				if (shot->GetHealth() == 100)
				{
					bulletOneExplosion->AddImage("topGunSplash.bmp", "bulletExplode", 3, 1, 0, 0, 2, 0, CColor::Black());
				}
				if (shot->GetHealth() == 200)
				{
					bulletOneExplosion->AddImage("superShotSplash.bmp", "bulletExplode", 3, 1, 0, 0, 2, 0, CColor::Black());
					supershothit.Play("supershothit.wav");
				}
				bulletOneExplosion->SetAnimation("bulletExplode");
				bulletOneExplosion->Die(300);
				explosions.push_back(bulletOneExplosion);
				shot->Delete();
			}
		}
	}
	shots.remove_if(deleted);
	explosions.remove_if(deleted);
}

void CMyGame::levelControl()
{
	long t = GetTime();

	// level 1 enemies control
	if (level == 1)
	{
		if (startTime == false)
		{
			//set spawning duration for end level condition
			levelTimer = duration(80000);
			startTime = true;
		}
		//wait 5 seconds before spawwning enemy type 1 & stop spawning 5 seconds before level timer
		if (t < levelTimer - 5000 && t > levelTimer - 75000)
		{
			if (rand() % 160 == 0) enemyOne();
			// wait 10 seconds before spawning enemy type 2
			if (t > levelTimer - 70000)
			{
				if (rand() % 140 == 0) enemyTwo();
			}
		}
		// warning sound for boss arrival
		if (GetTime() > levelTimer - 3500 && GetTime() < levelTimer - 1000)
		{
			if (spawnOnce == false)
			{
				warningsound.Play("alert.wav");
				spawnOnce = true;
			}
		}
		// in last second of level timer spawn single enemy type 3
		else if (t < levelTimer && t > levelTimer - 1000)
		{
			if (spawnOnce == true)
			{
				enemyThree();
				spawnOnce = false;
			}
		}
		// current time is more than level time 
		else if (t > levelTimer)
		{
			if (enemies.size() == 0)
			{
				level = 2;
				startTime = false;
				spawnOnce = false;
				levelOneCompleted = true;
				// reinitiate motion
				backgroundPlanet.SetMotion(-100, 0);
				background.SetMotion(-100, 0);
			}
		}
	}
	// level 2 enemies control
	if (level == 2)
	{
		if (startTime == false)
		{
			levelTimer = duration(80000);
			startTime = true;
		}
		//wait 5 seconds before spawning enemy type 1 & 2 & stop spawning 5 seconds before level timer
		if (t < levelTimer - 5000 && t > levelTimer - 75000)
		{
			if (t < levelTimer - 50000)
			{
				if (rand() % 140 == 0) enemyOne();
				if (rand() % 170 == 0) enemyTwo();
			}
			// spawn wave of enemy type 1
			if (t > levelTimer - 50000 && t < levelTimer - 45000)
			{
				if (spawnOnce == false)
				{
					// create 6 enemies
					for (int i = 0; i < 6; i++) enemyOne();
					spawnOnce = true;
				}
			}
			// wait 35 seconds before spawning asteroids
			if (t > levelTimer - 45000)
			{
				// 5 seconds break to display on screen text
				//play sound once
				if (t < levelTimer - 40000)
				{
					if (spawnOnce == true)
					{
						warningsound.Play("warning.wav");
						spawnOnce = false;
					}
				}
				if (t > levelTimer - 40000)
				{
					if (rand() % 40 == 0) enemyFour();
					if (spawnOnce == false)
					{
						// create 8 asteroids
						for (int i = 0; i < 8; i++) enemyFour();
						spawnOnce = true;
					}
				}
			}
		}
		// bonus reward if score loss is less than bonus threshold amount
		else if (t < levelTimer && t > levelTimer - 5000)
		{
			if (bonusThreshold >= -30 && bonusThreshold <= 0)
			{
				rewardsound.Play("spree.wav");
				if (lives.size() < 3)
				{
					CSprite* reward1 = new CSprite(400, 300, "asteroidsreward1.bmp", CColor::Black(), GetTime());
					reward1->Die(1000);
					explosions.push_back(reward1);
				}
				// add +1 life to front of lives sprite list
				if (lives.size() == 0)
				{
					CSprite* newLife = new CSprite(43, 532, "ship_life.bmp", CColor::Black(), GetTime());
					lives.push_front(newLife);
				}
				else if (lives.size() == 1)
				{
					CSprite* newLife = new CSprite(115, 532, "ship_life.bmp", CColor::Black(), GetTime());
					lives.push_front(newLife);
				}
				else if (lives.size() == 2)
				{
					CSprite* newLife = new CSprite(187, 532, "ship_life.bmp", CColor::Black(), GetTime());
					lives.push_front(newLife);
				}
				else
				{
					CSprite* reward2 = new CSprite(400, 300, "asteroidsreward2.bmp", CColor::Black(), GetTime());
					reward2->Die(1000);
					explosions.push_back(reward2);

					shieldPickup += 2;
					healthPickup += 2;
				}

				// stop condition running 
				bonusThreshold = 9999;
			}
			// no reward message
			else if (bonusThreshold < -30)
			{
				CSprite* reward3 = new CSprite(400, 300, "asteroidsreward3.bmp", CColor::Black(), GetTime());
				reward3->Die(1000);
				explosions.push_back(reward3);

				// stop condition running 
				bonusThreshold = 9999;
			}
			// warning sound for boss arrival
			if (t > levelTimer - 3500 && t < levelTimer - 1000)
			{
				if (spawnOnce == true)
				{
					warningsound.Play("alert.wav");
					spawnOnce = false;
				}
			}
			// spawn end level boss
			if (t > levelTimer - 1000)
			{
				if (spawnOnce == false)
				{
					for (int i = 0; i < 10; i++) enemyTwo();
					enemyThree();
					spawnOnce = true;
				}
			}
		}
		// end level condition
		else if (t > levelTimer)
		{
			if (enemies.size() == 0)
			{
				level = 3;
				startTime = false;
				spawnOnce = false;
				levelTwoCompleted = true;
				backgroundPlanet.SetMotion(-100, 0);
				background.SetMotion(-100, 0);
			}
		}
	}
	// level 3 enemies control
	if (level == 3)
	{
		if (startTime == false)
		{
			levelTimer = duration(80000);
			startTime = true;
		}
		if (t < levelTimer - 5000 && t > levelTimer - 75000)
		{
			if (rand() % 170 == 0) enemyOne();
			if (rand() % 140 == 0) enemyTwo();

			// spawn enemy type 4 at two 10 second intervals
			if (t > levelTimer - 60000 && t < levelTimer - 50000)
			{
				if (rand() % 75 == 0) enemyFour();
			}
			else if (t > levelTimer - 35000 && t < levelTimer - 25000)
			{
				if (rand() % 100 == 0) enemyFour();
			}
		}
		// warning sound for boss arrival
		if (GetTime() > levelTimer - 3500 && GetTime() < levelTimer - 1000)
		{
			if (spawnOnce == false)
			{
				warningsound.Play("alert.wav");
				spawnOnce = true;
			}
		}
		// spawn end level boss condition
		else if (t < levelTimer && t > levelTimer - 1000)
		{
			if (spawnOnce == true)
			{
				enemyBoss();
				spawnOnce = false;
			}
		}
		// game over condition
		else if (t > levelTimer)
		{
			if (enemies.size() == 0)
			{
				gameWon = true;
				levelThreeCompleted = true;
				endgamesound.Play("youwin.wav");
				GameOver();
			}
		}
	}
}

void CMyGame::enemyControl()
{
	// enemy behaviours differenciated by assigning enemies a "status"
	for (CSprite* enemy : enemies)
	{
		// for movement towards play and rotation
		CVector dir = CVector(player.GetPosition() - enemy->GetPosition());
		double dirToEnemyX = enemy->GetX() - player.GetX();
		double dirToEnemyY = enemy->GetY() - player.GetY();

		// leaving the screen area
		if (enemy->GetX() < -70)
		{
			CSprite* lifeloss = new CSprite(25, 270, "enemyEscaped.bmp", CColor::Black(), GetTime());
			lifeloss->Die(2000);
			cleansweep.push_back(lifeloss);

			// player loses life
			enemy->SetHealth(0);
			player.SetHealth(0);
			updateHealth();
		}

		// hit test with player
		if (enemy->HitTest(&player) && shields.size() == 0)
		{
			// kill enemy
			if (enemy->GetStatus() == 1 || enemy->GetStatus() == 2 || enemy->GetStatus() == 4)
			{
				enemy->SetHealth(0);
			}
			// reduce player health
			if (player.GetHealth() > 0)
			{
				if (enemy->GetStatus() == 1) player.SetHealth(player.GetHealth() - 40);
				if (enemy->GetStatus() == 2) player.SetHealth(player.GetHealth() - 20);
				if (enemy->GetStatus() == 4) player.SetHealth(player.GetHealth() - 20);
				if (player.GetHealth() < 0) player.SetHealth(0);
				updateHealth();
			}
		}

		// hit test with shield
		for (CSprite* shield : shields)
		{
			if (enemy->HitTest(shield, 1))
			{
				if (enemy->GetStatus() == 1 || enemy->GetStatus() == 2 || enemy->GetStatus() == 4)
				{
					enemy->SetHealth(0);
				}
			}
		}

		// enemy type 2 behaviour
		if (enemy->GetStatus() == 2)
		{
			enemy->SetMotion(dir);
			enemy->SetSpeed(130);
			// rotate to angle to face player
			enemy->SetRotation(atan2(-dirToEnemyY, dirToEnemyX) * 180 / M_PI);
		}
		// enemy type 3 behaviour
		if (enemy->GetStatus() == 3)
		{
			// remove enemy thruster
			if (enemy->GetX() > 650 && enemy->GetX() < 655)
			{
				enemy->LoadImage("enemy02_red.bmp", "enemy3red", CColor::Black());
				enemy->SetImage("enemy3red");
			}
			// movement
			if (enemy->GetX() <= 650)
			{
				// stay still whilst player is respawning to give recovery time
				if (respawning == false)
				{
					// if player is within firing range don't move otherwise follow Y motion
					if (enemy->GetY() <= player.GetY() + 20 && enemy->GetY() >= player.GetY() - 20)
					{
						enemy->SetMotion(0, 0);
					}
					else
					{
						enemy->SetMotion(0, dir.GetY());
						enemy->SetSpeed(400);
					}
				}
				else
					enemy->SetMotion(0, 0);
			}
		}
		// enemy type 4 behaviour
		if (enemy->GetStatus() == 4)
		{
			if (enemy->GetX() < 0)
			{
				score -= 50;
				bonusThreshold -= 5;
				enemy->SetX(850);
			}
		}
		// level 3 enemy boss behaviour
		if (enemy->GetStatus() == 5)
		{
			// remove enemy thruster
			if (enemy->GetX() > 650 && enemy->GetX() < 655)
			{
				enemy->LoadImage("boss.bmp", "bossimage", CColor::Magenta());
				enemy->SetImage("bossimage");
			}
			// movement
			if (enemy->GetX() <= 650)
			{
				// stay still whilst player is respawning to give recovery time
				if (respawning == false)
				{
					// if player is within firing range don't move otherwise follow Y motion
					if (enemy->GetY() <= player.GetY() + 25 && enemy->GetY() >= player.GetY() - 25)
					{
						enemy->SetMotion(0, 0);
					}
					else
					{
						enemy->SetMotion(0, dir.GetY());
						enemy->SetSpeed(350);
					}
				}
				else
					enemy->SetMotion(0, 0);
			}
			// keep enemy in screen
			if (enemy->GetY() < 130)
			{
				enemy->SetY(130);
			}
			if (enemy->GetY() > 410)
			{
				enemy->SetY(410);
			}
			// spawn shield
			if (bossShields.size() == 0 && rand() % 270 == 0)
			{
				CSprite* newShield = new CSprite(enemy->GetX(), enemy->GetY(), "bossshield.png", CColor::Black(), GetTime());
				newShield->SetSize(280, 320);
				newShield->Die(2000);
				bossShields.push_back(newShield);
			}
			// control shield position
			for (CSprite* bossShield : bossShields)
			{
				bossShield->SetPosition(enemy->GetPosition());
			}
			bossShields.remove_if(deleted);
		}
	}
}

void CMyGame::enemyexplosionscontrol()
{
	for (CSprite* enemy : enemies)
	{
		// delete enemies. score and animation dependant on an assigned "status" to differenciate enemies in list
		if (enemy->GetHealth() <= 0)
		{
			if (enemy->GetStatus() == 1)
			{
				CSprite* pExplosion = new CSprite(enemy->GetX(), enemy->GetY(), 0, 0, GetTime());
				pExplosion->AddImage("Explosion3.bmp", "explode1", 3, 4, 0, 3, 2, 0, CColor::Black());
				pExplosion->SetAnimation("explode1", 24);
				explosionsound.Play("explosion.wav");
				pExplosion->Die(500);
				explosions.push_back(pExplosion);

				score += 50;
			}
			if (enemy->GetStatus() == 2)
			{
				CSprite* pExplosion = new CSprite(enemy->GetX(), enemy->GetY(), 0, 0, GetTime());
				pExplosion->AddImage("explosionType2.bmp", "explode2", 5, 5, 0, 4, 3, 0, CColor::Black());
				pExplosion->SetAnimation("explode2", 36);
				explosionsound.Play("explosion.wav");
				pExplosion->Die(550);
				explosions.push_back(pExplosion);

				score += 30;
			}
			if (enemy->GetStatus() == 3)
			{
				CSprite* pExplosion = new CSprite(enemy->GetX(), enemy->GetY(), 0, 0, GetTime());
				pExplosion->AddImage("explosionBoss.bmp", "explode4", 5, 1, 0, 0, 4, 0, CColor::Black());
				pExplosion->SetAnimation("explode4");
				explosionsound.Play("explosion.wav");
				pExplosion->Die(500);
				explosions.push_back(pExplosion);

				score += 100;
			}
			if (enemy->GetStatus() == 4)
			{
				CSprite* pExplosion = new CSprite(enemy->GetX(), enemy->GetY(), 0, 0, GetTime());
				pExplosion->AddImage("explosion.bmp", "explode3", 5, 5, 0, 0, 4, 4, CColor::Black());
				pExplosion->SetAnimation("explode3", 36);
				explosionsound.Play("explosion.wav");
				pExplosion->Die(700);
				explosions.push_back(pExplosion);

				score += 20;
			}
			if (enemy->GetStatus() == 5)
			{
				score += 500;
			}
			enemy->Delete();
		}
	}
	enemies.remove_if(deleted);
	explosions.remove_if(deleted);
}

void CMyGame::enemyOne()
{
	// static image has been used before animation to avoid Nullptr error
	// spawn enemy type 1
	CSprite* newEnemy1 = new CSprite(850, rand() % 430 + 32, "staticEnemySingle.bmp", CColor::Black(), GetTime());
	newEnemy1->AddImage("staticEnemyTest.bmp", "staticFly", 3, 1, 0, 0, 2, 0, CColor::Black());
	newEnemy1->SetAnimation("staticFly");
	newEnemy1->SetDirection(270);
	newEnemy1->SetSpeed(140);
	newEnemy1->SetHealth(100);
	// set status to differenciate enemy in list
	newEnemy1->SetStatus(1);
	enemies.push_back(newEnemy1);
}

void CMyGame::enemyTwo()
{
	// spawn enemy type 2
	CSprite* newEnemy2 = new CSprite(850, rand() % 430 + 32, "enemyTwoTest.bmp", CColor::Black(), GetTime());
	newEnemy2->SetHealth(60);
	// set status to differenciate enemy in list
	newEnemy2->SetStatus(2);
	enemies.push_back(newEnemy2);
}

void CMyGame::enemyThree()
{
	// spawn enemy type 3
	CSprite* newEnemy3 = new CSprite(950, rand() % 430 + 32, "enemy02.bmp", CColor::Black(), GetTime());
	newEnemy3->SetDirection(270);
	newEnemy3->SetSpeed(200);
	// set an initial health and scale it to a higher amount depending on players items to keep difficulty
	newEnemy3->SetHealth(500 + (250 * superAmmo) + (50 * specialAmmo));
	// set status to differenciate enemy in list
	newEnemy3->SetStatus(3);
	enemies.push_back(newEnemy3);
}

void CMyGame::enemyFour()
{
	// spawn enemy type 4
	CSprite* newAsteroid = new CSprite(850, rand() % 430 + 32, "rockSingle.bmp", CColor::White(), GetTime());
	newAsteroid->AddImage("rock.png", "asteroid", 16, 1, 0, 0, 15, 0, CColor::Black());
	newAsteroid->SetAnimation("asteroid");
	newAsteroid->SetDirection(270);
	newAsteroid->SetSpeed(rand() % 350 + 150);
	newAsteroid->SetHealth(40);
	newAsteroid->SetOmega(rand() % 100 + 25);
	// set status to differenciate enemy in list
	newAsteroid->SetStatus(4);
	enemies.push_back(newAsteroid);
}

void CMyGame::enemyBoss()
{
	// spawn final boss enemy
	CSprite* newEnemyBoss = new CSprite(950, 300, "bossThrusters.bmp", CColor::Magenta(), GetTime());
	newEnemyBoss->SetDirection(270);
	newEnemyBoss->SetSpeed(200);
	// set an initial health and scale it to a higher amount depending on players items to keep difficulty
	newEnemyBoss->SetHealth(2000.0 + (350 * superAmmo) + (70 * specialAmmo));
	// set status to differenciate enemy in list
	newEnemyBoss->SetStatus(5);
	enemies.push_back(newEnemyBoss);
}

void CMyGame::enemyShotsControl()
{
	// create shots differenciated by assigning shots a health equal to their damage done.
	for (CSprite* enemy : enemies)
	{
		//enemy 1 shots
		if (enemy->GetStatus() == 1)
		{
			if (rand() % 160 == 0)
			{
				CSprite* newBomb = new CSprite(enemy->GetX() - 50, enemy->GetY() + 1, "shot5.bmp", CColor::White(), GetTime());
				newBomb->AddImage("shot4.bmp", "staticshoot1", 2, 1, 0, 0, 1, 0, CColor::White());
				newBomb->SetAnimation("staticshoot1");
				newBomb->SetDirection(270);
				newBomb->SetSpeed(500);
				newBomb->SetHealth(20);
				enemyShots.push_back(newBomb);
			}
		}
		// enemy 2 shots
		if (enemy->GetStatus() == 2)
		{
			burstshots -= 2;
			if (burstshots < 20)
			{
				CSprite* newBomb = new CSprite(enemy->GetX(), enemy->GetY(), "shot.bmp", CColor::Black(), GetTime());
				CVector dir = CVector(player.GetPosition() - enemy->GetPosition());
				newBomb->SetMotion(dir);
				newBomb->SetSpeed(400);
				newBomb->SetHealth(5);
				enemyShots.push_back(newBomb);
			}

			if (burstshots <= 0)
			{
				burstshots = 480;
			}
		}
		//enemy 3 shots
		if (enemy->GetStatus() == 3)
		{
			// shoot if player is in sight and not respawning
			if (enemy->GetY() <= player.GetY() + 25 && enemy->GetY() >= player.GetY() - 25 && respawning == false)
			{
				if (rand() % 1 == 0)
				{
					CSprite* newBomb = new CSprite(enemy->GetX() - 126, enemy->GetY(), "shot3.bmp", CColor::Black(), GetTime());
					newBomb->SetDirection(270);
					newBomb->SetSpeed(1300);
					newBomb->SetHealth(2);
					enemyShots.push_back(newBomb);
				}
			}
		}
		// level 3 boss shots
		if (enemy->GetStatus() == 5)
		{
			// while not respawning
			if (respawning == false)
			{
				// inner guns
				if (enemy->GetY() <= player.GetY() + 80 && enemy->GetY() >= player.GetY() - 80 && rand() % 1 == 0)
				{
					for (int i = -51; i <= 51; i += 102)
					{
						CSprite* newBomb = new CSprite(enemy->GetX() - 108, enemy->GetY() - i, "shot3.bmp", CColor::Black(), GetTime());
						newBomb->SetDirection(270);
						newBomb->SetSpeed(1000);
						newBomb->SetHealth(2);
						enemyShots.push_back(newBomb);
					}
				}
				// center ship bullets
				burstshots -= 2;
				if (burstshots < 30 && rand() % 1 == 0)
				{
					CSprite* newBomb = new CSprite(enemy->GetX() + 50, enemy->GetY(), "shot.bmp", CColor::Black(), GetTime());
					CVector dir = CVector(player.GetPosition() - enemy->GetPosition());
					newBomb->SetMotion(dir);
					newBomb->SetSpeed(450);
					newBomb->SetHealth(5);
					enemyShots.push_back(newBomb);
				}
				// reset burst shot timer
				if (burstshots <= 0)
				{
					burstshots = 180;
				}
				// outer guns
				if (rand() % 100 == 0)
				{
					for (int i = -119; i <= 119; i += 238)
					{
						CSprite* newBomb = new CSprite(enemy->GetX() - 75, enemy->GetY() + i, "shot5.bmp", CColor::White(), GetTime());
						newBomb->AddImage("shot4.bmp", "staticshoot1", 2, 1, 0, 0, 1, 0, CColor::White());
						newBomb->SetAnimation("staticshoot1");
						newBomb->SetDirection(270);
						newBomb->SetSpeed(500);
						newBomb->SetHealth(20);
						enemyShots.push_back(newBomb);
					}
				}
			}
		}
	}
	enemies.remove_if(deleted);

	// shot collision with player and shield
	for (CSprite* enemyShot : enemyShots)
	{
		if (enemyShot->GetX() < 0 || enemyShot->GetX() > 800 || enemyShot->GetY() < 0 || enemyShot->GetY() > 600)
		{
			enemyShot->Delete();
		}

		if (enemyShot->HitTest(&player) && shields.size() == 0)
		{
			CSprite* bulletenemyExplosion = new CSprite(enemyShot->GetX(), enemyShot->GetY(), 0, 0, GetTime());
			bulletenemyExplosion->AddImage("testcrash.bmp", "rocketExplode", 3, 1, 0, 0, 2, 0, CColor::Black());
			bulletenemyExplosion->SetAnimation("rocketExplode");
			bulletenemyExplosion->Die(300);
			explosions.push_back(bulletenemyExplosion);

			playerHit.Play("boom2.wav");
			enemyShot->Delete();

			if (player.GetHealth() > 0)
			{
				if (enemyShot->GetHealth() == 2) player.SetHealth(player.GetHealth() - 2);
				if (enemyShot->GetHealth() == 5) player.SetHealth(player.GetHealth() - 5);
				if (enemyShot->GetHealth() == 20) player.SetHealth(player.GetHealth() - 20);
				if (player.GetHealth() < 0) player.SetHealth(0);
				updateHealth();
			}
		}
		for (CSprite* shield : shields)
		{
			if (enemyShot->HitTest(shield, 1))
			{
				CSprite* bulletenemyExplosion = new CSprite(enemyShot->GetX(), enemyShot->GetY(), 0, 0, GetTime());
				bulletenemyExplosion->AddImage("testcrash.bmp", "rocketExplode", 3, 1, 0, 0, 2, 0, CColor::Black());
				bulletenemyExplosion->SetAnimation("rocketExplode");
				bulletenemyExplosion->Die(300);
				explosions.push_back(bulletenemyExplosion);

				playerHit.Play("boom2.wav");
				enemyShot->Delete();
			}
		}
	}
	enemyShots.remove_if(deleted);
}

void CMyGame::livesControl()
{
	for (CSprite* life : lives)
	{
		if (player.GetHealth() == 0)
		{
			lifelostsound.Play("life_lose.wav");
			life->Delete();

			// indicate life lost
			CSprite* lifelost = new CSprite(life->GetX(), life->GetY(), 0, 0, GetTime());
			lifelost->AddImage("lifelost2.bmp", "lifelost", 6, 1, 5, 0, 0, 0, CColor::Black());
			lifelost->SetAnimation("lifelost");
			lifelost->Die(600);
			explosions.push_back(lifelost);

			// play explosion at player pos
			CSprite* pExplosion = new CSprite(player.GetX(), player.GetY(), 0, 0, GetTime());
			pExplosion->AddImage("explosion.bmp", "explode", 5, 5, 0, 0, 4, 4, CColor::Black());
			pExplosion->SetAnimation("explode", 36);
			explosionsound.Play("exploPop.wav");
			pExplosion->Die(700);
			explosions.push_back(pExplosion);

			// clear on screen enemies
			CSprite* firewall = new CSprite(-70, 270, "clearscreen.bmp", CColor::Black(), GetTime());
			firewall->SetSize(620, 340);
			firewall->SetRotation(90);
			firewall->SetDirection(90);
			firewall->SetSpeed(400);
			cleansweep.push_back(firewall);

			// respawn player
			respawning = true;
			player.SetPosition(-280, 250);
			player.SetDirection(90);
			player.SetSpeed(100);

			// shield to avoid spawn damage
			CSprite* newShield = new CSprite(player.GetX(), player.GetY(), "shield.png", CColor::Black(), GetTime());
			newShield->SetSize(180, 110);
			newShield->Die(3700);
			shields.push_back(newShield);

			// reset player health
			player.SetHealth(100);
			health.SetSize(285, 45);
			health.SetPosition(650, 585);
		}
	}
	lives.remove_if(deleted);
	explosions.remove_if(deleted);
}

void CMyGame::firewallControl()
{
	for (CSprite* firewall : cleansweep)
	{
		if (firewall->GetX() > 800)
		{
			firewall->Delete();
		}
		// collision detection
		for (CSprite* eShot1 : enemyShots)
		{
			if (firewall->HitTest(eShot1, 1))
			{
				eShot1->Delete();
			}
		}
		for (CSprite* enemy : enemies)
		{
			if (firewall->HitTest(enemy, 1))
			{
				if (enemy->GetStatus() == 1 || enemy->GetStatus() == 2 || enemy->GetStatus() == 4)
				{
					enemy->SetHealth(0);
				}
			}
		}
	}
	cleansweep.remove_if(deleted);
	enemyShots.remove_if(deleted);
	explosions.remove_if(deleted);
}

void CMyGame::itemsControl()
{
	// only spawn during set time
	if (GetTime() < levelTimer)
	{
		if (rand() % 1000 == 0)
		{
			// randomly spawn special ammo item
			CSprite* specialItem = new CSprite(850, rand() % 450 + 32, "specialAmmoItem.bmp", CColor::White(), GetTime());
			specialItem->SetDirection(270);
			specialItem->SetHealth(10);
			specialItem->SetSpeed(140);
			specialItem->SetOmega(-100);
			items.push_back(specialItem);
		}

		if (rand() % 1250 == 0)
		{
			// randomly spawn shield item
			CSprite* shieldItem = new CSprite(850, rand() % 450 + 32, "shielditem.bmp", CColor::White(), GetTime());
			shieldItem->SetDirection(270);
			shieldItem->SetHealth(20);
			shieldItem->SetSpeed(160);
			shieldItem->SetOmega(-100);
			items.push_back(shieldItem);
		}

		if (rand() % 1150 == 0)
		{
			// randomly spawn super ammo item
			CSprite* superItem = new CSprite(850, rand() % 450 + 32, "specialAmmoItem.bmp", CColor::White(), GetTime());
			superItem->AddImage("superAmmoItem.bmp", "supAmmo", 6, 1, 0, 0, 5, 0, CColor::White());
			superItem->SetAnimation("supAmmo");
			superItem->SetDirection(270);
			superItem->SetHealth(30);
			superItem->SetSpeed(200);
			superItem->SetOmega(-100);
			items.push_back(superItem);
		}

		if (rand() % 1100 == 0)
		{
			// randomly spawn health item
			CSprite* healthItem = new CSprite(850, rand() % 450 + 32, "healthItem.bmp", CColor::White(), GetTime());
			healthItem->SetDirection(270);
			healthItem->SetHealth(40);
			healthItem->SetSpeed(140);
			healthItem->SetOmega(-100);
			items.push_back(healthItem);
		}
	}

	for (CSprite* item : items)
	{
		if (player.HitTest(item, 1))
		{
			itemPickupSound.Play("item2.wav");

			//special ammo picked up
			if (item->GetHealth() == 10)
			{
				if (specialAmmo == 0 && superAmmo == 0)
				{
					//set duration initialisation
					transTimer = duration(500);
					//player.SetAnimation("playerTrans");
					player.SetAnimation("playerTrans");
					// has run condition
					runTrans = true;
				}
				specialAmmo += 3;
			}

			//shield item picked up
			if (item->GetHealth() == 20)
			{
				itemPickupSound.Play("item2.wav");
				shieldPickup += 1;
			}

			//super ammo picked up
			if (item->GetHealth() == 30)
			{
				itemPickupSound.Play("item2.wav");
				if (superAmmo == 0 && specialAmmo == 0)
				{
					transTimer = duration(500);
					player.SetAnimation("playerTrans");
					runTrans = true;
				}
				superAmmo += 2;
			}

			// health item picked up
			if (item->GetHealth() == 40)
			{
				itemPickupSound.Play("item2.wav");
				healthPickup += 1;
			}
			item->Delete();
		}
	}
	items.remove_if(deleted);

	// transform spaceship 
	if (runTrans == true)
	{
		if (GetTime() > transTimer)
		{
			if (specialAmmo > 0 || superAmmo > 0)
			{
				runTrans = false;
				player.SetAnimation("playerFull");
			}
			else
			{
				runTrans = false;
				player.SetAnimation("playerStd");
			}
		}
	}
}

long CMyGame::duration(long seconds)
{
	// set a timer
	seconds = GetTime() + seconds;
	return seconds;
}

void CMyGame::updateHealth()
{
	// health bar response
	double healthsize = 285.0 / 100 * player.GetHealth();
	double newpos = ((health.GetWidth() - healthsize) / 2);
	health.SetSize(healthsize, 45);
	health.SetPosition(health.GetX() - newpos, health.GetY());
}

void CMyGame::healthspritecontrol()
{
	for (CSprite* health : healthRegen)
	{
		health->SetPosition(player.GetPosition());
	}
	healthRegen.remove_if(deleted);
}

void CMyGame::shieldControl()
{
	// keep shield on player
	for (CSprite* shield : shields)
	{
		shield->SetPosition(player.GetPosition());
	}
	shields.remove_if(deleted);
}

void CMyGame::OnDraw(CGraphics* g)
{
	// TODO: add drawing code here
	// draw splashscreen
	if (IsMenuMode())
	{
		if (helpscreenactive == true)
		{
			helpscreen.Draw(g);
			backButton.Draw(g);
		}
		else
		{
			levelSelect.Draw(g);
			startScreen.Draw(g);
			nextButton.Draw(g);
			prevButton.Draw(g);
			helpButton.Draw(g);
			startButton.Draw(g);
		}
		return;
	}

	// draw sprites
	background.Draw(g);
	backgroundPlanet.Draw(g);
	for (CSprite* firewall : cleansweep) firewall->Draw(g);
	for (CSprite* eShot1 : enemyShots) eShot1->Draw(g);
	healthback.Draw(g);
	health.Draw(g);
	healthbar.Draw(g);
	for (CSprite* life : lives) life->Draw(g);
	for (CSprite* pSprite : shots) pSprite->Draw(g);
	for (CSprite* item : items) item->Draw(g);
	for (CSprite* enemy : enemies) enemy->Draw(g);
	for (CSprite* bossShield : bossShields) bossShield->Draw(g);
	for (CSprite* regen : healthRegen) regen->Draw(g);
	player.Draw(g);
	for (CSprite* shield : shields) shield->Draw(g);
	for (CSprite* pSprite : explosions) pSprite->Draw(g);

	// print score and set font style
	*g << font("Capture_it.ttf", 12) << color(CColor::White()) << xy(90, 565) << score;

	// special ammo counter
	*g << font(9) << color(CColor::White()) << xy(267, 548) << specialAmmo;
	// special shield counter
	*g << font(9) << color(CColor::White()) << xy(321, 548) << shieldPickup;
	// super ammo counter
	*g << font(9) << color(CColor::White()) << xy(369, 548) << superAmmo;
	// health counter
	*g << font(9) << color(CColor::White()) << xy(417, 548) << healthPickup;

	// on game mode
	if (!IsGameOverMode())
	{
		// display current time
		*g << font(12) << color(CColor::White()) << xy(19, 565) << GetTime() / 1000;

		// display level number
		if (GetTime() < levelTimer - 77000)
		{
			*g << font(60) << color(CColor::DarkYellow(255 - GetTime())) << vcenter << center << "Level: " + std::to_string(level);
		}

		// display asteroids warning
		if (level == 2 && GetTime() > levelTimer - 45000 && GetTime() < levelTimer - 41000)
		{
			*g << font(46) << color(CColor::DarkRed(255 - GetTime())) << vcenter << center << "!-- Incoming asteroid field --!";
		}

		// warning for boss approaching
		if (GetTime() > levelTimer - 3500 && GetTime() < levelTimer - 1000)
		{
			*g << font(46) << color(CColor::DarkRed(255 - GetTime())) << vcenter << center << "!-- Boss Approaching --!";
		}

		// print if game is paused
		if (IsPaused() == true)
		{
			*g << font(46) << color(CColor::Yellow(255)) << vcenter << center << " PAUSED ";
		}
	}

	// on game over mode
	if (IsGameOverMode())
	{
		// display endgame time on hud
		*g << font(12) << color(CColor::White()) << xy(19, 565) << GetTimeGameOver() / 1000;

		// print game over image at the centre of the screen
		gameoverScreen.Draw(g);
		*g << font(14) << color(CColor::White()) << xy(475, 280) << score;
		*g << font(14) << color(CColor::White()) << xy(475, 185) << std::to_string(GetTimeGameOver() / 1000) + "s";
		//level 1 completed?
		if (levelOneCompleted == true)
			*g << font(16) << color(CColor::Green()) << xy(320, 280) << " YES ";
		else
			*g << font(16) << color(CColor::Red()) << xy(325, 280) << " NO ";
		// level 2 completed?
		if (levelTwoCompleted == true)
			*g << font(16) << color(CColor::Green()) << xy(320, 232) << " YES ";
		else
			*g << font(16) << color(CColor::Red()) << xy(325, 232) << " NO ";
		// level 3 completed?
		if (levelThreeCompleted == true)
			*g << font(16) << color(CColor::Green()) << xy(320, 183) << " YES ";
		else
			*g << font(16) << color(CColor::Red()) << xy(325, 183) << " NO ";
		// game won?
		if (gameWon == true)
			*g << font(46) << color(CColor::Green()) << xy(250, 390) << " GAME WON ";
		else
			*g << font(46) << color(CColor::Red()) << xy(250, 390) << " GAME OVER ";

		// return to menu text
		*g << font(25) << color(CColor::Yellow()) << xy(220, 100) << " PRESS F2 TO RETURN TO MENU ";
	}
}

/////////////////////////////////////////////////////
// Game Life Cycle

// one time initialisation
void CMyGame::OnInitialize()
{
	// player ship animations
	player.AddImage("normSpaceship.bmp", "playerStd", 3, 1, 0, 0, 2, 0, CColor::White());
	player.AddImage("fullSpaceship.bmp", "playerFull", 3, 1, 0, 0, 2, 0, CColor::White());
	player.AddImage("transSpaceship.bmp", "playerTrans", 5, 1, 0, 0, 4, 0, CColor::White());
	player.AddImage("transSpaceship.bmp", "playerTransBack", 5, 1, 4, 0, 0, 0, CColor::White());
	player.SetAnimation("playerStd");

	// background planet images
	backgroundPlanet.LoadImage("planetOne.bmp", "planetOne", CColor::Black());
	backgroundPlanet.LoadImage("planetTwo.bmp", "planetTwo", CColor::Black());
	backgroundPlanet.LoadImage("planetThree.bmp", "planetThree", CColor::Black());

	// splash screen images
	levelSelect.LoadImage("level1.bmp", "level1", CColor::Magenta());
	levelSelect.LoadImage("level2.bmp", "level2", CColor::Magenta());
	levelSelect.LoadImage("level3.bmp", "level3", CColor::Magenta());

	nextButton.LoadImage("btnNextUnpressed.bmp", "btnNext", CColor::Magenta());
	nextButton.LoadImage("btnNextPressed.bmp", "btnNextPressed", CColor::Magenta());

	prevButton.LoadImage("btnPrevUnpressed.bmp", "btnPrev", CColor::Magenta());
	prevButton.LoadImage("btnPrevPressed.bmp", "btnPrevPressed", CColor::Magenta());
}

// called when a new game is requested (e.g. when F2 pressed)
// use this function to prepare a menu or a welcome screen
void CMyGame::OnDisplayMenu()
{
	ShowMouse();
	levelSelect.SetImage("level1");
	music.Stop();
	menumusic.Play("music_menu.wav", -1);

	// moving background
	background.SetMotion(-100, 0);
	// set background planet
	backgroundPlanet.SetPosition(1200, 270);
	backgroundPlanet.SetMotion(0, 0);

	// clear sprite lists
	shots.clear();
	items.clear();
	enemies.clear();
	enemyShots.clear();
	healthRegen.clear();
	explosions.clear();
	cleansweep.clear();
	lives.clear();
	shields.clear();
	bossShields.clear();

	// set boolean 
	respawning = true;
	runTrans = false;
	spawnOnce = false;
	startTime = false;
	gameWon = false;
	levelOneCompleted = false;
	levelTwoCompleted = false;
	levelThreeCompleted = false;
	helpscreenactive = false;

	// set inital variables
	score = 0;
	bonusThreshold = 0;
	level = 1;
	specialAmmo = 0;
	superAmmo = 0;
	healthPickup = 0;
	shieldPickup = 0;
	burstshots = 480;
	firerate = 30;

	// set inital player state
	player.SetPosition(-80, 250);
	player.SetDirection(90);
	player.SetSpeed(100);
	health.SetSize(285, 45);
	health.SetPosition(650, 585);
	player.SetHealth(100);
	player.SetAnimation("playerStd");

	// add lives sprites to list
	for (int i = 187; i >= 43; i = i - 72)
	{
		CSprite* newLife = new CSprite(i, 532, "ship_life.bmp", CColor::Black(), GetTime());
		lives.push_back(newLife);
	}
}

// called when a new game is started
// as a second phase after a menu or a welcome screen
void CMyGame::OnStartGame()
{
	HideMouse();
	menumusic.Stop();
	music.Play("music.wav", -1);
	// set inital planet image to use
	if (level == 1) backgroundPlanet.SetImage("planetOne");
	else if (level == 2) backgroundPlanet.SetImage("planetTwo");
	else if (level == 3) backgroundPlanet.SetImage("planetThree");
}

// called when the game is over
void CMyGame::OnGameOver()
{
	ShowMouse();
	music.Stop();
}

// one time termination code
void CMyGame::OnTerminate()
{
}


/////////////////////////////////////////////////////
// Keyboard Event Handlers

void CMyGame::OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode)
{
	// during gameplay
	if (IsGameMode() && IsPaused() == false)
	{
		// health item to restore health
		if (sym == SDLK_e)
		{
			if (player.GetHealth() > 0 && player.GetHealth() < 100 && healthPickup > 0)
			{
				CSprite* heal = new CSprite(player.GetX(), player.GetY(), 0, 0, GetTime());
				heal->AddImage("heal.bmp", "healing", 3, 1, 0, 0, 2, 0, CColor::Black());
				heal->SetAnimation("healing");
				heal->Die(300);
				healthRegen.push_back(heal);

				healthincrease.Play("life_add.wav");

				// amount to heal
				player.SetHealth(player.GetHealth() + 20);

				// if the player is "over-healed" return to 100 hp
				if (player.GetHealth() > 100) player.SetHealth(100);

				healthPickup--;
				updateHealth();
			}
		}
		// use shield, one at a time
		if (sym == SDLK_q && shieldPickup > 0 && shields.size() == 0)
		{
			CSprite* newShield = new CSprite(player.GetX(), player.GetY(), "shield.png", CColor::Black(), GetTime());
			newShield->SetSize(180, 110);
			newShield->Die(3000);
			shields.push_back(newShield);

			shieldPickup--;
		}
	}


	if (sym == SDLK_F4 && (mod & (KMOD_LALT | KMOD_RALT)))
		StopGame();
	if (sym == SDLK_SPACE)
	{
		PauseGame();
		if (IsPaused() == true) music.Pause();
		else music.Resume();
	}
	if (sym == SDLK_F2)
		NewGame();
}

void CMyGame::OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode)
{
}


/////////////////////////////////////////////////////
// Mouse Events Handlers

void CMyGame::OnMouseMove(Uint16 x, Uint16 y, Sint16 relx, Sint16 rely, bool bLeft, bool bRight, bool bMiddle)
{
	if (IsGameMode() && IsPaused() == false && respawning == false)
	{
		player.SetPosition(x, y);
	}
}

void CMyGame::OnLButtonDown(Uint16 x, Uint16 y)
{
	if (IsMenuMode())
	{


		if (helpscreenactive == false)
		{
			// level control for next button
			if (nextButton.HitTest(x, y))
			{
				nextButton.SetImage("btnNextPressed");
				if (level == 1)
				{
					level = 2;
					levelSelect.SetImage("level2");
					buttonPressed.Play("buttonPress.wav");
				}
				else if (level == 2)
				{
					level = 3;
					levelSelect.SetImage("level3");
					buttonPressed.Play("buttonPress.wav");
				}
			}
			// level control for previous button
			if (prevButton.HitTest(x, y))
			{
				prevButton.SetImage("btnPrevPressed");
				if (level == 3)
				{
					level = 2;
					levelSelect.SetImage("level2");
					buttonPressed.Play("buttonPress.wav");
				}
				else if (level == 2)
				{
					level = 1;
					levelSelect.SetImage("level1");
					buttonPressed.Play("buttonPress.wav");
				}
			}
			// help screen image
			if (helpButton.HitTest(x, y))
			{
				buttonPressed.Play("buttonPress.wav");
				helpscreenactive = true;
			}
			// start game button
			if (startButton.HitTest(x, y))
			{
				StartGame();
			}
		}
		else
		{
			// return to menu
			if (backButton.HitTest(x, y))
			{
				buttonPressed.Play("buttonPress.wav");
				helpscreenactive = false;
			}
		}
	}
}

void CMyGame::OnLButtonUp(Uint16 x, Uint16 y)
{
	// reset button images
	if (IsMenuMode())
	{
		nextButton.SetImage("btnNext");
		prevButton.SetImage("btnPrev");
	}

	// reset main player gun
	firerate = 30;
}

void CMyGame::OnRButtonDown(Uint16 x, Uint16 y)
{
	// during gameplay
	if (IsGameMode() && IsPaused() == false)
	{
		// if player has special ammo
		if (specialAmmo > 0)
		{
			CSprite* newShot = new CSprite(player.GetX() + 5, player.GetY() + 26, 0, 0, GetTime());
			newShot->AddImage("topGun.bmp", "specialShot", 4, 1, 0, 0, 3, 0, CColor::Black());
			newShot->SetAnimation("specialShot");
			newShot->SetHealth(100);
			newShot->SetDirection(90);
			newShot->SetSpeed(1000);
			shotsound.Play("shot.wav");
			shots.push_back(newShot);

			//condition for top gun animation 
			if (specialAmmo == 1 && superAmmo == 0)
			{
				transTimer = duration(500);
				player.SetAnimation("playerTransBack");
				runTrans = true;
			}
			specialAmmo--;
		}
		else
		{
			errorsound.Play("empty.wav");
		}
	}
}

void CMyGame::OnRButtonUp(Uint16 x, Uint16 y)
{
}

void CMyGame::OnMButtonDown(Uint16 x, Uint16 y)
{
	// during gameplay
	if (IsGameMode() && IsPaused() == false)
	{
		// if player has super ammo
		if (superAmmo > 0)
		{
			for (int i = -25; i <= 25; i += 50)
			{
				CSprite* newShot = new CSprite(player.GetX() + (i + 30), player.GetY() + -i, 0, 0, GetTime());
				newShot->AddImage("superShot.bmp", "Super", 4, 1, 0, 0, 3, 0, CColor::Black());
				newShot->SetAnimation("Super");
				newShot->SetHealth(200);
				newShot->SetDirection(90);
				newShot->SetSpeed(1000);
				supershotsound.Play("supershot.wav");
				shots.push_back(newShot);
			}

			//condition for top gun animation on last shot fired
			if (superAmmo == 1 && specialAmmo == 0)
			{
				transTimer = duration(500);
				player.SetAnimation("playerTransBack");
				runTrans = true;
			}
			superAmmo--;
		}
		else
		{
			errorsound.Play("empty.wav");
		}
	}
}

void CMyGame::OnMButtonUp(Uint16 x, Uint16 y)
{
}
