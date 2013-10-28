#include "Enemy.h"

/**
* Constructor
* @param pImgName the image to use
* @param pPos the position of the enemy
* @param pID the id number of the enemy
* @param pDiff the difficulty setting of the game
* @param pPatrolA first point which the enemy patrols between
* @param pPatrolB second point the enemy patrols between
*/
Enemy::Enemy(const char * pImgName, CIwSVec2 pPos, int pID, int pDiff, int pPatrolA, int pPatrolB):
	mAlive(true),
	mHealth(100),
	mState(IDLE),
	mTarget(b2Vec2(0, 0)),
	mDirection(1),
	mDrawDirection(1),
	mID(pID),
	mCanMove(false),
	mStunTimer(0.0f) {

		mPatrolPointA = -(768/16) + (0.5*4) + (pPatrolA*4) + 0.5;
		mPatrolPointB = -(768/16) + (0.5*4) + (pPatrolB*4) + 0.5;

		mImg = Iw2DCreateImage(pImgName);
		mPos = CIwSVec2(pPos.x*32, (pPos.y*32)+4);
		mHealth = 100.0f*pDiff;
		mMaxHealth = mHealth;
		mPatSave1 = pPatrolA;
		mPatSave2 = pPatrolB;

		mDeathSoundSpec = (CIwSoundSpec*)IwGetResManager()->GetGroupNamed("Sounds")->GetResNamed("enemyDeath", IW_SOUND_RESTYPE_SPEC);
}

/**
* Enemy update function
*/
void Enemy::update(bool pHost) {
	
}

/**
* Stuns the enemy. Stunned enemies can't move or attack
*/
void Enemy::stun() {

	mCanMove = false;
	mStunTimer = 5;
}

/**
* Kills the enemy
*/
void Enemy::kill() {
	
	mAlive = false;
	mDeathSoundSpec->Play();
}

/**
* Sete the target of this enemy (the player)
*/
void Enemy::setTarget(b2Vec2 pTarget) {

	mTarget = pTarget;
}

/**
* Sets the player this enemy will attack
*/
void Enemy::setPlayer(Player* pPlayer) {

	mPlayer = pPlayer;
}

/**
* Checks if the enemy is currently attacking
* @return true if the enemy is attacking, else false
*/
bool Enemy::isAttacking() {

	if(mState == ATTACK) {
		return true;
	}

	return false;
}

/**
* Checks if the enemy is alive
* @param the enemies alive state
*/
bool Enemy::isAlive() {
	
	return mAlive;
}

/**
* getts the enemies physics body
* @return the enemies physics body
*/
b2Body* Enemy::getBody() {

	return mBody;
}

/**
* Gets the enemies id number
* @return the enemies id number
*/
int Enemy::getID() {

	return mID;
}

/**
* Applies damage to the enemy, reducing its health
* @param the amount of damage to apply
*/
void Enemy::ApplyDamage(int pDamage) {

	mHealth -= pDamage;
}

/**
* Sets the enemy state to attack
*/
void Enemy::attack() {
	mState = ATTACK;
}

/**
* Sets the enemy state to idle
*/
void Enemy::idle() {
	mState = IDLE;
}

/**
* Serializes the enemy info to be sent across a network during multiplayer
* @param pStream The packet stream to write the info to
*/
void Enemy::serialize(PacketStream* pStream) {

	pStream->writeFloat(mBody->GetLinearVelocity().x);
	pStream->writeFloat(mBody->GetLinearVelocity().y);
	pStream->writeFloat(mBody->GetPosition().x);
	pStream->writeFloat(mBody->GetPosition().y);
	pStream->writeFloat(mDirection);
}

/**
* Deserializes enemy info from a packet stream during multiplayer
* @param pStream the packet stream to read from
*/
void Enemy::deserialize(PacketStream* pStream) {

	float xVel, yVel, xPos, yPos, dir;
	pStream->readFloat(xVel);
	pStream->readFloat(yVel);
	pStream->readFloat(xPos);
	pStream->readFloat(yPos);
	pStream->readFloat(dir);

	mBody->SetLinearVelocity(b2Vec2(xVel,yVel));
	mBody->SetTransform(b2Vec2(xPos, yPos), 0.0f);
	mDirection = dir;
}

/**
* Saves the enemy to an XML file
* @param pDoc the file to save to
*/
void Enemy::save(TiXmlDocument& pDoc) {

	TiXmlElement * enemy = new TiXmlElement( "enemy" );  
	pDoc.RootElement()->LinkEndChild( enemy );

	TiXmlElement* type = new TiXmlElement( "type" ); 
	if(mSensorData == "meleeEnemy") {
		type->LinkEndChild( new TiXmlText( "2" ));
	}
	else if(mSensorData == "bombEnemy") {
		type->LinkEndChild( new TiXmlText( "1" ));
	}
	enemy->LinkEndChild( type );

	string x = Constants::convertInt(mBody->GetPosition().x);

	TiXmlElement* xPos = new TiXmlElement( "xPos" );  
	xPos->LinkEndChild( new TiXmlText( x.c_str() ));
	enemy->LinkEndChild( xPos );

	string y = Constants::convertInt(mBody->GetPosition().y);

	TiXmlElement* yPos = new TiXmlElement( "yPos" );  
	yPos->LinkEndChild( new TiXmlText( y.c_str() ));
	enemy->LinkEndChild( yPos );

	TiXmlElement* pat1 = new TiXmlElement( "patrolA" );  
	pat1->LinkEndChild( new TiXmlText( Constants::convertInt(mPatSave1).c_str() ));
	enemy->LinkEndChild( pat1 );

	TiXmlElement* pat2 = new TiXmlElement( "patrolB" );  
	pat2->LinkEndChild( new TiXmlText( Constants::convertInt(mPatSave2).c_str() ));
	enemy->LinkEndChild( pat2 );
}

/**
* Destructor
*/
Enemy::~Enemy() {

}