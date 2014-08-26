//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	DigiBen@gametutorials.com		 //
//																		 //
//		$Program:		Ascii RPG (Part 4)								 //
//																		 //
//		$Description:	An RPG with ASCII graphics						 //
//																		 //
//***********************************************************************//

#include "Main.h"


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// This is where all the cut scenes and action key handling will be done.
// Only 2 of these functions are action key functions, HandleKey() and HandleMaps().
// The rest of these functions are just for the game's story, etc...  You will
// delete most of them when you make your own game/project.  The method of doing a 
// cut scene is simple.  We just make a separate function that writes a bunch
// of dialog to the screen, with pauses between each one.  We then can use functions
// to move the players or monsters/npcs which will make it look like an animated cut
// scene.  We do this with the King animation in the castle.  Nothing is hard, tricky
// or complicated about these functions below.
//
// This is how the action keys work.  We have an array, m_actionKeys, that stores
// about 1000 slots for action keys.  We then set a certain index into that array
// as a key.  For instance, the kKingSceneKey is 1.  When this key is set to something
// other than 0, it means that we have already done the King cut scene.  That way
// when we walk into the castle we don't always have the king stop us and then give
// us a quest.  That is how we do the progression of the story.  However, we don't always
// need to set the key, like in the case of a shop keeper.  The shop keepers key is
// 555.  That means when ever we talk to an npc, we will first check if it has a action key.
// If it does, we then have an if statement that handles this key.  We don't set this
// index into our action key array because we always want to be able to buy from the shops.
// So the key is just to tell us what type of npc it is.  It has nothing to do with
// the action key since we never set or check that index.  We just check the number
// of the npc's attached action key.  We will get into shops in the next tutorial.
//
//


//////////////////////////////// CACTION /////////////////////////////////
/////
/////	This is the CAction class constructor
/////
//////////////////////////////// CACTION /////////////////////////////////

CAction::CAction()
{
	// Let's reset all of the action keys in the array to 0
	memset(m_actionKeys, 0, sizeof(m_actionKeys));
}



//////////////////////////// DISPLAY CUT SCENE DIALOG /////////////////////////////
/////
/////	This displays dialog, delays, reset the input and waits for a key to continue
/////
//////////////////////////// DISPLAY CUT SCENE DIALOG /////////////////////////////

void DisplayCutSceneDialog(char *szDialog)
{
	// This is used to do our cut scene dialog scenes since we always want
	// to display the dialog, wait so the user can read it, then wait for a
	// key to be pressed before going on.
	g_Map.DisplayDialog(szDialog);
	Sleep(1000);
	FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
	getch();
}



///////////////////////////// ADD JAX TO GROUP //////////////////////////////
/////
/////	This does the cut scene with Jax and then adds him to our party
/////
///////////////////////////// ADD JAX TO GROUP //////////////////////////////

