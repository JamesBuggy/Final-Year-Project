#include "CollisionManager.h"

int MyContactListener::playersFinished = 0;

/**
* Called when two Box2D fixtures collide
* @param contact the contact information
*/
void MyContactListener::BeginContact(b2Contact* contact) {

	char* userDataA = (char*)contact->GetFixtureA()->GetUserData();
	char* userDataB = (char*)contact->GetFixtureB()->GetUserData();

	if(userDataA == "player" && userDataB == "spikeTrap" || 
		userDataA == "spikeTrap" && userDataB == "player" /*|| 
		userDataA == "player" && userDataB == "bladeTrap" || 
		userDataA == "bladeTrap" && userDataB == "player"*/) {
		
			if(contact->IsTouching()) {

				// Kill player
				if(userDataA == "player") {

					contact->GetFixtureA()->GetBody()->SetLinearVelocity(b2Vec2(0, 0));
					//contact->GetFixtureA()->GetBody()->SetTransform(b2Vec2( playerCheckpoint.x, playerCheckpoint.y), 0.0f);

					Player* p = static_cast<Player*>( contact->GetFixtureA()->GetBody()->GetUserData());
					p->mPos = CIwSVec2(mGame->getCheckPoint().x, mGame->getCheckPoint().y);
					p->loadCheckpoint = true;
					//gamePaused = true;
					//deathScreen->SetVisible(true);
				}
				else {

					contact->GetFixtureB()->GetBody()->SetLinearVelocity(b2Vec2(0, 0));
					//contact->GetFixtureB()->GetBody()->SetTransform(b2Vec2( playerCheckpoint.x, playerCheckpoint.y), 0.0f);

					Player* p = static_cast<Player*>( contact->GetFixtureB()->GetBody()->GetUserData());
					p->mPos = CIwSVec2(mGame->getCheckPoint().x, mGame->getCheckPoint().y);
					p->loadCheckpoint = true;
					//gamePaused = true;
					//deathScreen->SetVisible(true);
				}

				CIwSVec2 screenCentre = CIwSVec2((int16)Iw2DGetSurfaceWidth() >> 1, (int16)Iw2DGetSurfaceHeight() >> 1);

				CIwSVec2 pos = screenCentre + (CIwSVec2(int16(mGame->getCheckPoint().x*8), -int16(mGame->getCheckPoint().y*8)));

				mGame->createEffect(CIwSVec2(pos.x, pos.y), 10, "Images/respawn.png", 1.0f);

				//playerDeaths++;
			}			
	}

	if(userDataA == "player" && userDataB == "platform" || 
		userDataA == "platform" && userDataB == "player") {


			if(userDataA == "player") {

				contact->GetFixtureA()->GetBody()->SetLinearVelocity(b2Vec2(0, 0));
				Player* p = static_cast<Player*>( contact->GetFixtureA()->GetBody()->GetUserData());
				//p->setDir(0);
				//p->goIdle();
				p->onPlatform(true);
			}
			else {
				
				contact->GetFixtureB()->GetBody()->SetLinearVelocity(b2Vec2(0, 0));
				Player* p = static_cast<Player*>( contact->GetFixtureB()->GetBody()->GetUserData());
				//p->setDir(0);
				//p->goIdle();
				p->onPlatform(true);
			}
	}

	if(userDataA == "player" && userDataB == "bladeTrap" || 
		userDataA == "bladeTrap" && userDataB == "player") {
		
			if(contact->IsTouching()) {

				// Kill player
				if(userDataA == "player") {
					
					contact->GetFixtureA()->GetBody()->SetLinearVelocity(b2Vec2(0, 0));
					//contact->GetFixtureA()->GetBody()->SetTransform(b2Vec2( playerCheckpoint.x, playerCheckpoint.y), 0.0f);

					Player* p = static_cast<Player*>( contact->GetFixtureA()->GetBody()->GetUserData());
					p->mPos = CIwSVec2(mGame->getCheckPoint().x, mGame->getCheckPoint().y);
					p->loadCheckpoint = true;
				}
				else {
						
					contact->GetFixtureB()->GetBody()->SetLinearVelocity(b2Vec2(0, 0));
					//contact->GetFixtureB()->GetBody()->SetTransform(b2Vec2( playerCheckpoint.x, playerCheckpoint.y), 0.0f);

					Player* p = static_cast<Player*>( contact->GetFixtureB()->GetBody()->GetUserData());
					p->mPos = CIwSVec2(mGame->getCheckPoint().x, mGame->getCheckPoint().y);
					p->loadCheckpoint = true;
				}

				CIwSVec2 screenCentre = CIwSVec2((int16)Iw2DGetSurfaceWidth() >> 1, (int16)Iw2DGetSurfaceHeight() >> 1);

				CIwSVec2 pos = screenCentre + (CIwSVec2(int16(mGame->getCheckPoint().x*8), -int16(mGame->getCheckPoint().y*8)));

				mGame->createEffect(CIwSVec2(pos.x, pos.y), 10, "Images/respawn.png", 1.0f);
				//playerDeaths++;
			}			
	}

	if(userDataA == "player" && userDataB == "elevator" || 
		userDataA == "elevator" && userDataB == "player") {
		
			if(contact->IsTouching()) {

				if(userDataA == "elevator") {
					Elevator* e = static_cast<Elevator*>( contact->GetFixtureA()->GetBody()->GetUserData());
					e->toggle();
				}
				else {
					
					Elevator* e = static_cast<Elevator*>( contact->GetFixtureB()->GetBody()->GetUserData());
					e->toggle();
				}
			}			
	}

	if(userDataA == "player" && userDataB == "checkpoint" || 
		userDataA == "checkpoint" && userDataB == "player") {
		
			//if(contact->IsTouching()) {

				mGame->setCheckPoint(mGame->getPlayer()->getBody()->GetPosition());
					
				if(!mGame->isMultiplayer()) {
					mGame->saveGame();
				}
			//}			
	}

	if(userDataA == "player" && userDataB == "msgSensor" || 
		userDataA == "msgSensor" && userDataB == "player") {
		
			if(contact->IsTouching()) {

				if(userDataA == "msgSensor") {
					
					MSGSensor* msg = static_cast<MSGSensor*>( contact->GetFixtureA()->GetBody()->GetUserData());
					if(!msg->isActive()) {
						msg->setActive(true);

						//gamePaused = true;
						//mGame->getPlayer()->getBody()->SetLinearVelocity(b2Vec2(0, 0));
						//messageUI->DestroyChildren();
						//messageUI->AddChild(createUIImage(CIwVec2((s3eSurfaceGetInt(S3E_SURFACE_HEIGHT)/2)-250, 50), CIwVec2(500, 300), msg->getID().c_str(), mTexGroup));
						//messageUI->AddChild(createButton(CIwVec2((s3eSurfaceGetInt(S3E_SURFACE_HEIGHT)/2)-150, 250), CIwVec2(300, 66), "closeMessage", "OnClick_CloseMessage", "checkpointBtnUp", "checkpointBtnDown", "none", mTexGroup));
						//messageUI->SetVisible(true);
						//lastMessageID = msg->getID().c_str();
						//lastMessage->SetVisible(true);

						mGame->showMessage(msg->getID().c_str());
					}
				}
				else {
					
					MSGSensor* msg = static_cast<MSGSensor*>( contact->GetFixtureB()->GetBody()->GetUserData());
					if(!msg->isActive()) {
						msg->setActive(true);
								
						//gamePaused = true;
						//mGame->getPlayer()->getBody()->SetLinearVelocity(b2Vec2(0, 0));
						//messageUI->DestroyChildren();
						//messageUI->AddChild(createUIImage(CIwVec2((s3eSurfaceGetInt(S3E_SURFACE_HEIGHT)/2)-250, 50), CIwVec2(500, 300), lastMessageID.c_str(), mTexGroup));
						//messageUI->AddChild(createButton(CIwVec2((s3eSurfaceGetInt(S3E_SURFACE_HEIGHT)/2)-150, 250), CIwVec2(300, 66), "closeMessage", "OnClick_CloseMessage", "checkpointBtnUp", "checkpointBtnDown", "none", mTexGroup));
						//messageUI->SetVisible(true);
						//lastMessageID = msg->getID().c_str();
						//lastMessage->SetVisible(true);

						mGame->showMessage(msg->getID().c_str());
					}
				}
			}
	}

	if(userDataA == "player" && userDataB == "levelEnd" || 
		userDataA == "levelEnd" && userDataB == "player") {
		
			if(contact->IsTouching()) {

				int players = 1;
				if(mGame->isMultiplayer()) {
					players = 2;
				}
				playersFinished++;

				LevelEnd* lvlEnd;
				if(userDataA == "levelEnd") {
					
					lvlEnd = static_cast<LevelEnd*>( contact->GetFixtureA()->GetBody()->GetUserData());
					Player* p = static_cast<Player*>( contact->GetFixtureB()->GetBody()->GetUserData());
					p->finish();
				}
				else {
						
					lvlEnd = static_cast<LevelEnd*>( contact->GetFixtureA()->GetBody()->GetUserData());
					Player* p = static_cast<Player*>( contact->GetFixtureA()->GetBody()->GetUserData());
					p->finish();
				}

				if(playersFinished >= players) {

					if(!lvlEnd->isActive()) {

						lvlEnd->setActive();
						
						//endScreen->SetVisible(true);
						//timeTaken = mGame->getTime();
						//gamePaused = true;

						mGame->endLevel();
					}
				}
			}			
	}

	if(userDataA == "player" && userDataB == "bombEnemy" || 
		userDataA == "bombEnemy" && userDataB == "player") {
		
			if(contact->IsTouching()) {
				b2Vec2 force(0, 0);
				if(userDataA == "bombEnemy") {
					
					BombEnemy* b = static_cast<BombEnemy*>( contact->GetFixtureA()->GetBody()->GetUserData());
					b->arm();
				}
				else {
					
					BombEnemy* b = static_cast<BombEnemy*>( contact->GetFixtureB()->GetBody()->GetUserData());
					b->arm();
				}
			}			
	}
		

	if(userDataA == "bladeTrap" && userDataB == "enemy" || 
		userDataA == "enemy" && userDataB == "bladeTrap" ||
		userDataA == "spikeTrap" && userDataB == "enemy" || 
		userDataA == "enemy" && userDataB == "spikeTrap") {
		
			if(contact->IsTouching()) {

				if(userDataA == "enemy") {
					
					Enemy* e = static_cast<BombEnemy*>( contact->GetFixtureA()->GetBody()->GetUserData());
					e->kill();

					CIwSVec2 screenCentre = CIwSVec2((int16)Iw2DGetSurfaceWidth() >> 1, (int16)Iw2DGetSurfaceHeight() >> 1);
					b2Transform t = e->getBody()->GetTransform();
					CIwSVec2 pos = screenCentre + (CIwSVec2(int16(t.p.x*8), -int16(t.p.y*8)));

					mGame->createEffect(pos, 8 , "Images/blood.png", 1.5f);
				}
				else {
					
					Enemy* e = static_cast<BombEnemy*>( contact->GetFixtureB()->GetBody()->GetUserData());
					e->kill();

					CIwSVec2 screenCentre = CIwSVec2((int16)Iw2DGetSurfaceWidth() >> 1, (int16)Iw2DGetSurfaceHeight() >> 1);
					b2Transform t = e->getBody()->GetTransform();
					CIwSVec2 pos = screenCentre + (CIwSVec2(int16(t.p.x*8), -int16(t.p.y*8)));

					mGame->createEffect(pos, 8 , "Images/blood.png", 1.5f);
				}
			}			
	}

	if(userDataA == "player" && userDataB == "meleeEnemy" || 
		userDataA == "meleeEnemy" && userDataB == "player") {
		
			if(contact->IsTouching()) {
				b2Vec2 force(0, 0);
				if(userDataA == "meleeEnemy") {
					
					MeleeEnemy* m = static_cast<MeleeEnemy*>( contact->GetFixtureA()->GetBody()->GetUserData());
					//m->setAttack();
				}
				else {
					
					MeleeEnemy* m = static_cast<MeleeEnemy*>( contact->GetFixtureB()->GetBody()->GetUserData());
					//m->setAttack();
				}
			}			
	}
	
	// Check collision with explosions
	if(userDataA == "player" && userDataB == "explosion" || 
		userDataA == "explosion" && userDataB == "player") {
		
			if(contact->IsTouching()) {
					
				b2Vec2 force(0, 0);

				// Hurt player
				if(userDataA == "player") {
					
					Player* p = static_cast<Player*>( contact->GetFixtureA()->GetBody()->GetUserData());
					p->setDir(0);
					p->setCanRun(false);
					p->getBody()->SetLinearVelocity(b2Vec2(0, 0));

					b2Vec2 dir = contact->GetFixtureA()->GetBody()->GetPosition() - contact->GetFixtureB()->GetBody()->GetPosition();

					dir.Normalize();
					force = b2Vec2(dir.x*80000, dir.y*100000);

					contact->GetFixtureA()->GetBody()->ApplyForce(force, contact->GetFixtureA()->GetBody()->GetWorldCenter());	
				}
				else {

					Player* p = static_cast<Player*>( contact->GetFixtureB()->GetBody()->GetUserData());
					p->setDir(0);
					p->setCanRun(false);
					p->getBody()->SetLinearVelocity(b2Vec2(0, 0));

					b2Vec2 dir = contact->GetFixtureB()->GetBody()->GetPosition() - contact->GetFixtureA()->GetBody()->GetPosition();

					dir.Normalize();
					force = b2Vec2(dir.x*80000, dir.y*100000);
					
					contact->GetFixtureB()->GetBody()->ApplyForce(force, contact->GetFixtureB()->GetBody()->GetWorldCenter());	
				}
			}			
	}

	if(userDataA == "player" && userDataB == "healthPickup" || 
		userDataA == "healthPickup" && userDataB == "player") {
		
			if(contact->IsTouching()) {
					
				int id = 0;

				if(userDataA == "healthPickup") {
					
					HealthPickup* h = static_cast<HealthPickup*>( contact->GetFixtureA()->GetBody()->GetUserData());
					Player* p = static_cast<Player*>( contact->GetFixtureB()->GetBody()->GetUserData());
					h->pickUp();
					p->heal(h->getStrength());

					id = h->getID();
				}
				else {
					
					HealthPickup* h = static_cast<HealthPickup*>( contact->GetFixtureB()->GetBody()->GetUserData());
					Player* p = static_cast<Player*>( contact->GetFixtureA()->GetBody()->GetUserData());
					h->pickUp();
					p->heal(h->getStrength());

					id = h->getID();
				}

				PacketStream s;
				s.writeInt(Constants::DESTROYPICKUP);
				s.writeInt(id);
				s.writeChar('\0');

				char message[10];
				s.toCharArray(message);
				message[strlen(message)-1] = '\0';

				//s3eSocketSendTo(Game::mSock, message, strlen(message), 0, &mDest_addr);
			}			
	}

	if(userDataA == "player" && userDataB == "speedPickup" || 
		userDataA == "speedPickup" && userDataB == "player") {
		
			if(contact->IsTouching()) {
					
				int id = 0;

				if(userDataA == "speedPickup") {
					
					SpeedPickup* s = static_cast<SpeedPickup*>( contact->GetFixtureA()->GetBody()->GetUserData());
					Player* p = static_cast<Player*>( contact->GetFixtureB()->GetBody()->GetUserData());
					s->pickUp();
					p->speedBoost(s->getStrength());

					id = s->getID();
				}
				else {
					
					SpeedPickup* s = static_cast<SpeedPickup*>( contact->GetFixtureB()->GetBody()->GetUserData());
					Player* p = static_cast<Player*>( contact->GetFixtureA()->GetBody()->GetUserData());
					s->pickUp();
					p->speedBoost(s->getStrength());

					id = s->getID();
				}

				PacketStream s;
				s.writeInt(Constants::DESTROYPICKUP);
				s.writeInt(id);
				s.writeChar('\0');

				char message[10];
				s.toCharArray(message);
				message[strlen(message)-1] = '\0';

				//s3eSocketSendTo(Game::mSock, message, strlen(message), 0, &mDest_addr);
			}			
	}

	if(userDataA == "player" && userDataB == "damagePickup" || 
		userDataA == "damagePickup" && userDataB == "player") {
		
			if(contact->IsTouching()) {
					
				int id = 0;

				if(userDataA == "damagePickup") {
					
					DamagePickup* d = static_cast<DamagePickup*>( contact->GetFixtureA()->GetBody()->GetUserData());
					Player* p = static_cast<Player*>( contact->GetFixtureB()->GetBody()->GetUserData());
					d->pickUp();
					p->damageBoost(d->getStrength());

					id = d->getID();
				}
				else {
					
					DamagePickup* d = static_cast<DamagePickup*>( contact->GetFixtureB()->GetBody()->GetUserData());
					Player* p = static_cast<Player*>( contact->GetFixtureA()->GetBody()->GetUserData());
					d->pickUp();
					p->damageBoost(d->getStrength());

					id = d->getID();
				}

				PacketStream s;
				s.writeInt(Constants::DESTROYPICKUP);
				s.writeInt(id);
				s.writeChar('\0');

				char message[10];
				s.toCharArray(message);
				message[strlen(message)-1] = '\0';

				//s3eSocketSendTo(Game::mSock, message, strlen(message), 0, &mDest_addr);
			}			
	}

	if(userDataA == "spikeTrap" && userDataB == "woodenCrate" || 
		userDataA == "woodenCrate" && userDataB == "spikeTrap") {
		
			//if(contact->IsTouching()) {
				b2Vec2 pos;
				if(userDataA == "woodenCrate") {
					
					Crate* c = static_cast<Crate*>( contact->GetFixtureA()->GetBody()->GetUserData());
					pos = c->getBody()->GetPosition();
					c->destroy();
				}
				else {
					
					Crate* c = static_cast<Crate*>( contact->GetFixtureB()->GetBody()->GetUserData());
					pos = c->getBody()->GetPosition();
					c->destroy();
				}

				mGame->crateSpawned = false;
				mGame->create(Constants::CREATEPICKUP, 1, pos.x, pos.y);

				//if(host) {
				//mGame->spawnPickup(true);
					//pickupSpawnPoint = pos;
				//}

				/*PacketStream s;
				s.writeInt(Constants::CREATEPICKUP);
				s.writeInt(mGame->getLevel()->getPickups().back()->getID());
				s.writeInt(1);
				s.writeFloat(pos.x);
				s.writeFloat(pos.y);
				s.writeChar('\0');

				char message[10];
				s.toCharArray(message);
				message[strlen(message)-1] = '\0';*/

				//s3eSocketSendTo(Game::mSock, message, strlen(message), 0, &mDest_addr);
			//}			
	}

	if(userDataA == "killBox" && userDataB == "speedPickup" || 
		userDataA == "speedPickup" && userDataB == "killBox") {
		
			if(contact->IsTouching()) {
				if(userDataA == "speedPickup") {
					
					SpeedPickup* s = static_cast<SpeedPickup*>( contact->GetFixtureA()->GetBody()->GetUserData());
					s->pickUp();
				}
				else {
					
					SpeedPickup* s = static_cast<SpeedPickup*>( contact->GetFixtureB()->GetBody()->GetUserData());
					s->pickUp();
				}
			}
		
	}

	if(userDataA == "player" && userDataB == "platform" || 
		userDataA == "platform" && userDataB == "player") {
		
			if(contact->IsTouching()) {
				if(userDataA == "player") {
					
					Player* p = static_cast<Player*>( contact->GetFixtureA()->GetBody()->GetUserData());
					p->idleLegs();
					p->jumped = 0;
					p->setDir(0);
				}
				else {
					
					Player* p = static_cast<Player*>( contact->GetFixtureB()->GetBody()->GetUserData());
					p->idleLegs();
					p->jumped = 0;
					p->setDir(0);
				}
			}	
	}

	if(userDataA == "player" && userDataB == "woodenCrate" || 
		userDataA == "woodenCrate" && userDataB == "player" || 
		userDataA == "player" && userDataB == "metalCrate" || 
		userDataA == "metalCrate" && userDataB == "player") {
		
			if(contact->IsTouching()) {
				if(userDataA == "player") {
					
					Player* p = static_cast<Player*>( contact->GetFixtureA()->GetBody()->GetUserData());
					p->idleLegs();
					p->jumped = 0;
					p->setDir(0);
				}
				else {
					
					Player* p = static_cast<Player*>( contact->GetFixtureB()->GetBody()->GetUserData());
					p->idleLegs();
					p->jumped = 0;
					p->setDir(0);
				}
			}
	}
}

/**
* Called when two Box2D fixtures stop colliding
* @param contact the contact information
*/
void MyContactListener::EndContact(b2Contact* contact) {
	
	char* userDataA = (char*)contact->GetFixtureA()->GetUserData();
	char* userDataB = (char*)contact->GetFixtureB()->GetUserData();

	if(userDataA == "player" && userDataB == "platform" || 
		userDataA == "platform" && userDataB == "player") {


			if(userDataA == "player") {

				Player* p = static_cast<Player*>( contact->GetFixtureA()->GetBody()->GetUserData());
				p->onPlatform(false);
			}
			else {
				
				Player* p = static_cast<Player*>( contact->GetFixtureB()->GetBody()->GetUserData());
				p->onPlatform(false);
			}
	}
}

/**
* Sets the game which the listener is active for
* @param pGame the game
*/
void MyContactListener::setGame(Game* pGame) {

	mGame = pGame;
}