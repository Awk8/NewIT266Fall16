//----------------------------------------------------------------
// Buying.cpp
//
// Copyright 2005 Ritual Entertainment
//
// This file essentially serves as an extension to the Game DLL
// source files Multiplayer.cpp and Player.cpp, in an attempt
// to isolate, as much as possible, these changes from the main
// body of code (for merge simplification, etc).
//----------------------------------------------------------------

#include "../../idlib/precompiled.h"
#pragma hdrstop

#include "Buying.h"


riBuyingManager::riBuyingManager() :
	_buyingGameBalanceConstants( NULL ),
	opponentKillCashAward( 0 ),
	opponentKillXPAward( 0 ),
	opponentKillFragCount( -1 ) { }

riBuyingManager::~riBuyingManager() { }

int riBuyingManager::GetIntValueForKey( const char* keyName, int defaultValue ) {
	if( !keyName )
	{
		return defaultValue;
	}

	if( !_buyingGameBalanceConstants )
	{
		_buyingGameBalanceConstants = static_cast<const idDeclEntityDef*>( declManager->FindType( DECL_ENTITYDEF, "BuyingGameBalanceConstants", false ) );

		if( !_buyingGameBalanceConstants )
		{
			return defaultValue;
		}
	}

	for( int i = 0; i < _buyingGameBalanceConstants->dict.GetNumKeyVals(); i++ )
	{
		const idKeyValue* keyValuePair = _buyingGameBalanceConstants->dict.GetKeyVal( i );
		if( !keyValuePair->GetKey().Icmp( keyName ) )
		{
			return atoi( keyValuePair->GetValue() );
		}
	}

	return defaultValue;
}

int riBuyingManager::GetOpponentKillCashAward( int playerLevel ) {
	switch ( playerLevel )
	{
		case 1:
			opponentKillCashAward = 500;
			break;
		case 2:
			opponentKillCashAward = 550;
			break;
		case 3:
			opponentKillCashAward = 600;
			break;
		case 4:
			opponentKillCashAward = 650;
			break;
		case 5:
			opponentKillCashAward = 700;
			break;
		case 6:
			opponentKillCashAward = 700;
			break;
		case 7:
			opponentKillCashAward = 800;
			break;
		case 8:
			opponentKillCashAward = 900;
			break;
		case 9:
			opponentKillCashAward = 1000;
			break;
		case 10:
			opponentKillCashAward = 1200;
			break;
		default:
			opponentKillCashAward = 0;
			break;
	}
	return opponentKillCashAward;
}

int riBuyingManager::GetOpponentKillXPAward( int playerLevel ) {
	switch ( playerLevel )
	{
		case 1:
			opponentKillXPAward = 10;
			break;
		case 2:
			opponentKillXPAward = 20;
			break;
		case 3:
			opponentKillXPAward = 30;
			break;
		case 4:
			opponentKillXPAward = 40;
			break;
		case 5:
			opponentKillXPAward = 50;
			break;
		case 6:
			opponentKillXPAward = 60;
			break;
		case 7:
			opponentKillXPAward = 70;
			break;
		case 8:
			opponentKillXPAward = 80;
			break;
		case 9:
			opponentKillXPAward = 90;
			break;
		case 10:
			opponentKillXPAward = 100;
			break;
		default:
			opponentKillXPAward = 0;
			break;
	}
	return opponentKillXPAward;
}