void AddJaxToGroup(CNpc *pNpc)
{
	char szDialog[5000] = {0};

	// This is our first cut scene we will go over.  What happens here is that
	// you are talking to a guy named Jax in a shop. He wants to join your group.
	// After the dialog is all done, we will add him to our group so that he will
	// follow us.  We never actually display his normal Npc player dialog because
	// we will do this custom dialog instead.  Jax is an Npc, so we need to convert
	// him into a player, then delete the Npc on the map.  Each dialog line will
	// display on the screen, pause, then wait for the user to go on.  Neat huh :)
	DisplayCutSceneDialog("Jax: So you are Adol huh?  Your look as good as they say.");
	DisplayCutSceneDialog("Adol: Do I know you?");
	DisplayCutSceneDialog("Jax: No, but I've heard of you.  Allow me to introduce myself.  My name's Jax.");
	DisplayCutSceneDialog("Adol: ...");
	DisplayCutSceneDialog("Jax: Listen, ... I know you're on a quest to go and find the King's gold right?");
	DisplayCutSceneDialog("Adol: Maybe...");
	DisplayCutSceneDialog("Jax: I'm sure you are doing fine by yourself, but I think you could use another man against some of those monsters up north.");
	DisplayCutSceneDialog("Adol: Who says I'm looking for a partner?");
	DisplayCutSceneDialog("Jax: Well, let's just say that you'll wish you had when you go further north.  The monsters down here aren't so bad, but past Rivenwood is another story.");
	DisplayCutSceneDialog("Adol: Hmmm... What's in it for you?");
	DisplayCutSceneDialog("Jax: Women dig guys who do quests... .... it's probably better that you don't ask...");
	DisplayCutSceneDialog("Adol: Okay Jax, your on board.  Just try not to slow me down.");
	DisplayCutSceneDialog("Jax: I'll be right behind you.  (Literally)");
	DisplayCutSceneDialog("Adol: Ok, let's go.");
	DisplayCutSceneDialog("Jax: Oh, and Adol?");
	DisplayCutSceneDialog("Adol: Yah?");
	DisplayCutSceneDialog("Jax: ...  .... Can you keep what I said about the women to yourself?");
	DisplayCutSceneDialog("Adol: *Adol rolls his eyes*");


	// Now we need to create a new player dynamically, otherwise it will cause memory
	// problems later.  We will free this later in the CPlayer deconstructor.  After
	// creating memory for the player, we set his image, data, etc...
	CPlayer *pJax = new CPlayer;
	CHAR_INFO jaxImage = {1, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE};
	pJax->Init(jaxImage, pNpc->GetIndex().X, pNpc->GetIndex().Y);
	pJax->SetHealthMax(200);
	pJax->SetHealth(200);
	pJax->SetProtection(25);
	pJax->SetStrength(75);
	pJax->SetName(pNpc->GetName());

	// Let's add the new Jax player to our party and delete the Npc from the map
	g_Player.AddPlayerToParty(pJax);
	g_Map.DeleteTile(kNpcType, pNpc->GetIndex().X, pNpc->GetIndex().Y);
}


//////////////////////////////// HANDLE KEY /////////////////////////////////
/////
/////	This function takes an action key and a tile type to handle
/////
//////////////////////////////// HANDLE KEY /////////////////////////////////

void CAction::HandleKey(int actionKey, CTile *pTile)
{
	// This is our main action key function.  This will take an action key
	// so we know what should happen, and then a tile, which is default to NULL.
	// We don't always need a tile (which can be casted to a npc, exit, etc...),
	// but it can be necessary sometimes for your game situations.

	// If the key is the shop keeper key
	if(actionKey == kShopKeeperKey)
	{
		// Currently we don't handle shops
	}
	// If the key is to add Jax 
	else if(actionKey == kAddJaxKey)
	{
		// This is an example of when we would need to use the pTile
		// parameter.  The pTile is really an Npc that we passed in, Jax.
		// We want to cast the CTile pointer to a CNpc pointer.  Next we
		// do the cut scene of adding Jax, then set the Jax to a 1 to make
		// sure we know that this event has already happened.
		if(!m_actionKeys[actionKey])
		{
			CNpc *pNpc = (CNpc*)pTile;
			AddJaxToGroup(pNpc);
			m_actionKeys[actionKey] = 1;
		}
	}
	// If the key is the treasure key for the end of the game
	else if(actionKey == kTreasureKey)
	{
		// First, set the treasure key to being accomplished
		m_actionKeys[kTreasureKey] = 1;

		// To make a different ending, we check if there if Jax is with
		// you or not.  If he is we do a funny ending with dialog, otherwise 
		// just do a simple dialog line from Adol saying he found the treasure.
		if(g_Player.GetPartySize() > 1)
		{
			DisplayCutSceneDialog("Adol: We found the treasure!");
			DisplayCutSceneDialog("Jax: Sweet!  Now I can get those ... ... nevermind");
			DisplayCutSceneDialog("Adol: Get those feelings of a job well done?");
			DisplayCutSceneDialog("Adol: Yah, ... that's it *smile*.");
		}
		else
		{
			DisplayCutSceneDialog("Adol: I found the treasure!");
		}

		// Display the end game message and then wait for input, then quit
		g_Menu.DrawMessageBox("You won!  G * A * M * E  O * V * E * R"); 
		Sleep(3000);
		getch();
		exit(0);
	}
}


