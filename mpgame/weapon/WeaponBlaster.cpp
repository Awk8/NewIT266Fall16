#include "../../idlib/precompiled.h"
#pragma hdrstop

#include "../Game_local.h"
#include "../Weapon.h"

#define BLASTER_SPARM_CHARGEGLOW		6

class rvWeaponPussBox : public rvWeapon {
public:

	CLASS_PROTOTYPE( rvWeaponPussBox );

	rvWeaponPussBox ( void );

	virtual void		Spawn				( void );
	void				Save				( idSaveGame *savefile ) const;
	void				Restore				( idRestoreGame *savefile );
	void				PreSave		( void );
	void				PostSave	( void );

protected:

	bool				UpdateAttack		( void );
	bool				UpdateFlashlight	( void );
	void				Flashlight			( bool on );

private:

	int					chargeTime;
	int					chargeDelay;
	idVec2				chargeGlow;
	bool				fireForced;
	int					fireHeldTime;

	stateResult_t		State_Raise				( const stateParms_t& parms );
	stateResult_t		State_Lower				( const stateParms_t& parms );
	stateResult_t		State_Idle				( const stateParms_t& parms );
	stateResult_t		State_Charge			( const stateParms_t& parms );
	stateResult_t		State_Charged			( const stateParms_t& parms );
	stateResult_t		State_Fire				( const stateParms_t& parms );
	stateResult_t		State_Flashlight		( const stateParms_t& parms );
	
	CLASS_STATES_PROTOTYPE ( rvWeaponPussBox );
};

CLASS_DECLARATION( rvWeapon, rvWeaponPussBox )
END_CLASS

/*
================
rvWeaponPussBox::rvWeaponPussBox
================
*/
rvWeaponPussBox::rvWeaponPussBox ( void ) {
}

/*
================
rvWeaponPussBox::UpdateFlashlight
================
*/
bool rvWeaponPussBox::UpdateFlashlight ( void ) {
	if ( !wsfl.flashlight ) {
		return false;
	}
	
	SetState ( "Flashlight", 0 );
	return true;		
}

/*
================
rvWeaponPussBox::Flashlight
================
*/
void rvWeaponPussBox::Flashlight ( bool on ) {
	owner->Flashlight ( on );
	
	if ( on ) {
		worldModel->ShowSurface ( "models/weapons/blaster/flare" );
		viewModel->ShowSurface ( "models/weapons/blaster/flare" );
	} else {
		worldModel->HideSurface ( "models/weapons/blaster/flare" );
		viewModel->HideSurface ( "models/weapons/blaster/flare" );
	}
}

/*
================
rvWeaponPussBox::UpdateAttack
================
*/
bool rvWeaponPussBox::UpdateAttack ( void ) {
	// Clear fire forced
	if ( fireForced ) {
		if ( !wsfl.attack ) {
			fireForced = false;
		} else {
			return false;
		}
	}

	// If the player is pressing the fire button and they have enough ammo for a shot
	// then start the shooting process.
	if (manaAvailable())
	//if (AmmoAvailable())
	{
		if ( wsfl.attack && gameLocal.time >= nextAttackTime ) {
			// Save the time which the fire button was pressed
			if ( fireHeldTime == 0 ) {		
				nextAttackTime = gameLocal.time + (fireRate * owner->PowerUpModifier ( PMOD_FIRERATE ));
				fireHeldTime   = gameLocal.time;
				viewModel->SetShaderParm ( BLASTER_SPARM_CHARGEGLOW, chargeGlow[0] );
			}
		}		
	}

	// If they have the charge mod and they have overcome the initial charge 
	// delay then transition to the charge state.
	if ( fireHeldTime != 0 ) {
		if ( gameLocal.time - fireHeldTime > chargeDelay ) {
			SetState ( "Charge", 4 );
			return true;
		}

		// If the fire button was let go but was pressed at one point then 
		// release the shot.
		if ( !wsfl.attack ) {
			idPlayer * player = gameLocal.GetLocalPlayer();
			if( player )	{
			
				if( player->GuiActive())	{
					//make sure the player isn't looking at a gui first
					SetState ( "Lower", 0 );
				} else {
					SetState ( "Fire", 0 );
				}
			}
			return true;
		}
	}
	
	return false;
}