//////////////////////////// DO CASTLE CUT SCENE /////////////////////////////
/////
/////	This is the cut scene for when the King gives Adol his quest
/////
//////////////////////////// DO CASTLE CUT SCENE /////////////////////////////

void DoCastleCutScene()
{
	// Let's draw the new map for inside the caste, then do a short pause
	g_Map.SetDrawFlag(true);
	g_Map.Draw();
	Sleep(1000);

	// Display the King's first dialog
	DisplayCutSceneDialog("King: Adol, please come here.");
	
	// To make some simple animation in the cut scene, we just make
	// Adol step up towards the King 18 times so that he is right
	// in front of the King.  Each step we do a small delay and draw.
	for(int i = 0; i < 18; i++)
	{
		g_Player.Move(kUp);
		g_Map.SetDrawFlag(true);
		g_Map.Draw();
		Sleep(200);
	}

	// After Adol stops, we do a traditional delay before the dialog begins after animation :)
	Sleep(1000);

	// Give adol the quest!
	DisplayCutSceneDialog("King: Adol, you have come at a very important time.  My treaure has been stolen!");
	DisplayCutSceneDialog("Adol: Oh... I am sorry to hear that King.");
	DisplayCutSceneDialog("King: Yes yes, but we need your help to find it.  You see, no one knows who stole it or where it went.");
	DisplayCutSceneDialog("King: Will you search the kingdom far and wide and bring me back my treasure?");
	DisplayCutSceneDialog("Adol: I'll do my best...");
	DisplayCutSceneDialog("King: Good, be sure to talk to everyone.  You might be able to get some good tips as to who might have done this horrible thing.");
	DisplayCutSceneDialog("Adol: Sure thing.");
	DisplayCutSceneDialog("King: Good luck, and be careful.  There has been a huge increase of monsters around.");
}


///////////////////////////// DO VALKAR CUT SCENE //////////////////////////////
/////
/////	This is the cut scene that happens when we enter the palace and meet Valkar
/////
///////////////////////////// DO VALKAR CUT SCENE //////////////////////////////

void DoValkarCutScene()
{
	// Draw the map, then do a quick 1 second pause
	g_Map.SetDrawFlag(true);
	g_Map.Draw();
	Sleep(1000);

	// This function is called right when we walk into the InsidePalace.map room.  Valkar
	// is standing right in front of the treasure as a big "V".  We have a little dialog
	// between Adol and Valkar, then we need to make our way up to the treasure to win.

	// Next we want Valkar to start speaking to us and initiate the last show down
	DisplayCutSceneDialog("Valkar: Who do we have here?  The great Adol that I have heard about?");
	DisplayCutSceneDialog("Adol: Who are you?");
	DisplayCutSceneDialog("Valkar: You can call me Valkar.");
	DisplayCutSceneDialog("Adol: Valkar, ... why does that name ring a bell...");
	DisplayCutSceneDialog("Valkar: Ha ha ha...");
	DisplayCutSceneDialog("Adol: Wait!  I remember you, you were the Kings servant!  You were banned from Tafar for murder.  You disguised yourself as a human.");
	DisplayCutSceneDialog("Valkar: That's impressive weakling.  I am a monster.  I stole the treasure and you aren't going to get it back.");
	DisplayCutSceneDialog("Adol: We'll see about that monster punk.");
	DisplayCutSceneDialog("Valkar: Ouch, that hurt....");
	DisplayCutSceneDialog("Adol: Oh... sorry... monster ... guy!");
	DisplayCutSceneDialog("Valkar: That was nice of you to take back that hurtfull thing.");
	DisplayCutSceneDialog("Adol: No problem...  My girlfriend says I'm a nice guy.");
	DisplayCutSceneDialog("Valkar: That's cool....  .... .... anyway, you won't get this treasure behind me!  Prepare to die");

	// Just for fun, we play a crazy little laugh from Valkar
	PlaySound("EvilLaugh.wav", NULL, SND_FILENAME | SND_ASYNC);
}


/////////////////////////// ADD THE TREASURE TO ROOM ////////////////////////////
/////
/////	This creates the treasure item on the fly and inserts it into the map
/////
/////////////////////////// ADD THE TREASURE TO ROOM ////////////////////////////

void AddTheTreasureToRoom()
{
	// Create a new treeasure item and give it a normal item image
	CItem treasure;
	CHAR_INFO treasureImage = {'i', FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE};

	// We just need to set up the treasures action key, image and position.  The rest
	// isn't necessary since the player will immediately win the game once they get it.
	treasure.SetActionKey(kTreasureKey);
	treasure.SetChar(treasureImage);
	treasure.SetIndex(kTreasureX, kTreasureY);

	// Insert the item into our map right behind Valkar
	g_Map.InsertTile(&treasure, kItemType, kTreasureX, kTreasureY);
}


//////////////////////////////// HANDLE MAPS /////////////////////////////////
/////
/////	This function handles special events for certain maps upon opening
/////
//////////////////////////////// HANDLE MAPS /////////////////////////////////

void CAction::HandleMaps()
{
	// Every time a map loads, this function is called to see if we need to
	// do a special event, like in the case of meeting the King or Valkar for
	// the first time.  You can also use this function to start a new MIDI
	// file, like we do for the Valkar scene in InsidePalace.map.  Another
	// important thing we do is get rid of the Jax Npc once he has joined
	// our party.  Jax will join our party, and if we go back into that
	// map, we don't want to see the Npc "Jax" sitting in the shop if he
	// is already attached to us in our party, so we check for this and then
	// delete that npc if Jax has already joined our party.  This is another
	// way action keys come into play.

	// Get the current map name
	string strMap = g_Map.GetMapName();

	// If the current map is the castle
	if(strMap == "Castle.map")
	{
		// If we haven't done the King cut scene, let's do it
		if(!m_actionKeys[kKingSceneKey])
		{
			DoCastleCutScene();
			m_actionKeys[kKingSceneKey] = 1;
		}
	}
	// If the current map is the second shop map in Caldwell
	else if(strMap == "Shop2.map")
	{
		// Check if Jax has already been added to our party, if so, delete his npc
		if(m_actionKeys[kAddJaxKey])
		{
			g_Map.DeleteNpc("Jax");
		}
	}
	// Check if the current map is where Valkar is, inside the palace
	else if(strMap == "InsidePalace.map")
	{
		// Let's play the cool fighting boss music
		g_Music.PlayNewSong("valkar.mid");

		// We also need to create and place the treasure in the room
		AddTheTreasureToRoom();

		// If we haven't talked to Valkar yet, we need to do the cut scene.  We
		// then set that key so that every time we enter the room again it won't
		// jump into the long dialog that we already heard.
		if(!m_actionKeys[kValkarSceneKey])
		{
			DoValkarCutScene();
			m_actionKeys[kValkarSceneKey] = 1;
		}
	}
	// If the map is the palace map, we want to switch the music back from
	// the big boss music that Valkar had and return it back to the default music.
	else if(strMap == "Palace.map")
	{
		// Get the current song name
		string strSong = g_Music.GetFileName();

		// If the current song is the big boss music (Valkar.mid) then do a new song
		if(strSong == "valkar.mid")
		{
			g_Music.PlayNewSong("default.mid");
		}
	}
}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


////////////////////////////////////////////////////////////////////
//
// *Quick Notes*
//  
// We implemented the functions necessary to have cut scenes and handle
// special events using action keys.  The important functions to focus
// on are HandleKey() and HandleMaps().  The rest of the functions are
// just extra function for this game that do cut scenes and special
// events.
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com
//
// © 2000-2005 GameTutorials  