/*
================
rvWeaponPussBox::Spawn
================
*/
void rvWeaponPussBox::Spawn ( void ) {
	viewModel->SetShaderParm ( BLASTER_SPARM_CHARGEGLOW, 0 );
	SetState ( "Raise", 0 );
	
	chargeGlow   = spawnArgs.GetVec2 ( "chargeGlow" );
	chargeTime   = SEC2MS ( spawnArgs.GetFloat ( "chargeTime" ) );
	chargeDelay  = SEC2MS ( spawnArgs.GetFloat ( "chargeDelay" ) );

	fireHeldTime		= 0;
	fireForced			= false;
			
	Flashlight ( owner->IsFlashlightOn() );
}

/*
================
rvWeaponPussBox::Save
================
*/
void rvWeaponPussBox::Save ( idSaveGame *savefile ) const {
	savefile->WriteInt ( chargeTime );
	savefile->WriteInt ( chargeDelay );
	savefile->WriteVec2 ( chargeGlow );
	savefile->WriteBool ( fireForced );
	savefile->WriteInt ( fireHeldTime );
}

/*
================
rvWeaponPussBox::Restore
================
*/
void rvWeaponPussBox::Restore ( idRestoreGame *savefile ) {
	savefile->ReadInt ( chargeTime );
	savefile->ReadInt ( chargeDelay );
	savefile->ReadVec2 ( chargeGlow );
	savefile->ReadBool ( fireForced );
	savefile->ReadInt ( fireHeldTime );
}

/*
================
rvWeaponPussBox::PreSave
================
*/
void rvWeaponPussBox::PreSave ( void ) {

	SetState ( "Idle", 4 );

	StopSound( SND_CHANNEL_WEAPON, 0);
	StopSound( SND_CHANNEL_BODY, 0);
	StopSound( SND_CHANNEL_ITEM, 0);
	StopSound( SND_CHANNEL_ANY, false );
	
}

/*
================
rvWeaponPussBox::PostSave
================
*/
void rvWeaponPussBox::PostSave ( void ) {
}

/*
===============================================================================

	States 

===============================================================================
*/

CLASS_STATES_DECLARATION ( rvWeaponPussBox )
	STATE ( "Raise",						rvWeaponPussBox::State_Raise )
	STATE ( "Lower",						rvWeaponPussBox::State_Lower )
	STATE ( "Idle",							rvWeaponPussBox::State_Idle)
	STATE ( "Charge",						rvWeaponPussBox::State_Charge )
	STATE ( "Charged",						rvWeaponPussBox::State_Charged )
	STATE ( "Fire",							rvWeaponPussBox::State_Fire )
	STATE ( "Flashlight",					rvWeaponPussBox::State_Flashlight )
END_CLASS_STATES

/*
================
rvWeaponPussBox::State_Raise
================
*/
stateResult_t rvWeaponPussBox::State_Raise( const stateParms_t& parms ) {
	enum {
		RAISE_INIT,
		RAISE_WAIT,
	};	
	switch ( parms.stage ) {
		case RAISE_INIT:			
			SetStatus ( WP_RISING );
			PlayAnim( ANIMCHANNEL_ALL, "raise", parms.blendFrames );
			return SRESULT_STAGE(RAISE_WAIT);
			
		case RAISE_WAIT:
			if ( AnimDone ( ANIMCHANNEL_ALL, 4 ) ) {
				SetState ( "Idle", 4 );
				return SRESULT_DONE;
			}
			if ( wsfl.lowerWeapon ) {
				SetState ( "Lower", 4 );
				return SRESULT_DONE;
			}
			return SRESULT_WAIT;
	}
	return SRESULT_ERROR;	
}

/*
================
rvWeaponPussBox::State_Lower
================
*/
stateResult_t rvWeaponPussBox::State_Lower ( const stateParms_t& parms ) {
	enum {
		LOWER_INIT,
		LOWER_WAIT,
		LOWER_WAITRAISE
	};	
	switch ( parms.stage ) {
		case LOWER_INIT:
			SetStatus ( WP_LOWERING );
			PlayAnim( ANIMCHANNEL_ALL, "putaway", parms.blendFrames );
			return SRESULT_STAGE(LOWER_WAIT);
			
		case LOWER_WAIT:
			if ( AnimDone ( ANIMCHANNEL_ALL, 0 ) ) {
				SetStatus ( WP_HOLSTERED );
				return SRESULT_STAGE(LOWER_WAITRAISE);
			}
			return SRESULT_WAIT;
	
		case LOWER_WAITRAISE:
			if ( wsfl.raiseWeapon ) {
				SetState ( "Raise", 0 );
				return SRESULT_DONE;
			}
			return SRESULT_WAIT;
	}
	return SRESULT_ERROR;
}

/*
================
rvWeaponPussBox::State_Idle
================
*/
stateResult_t rvWeaponPussBox::State_Idle ( const stateParms_t& parms ) {	
	enum {
		IDLE_INIT,
		IDLE_WAIT,
	};	
	switch ( parms.stage ) {
		case IDLE_INIT:			
			SetStatus ( WP_READY );
			PlayCycle( ANIMCHANNEL_ALL, "idle", parms.blendFrames );
			return SRESULT_STAGE ( IDLE_WAIT );
			
		case IDLE_WAIT:
			if ( wsfl.lowerWeapon ) {
				SetState ( "Lower", 4 );
				return SRESULT_DONE;
			}
			
			if ( UpdateFlashlight ( ) ) { 
				return SRESULT_DONE;
			}
			if ( UpdateAttack ( ) ) {
				return SRESULT_DONE;
			}
			return SRESULT_WAIT;
	}
	return SRESULT_ERROR;
}

/*
================
rvWeaponPussBox::State_Charge
================
*/
stateResult_t rvWeaponPussBox::State_Charge ( const stateParms_t& parms ) {
	enum {
		CHARGE_INIT,
		CHARGE_WAIT,
	};	
	switch ( parms.stage ) {
		case CHARGE_INIT:
			viewModel->SetShaderParm ( BLASTER_SPARM_CHARGEGLOW, chargeGlow[0] );
			StartSound ( "snd_charge", SND_CHANNEL_ITEM, 0, false, NULL );
			PlayCycle( ANIMCHANNEL_ALL, "charging", parms.blendFrames );
			return SRESULT_STAGE ( CHARGE_WAIT );
			
		case CHARGE_WAIT:	
			if ( gameLocal.time - fireHeldTime < chargeTime ) {
				float f;
				f = (float)(gameLocal.time - fireHeldTime) / (float)chargeTime;
				f = chargeGlow[0] + f * (chargeGlow[1] - chargeGlow[0]);
				f = idMath::ClampFloat ( chargeGlow[0], chargeGlow[1], f );
				viewModel->SetShaderParm ( BLASTER_SPARM_CHARGEGLOW, f );
				
				if ( !wsfl.attack ) {
					SetState ( "Fire", 0 );
					return SRESULT_DONE;
				}
				
				return SRESULT_WAIT;
			} 
			SetState ( "Charged", 4 );
			return SRESULT_DONE;
	}
	return SRESULT_ERROR;	
}

/*
================
rvWeaponPussBox::State_Charged
================
*/
stateResult_t rvWeaponPussBox::State_Charged ( const stateParms_t& parms ) {
	enum {
		CHARGED_INIT,
		CHARGED_WAIT,
	};	
	switch ( parms.stage ) {
		case CHARGED_INIT:		
			viewModel->SetShaderParm ( BLASTER_SPARM_CHARGEGLOW, 1.0f  );

			StopSound ( SND_CHANNEL_ITEM, false );
			StartSound ( "snd_charge_loop", SND_CHANNEL_ITEM, 0, false, NULL );
			StartSound ( "snd_charge_click", SND_CHANNEL_BODY, 0, false, NULL );
			return SRESULT_STAGE(CHARGED_WAIT);
			
		case CHARGED_WAIT:
			if ( !wsfl.attack ) {
				fireForced = true;
				SetState ( "Fire", 0 );
				return SRESULT_DONE;
			}
			return SRESULT_WAIT;
	}
	return SRESULT_ERROR;
}

/*
================
rvWeaponPussBox::State_Fire
================
*/
stateResult_t rvWeaponPussBox::State_Fire ( const stateParms_t& parms ) {
	enum {
		FIRE_INIT,
		FIRE_WAIT,
	};	
	switch ( parms.stage ) {
		case FIRE_INIT:	

			StopSound ( SND_CHANNEL_ITEM, false );
			viewModel->SetShaderParm ( BLASTER_SPARM_CHARGEGLOW, 0 );
			//don't fire if we're targeting a gui.
			idPlayer* player;
			player = gameLocal.GetLocalPlayer();

			//make sure the player isn't looking at a gui first
			if( player && player->GuiActive() )	{
				fireHeldTime = 0;
				SetState ( "Lower", 0 );
				return SRESULT_DONE;
			}

			if( player && !player->CanFire() )	{
				fireHeldTime = 0;
				SetState ( "Idle", 4 );
				return SRESULT_DONE;
			}


	
			if ( gameLocal.time - fireHeldTime > chargeTime ) {	
				Attack ( true, 1, spread, 0, 1.0f );
				PlayEffect ( "fx_chargedflash", barrelJointView, false );
				PlayAnim( ANIMCHANNEL_ALL, "chargedfire", parms.blendFrames );
			} else {
				Attack ( true, 1, spread, 0, 1.0f );
				PlayEffect ( "fx_normalflash", barrelJointView, false );
				PlayAnim( ANIMCHANNEL_ALL, "fire", parms.blendFrames );
			}
			fireHeldTime = 0;
			
			return SRESULT_STAGE(FIRE_WAIT);
		
		case FIRE_WAIT:
			if ( AnimDone ( ANIMCHANNEL_ALL, 4 ) ) {
				SetState ( "Idle", 4 );
				return SRESULT_DONE;
			}
			if ( UpdateFlashlight ( ) || UpdateAttack ( ) ) {
				return SRESULT_DONE;
			}
			return SRESULT_WAIT;
	}			
	return SRESULT_ERROR;
}

/*
================
rvWeaponPussBox::State_Flashlight
================
*/
stateResult_t rvWeaponPussBox::State_Flashlight ( const stateParms_t& parms ) {
	enum {
		FLASHLIGHT_INIT,
		FLASHLIGHT_WAIT,
	};	
	switch ( parms.stage ) {
		case FLASHLIGHT_INIT:			
			SetStatus ( WP_FLASHLIGHT );
			// Wait for the flashlight anim to play		
			PlayAnim( ANIMCHANNEL_ALL, "flashlight", 0 );
			return SRESULT_STAGE ( FLASHLIGHT_WAIT );
			
		case FLASHLIGHT_WAIT:
			if ( !AnimDone ( ANIMCHANNEL_ALL, 4 ) ) {
				return SRESULT_WAIT;
			}
			
			if ( owner->IsFlashlightOn() ) {
				Flashlight ( false );
			} else {
				Flashlight ( true );
			}
			
			SetState ( "Idle", 4 );
			return SRESULT_DONE;
	}
	return SRESULT_ERROR;
}
