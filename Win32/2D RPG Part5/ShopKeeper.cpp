//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	DigiBen@gametutorials.com		 //
//																		 //
//		$Program:		RPG (Part 5)									 //
//																		 //
//		$Description:	A basic example of a 2D RPG						 //

#include "Main.h"

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// This is where the beef of our tutorial is.  We define the CShop class functions
// and create functions which will be used as function pointers for our menu
// buttons.  Basically, a menu will pop up asking the player if they want to
// buy or sell.  Depending on that choice, we will display different things
// in our menu.  If we buy, then we will show the shop keepers items with their
// cost.  If we sell, it will show the player's items with no cost, but when
// the player selects an item (click twice to sell) the shop keeper will give
// the player a bid on their item (half the original price).  You can then
// choose if you want to sell or not. If you buy or sell, the item is added or
// taken away from your inventory, respectively.  The shop keeper loads
// random items from the Items.txt file every time you talk to them.  The shop
// code is very similar to the menu code, so it should be very simple and familiar
// to follow.  Let's bungee jump down into the code!
//
//


///////////////////////////////// CSHOP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This is the default constructor for the CShop class
/////
///////////////////////////////// CSHOP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CShop::CShop():m_bBuying(false)
{
}


///////////////////////////////// OPEN STORE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This stores the shop keeper's npc, loads the items, then draw the menu
/////
///////////////////////////////// OPEN STORE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CShop::OpenStore(CNpc *pShopKeeper)
{
	// When we open a store, we want to reset everything and store the npc
	Reset();
	m_pShopKeeper = pShopKeeper;

	// Next, we just load the random items from the text file and draw the menu
	g_Shop.Load();
	Draw();	
}


///////////////////////////////// LOAD \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This loads an item list and picks some random items to sell
/////
///////////////////////////////// LOAD \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CShop::Load()
{
	// This function is taken mainly from the function in our Map Editor
	// tutorial series, which loads the items into the global lists.  We did
	// change some things though, like the random selection of the items.
	// Basically, we just do some random checks to make sure there is random
	// items in the shop each time.  Depending on your project, you will most
	// likely want to change this functionality.  We set it up this way to
	// make a simple way of assigning items to a shop keeper for tutorial purposes.  
	// This is not the most effective way to do this as you can imagine.  You will
	// want to add more functionality in the map editor to assign items to shop keepers.

	// Create a variable to store the item names
	char szFileName[MAX_PATH] = {0};
	char szName[MAX_PATH] = {0};	
	char szType[MAX_PATH] = {0};	

	// Open our info file in "read" mode.
	FILE *fp = fopen(kItemInfoFile, "r");

	// Make sure we found the file, if not report the error
	if(!fp)
	{
		// Display an error message if we couldn't find the file
		MessageBox(NULL, "Unable to find items in the items file", "Error", MB_OK);
		return;
	}

	// Keep going through the file until we reach the end of it
	while(!feof(fp))
	{

		CItem tempItem;						   // Create a temp item tile
		int life = 0, str = 0, protection = 0; // Create variables for read data and item's image
		
		// Read in the current line for this item in the info file
		fscanf(fp, "%s > lifeInc: %d strengthInc: %d protectionInc: %d type: %s\n", szName, &life, &str, &protection, szType);
			
		// Store the actual file name of the tile
		strcpy(szFileName, kItemsDir);
		strcat(szFileName, szName);
		strcat(szFileName, ".bmp");

		// Set the info read in for this item
		tempItem.SetBitmap(g_Buffer.LoadABitmap(szFileName));
		tempItem.SetLifeInc(life);
		tempItem.SetStrengthInc(str);
		tempItem.SetProtectionInc(protection);
		tempItem.SetName(szName);

		// Let's grab the type of this item
		string strType = szType;
		int type = kItem;

		// Depending on the string read in, set the item's type
		if(strType == "head") type = kHead;
		else if(strType == "chest")  type = kChest;
		else if(strType == "weapon") type = kWeapon;
		else if(strType == "feet")   type = kFeet;

		// Set the numeric item type
		tempItem.SetItemType(type);

		// Set this tile type as an item
		tempItem.SetType(kItemType);
		
		// Here is where the random selection comes in.  We give this item a %50 chance
		// of not being added to the shop keeper's list of items being sold.
		if(rand() % 3)
			m_vItems.push_back(tempItem);

		// Make sure that the size of our item list isn't over the maximum limit for shops
		if((int)m_vItems.size() >= kMaxShopKeeperInv)
			break;
	}
	
	// Close the file, a success!
	fclose(fp);
}


///////////////////////////// CALCULATE ITEM PRICE \\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This calculates the new or used price of an item
/////
///////////////////////////// CALCULATE ITEM PRICE \\\\\\\\\\\\\\\\\\\\\\\\\\\\*

int CalculateItemPrice(CItem *pItem, bool bNew)
{
	// To calculate the price of an item, we just add it's stats together
	// and then multiply that by 2.  If it's used we don't do any multiplication.
	// We don't use the health stat because it would be huge.  Instead we just
	// make sure the item is at least $5.  If it's food, it will be at least $5.
	// We could however add a small ratio of the health stats if we wanted.
	int totalStats = pItem->GetStrengthInc() + pItem->GetProtectionInc();

	// Make sure it's at least 5 dollars
	if(totalStats <= 4)
		return 5;

	// If the item isn't new, we don't double it's price
	if(!bNew)
		return totalStats;

	// Return the doubled new retail price :)
	return totalStats * 2;
}


///////////////////////////// ITEM PROMPT BUY \\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function will be used as a function pointer to buy an item
/////
///////////////////////////// ITEM PROMPT BUY \\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void ItemPromptBuy()
{
	// When the user finally decides they want to buy an item, this function
	// is called.  We just need to get a pointer to the item, calculate it's
	// price, then see if the player has enough gold to even buy it.  If so,
	// we subtract the gold from the player to buy the item, then add it to
	// the inventory.

	// Get the selected item to buy and make sure it's valid
	CItem *pItem = g_Shop.GetSelectedItem();
	if(!pItem) return;

	// Copy the item's data directly over to a new item and calculate it's retail price (true)
	CItem newItem = *pItem;
	int salePrice = CalculateItemPrice(&newItem, true);

	// If the user can't afford the item, make them feel stupid for trying to cheat :)
	if(g_Player.GetGold() < salePrice)
	{
		g_Shop.DrawMessageBox("You don't have enough money for that!  This isn't a charity! ");
		Sleep(1500);
		return;
	}

	// Subtract the gold and add the item to the player's inventory
	g_Player.SetGold(g_Player.GetGold() - salePrice);
	g_Player.AddItem(newItem);
	
	// Display a message saying the player bought the item for such and such $$$
	char szMessage[80] = {0};
	sprintf(szMessage, "You bought the %s for %d gold.", newItem.GetItemName(), salePrice);
	g_Shop.DrawMessageBox(szMessage);
	Sleep(1500);
}


///////////////////////////// ITEM PROMPT SELL \\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function will be used as a function pointer to sell an item
/////
///////////////////////////// ITEM PROMPT SELL \\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void ItemPromptSell()
{
	// If the player finally decides to sell their item, this function is called.
	// First we get the item and then calculate it's USED price (false).
	CItem *pItem = g_Shop.GetSelectedItem();
	int salePrice = CalculateItemPrice(pItem, false);

	// We don't add the item to the shop keeper's list, but instead just make
	// the player drop it and then it is deleted from the map.  You can change
	// this if you want to have the option to buy the item back.
	g_Player.DropItem(pItem);
	g_Map.DeleteTile(kItemType, g_Player.GetPosition().x, g_Player.GetPosition().y);

	// Add the money made to our current gold
	g_Player.SetGold(g_Player.GetGold() + salePrice);
	
	// Display the transaction made
	char szMessage[80] = {0};
	sprintf(szMessage, "You recieved %d gold for the %s.", salePrice, pItem->GetItemName());
	g_Shop.DrawMessageBox(szMessage);
	Sleep(1000);
}


///////////////////////////// SHOW CONFIRM PROMPT \\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This brings up a prompt after the user picks an item to buy or sell
/////
///////////////////////////// SHOW CONFIRM PROMPT \\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void ShowConfirmPrompt(CItem *pItem, bool bBuying)
{
	// When the user chooses an item (clicks twice on the same item), this menu
	// will pop up.  It's a prompt that basically asks for you to confirm your
	// decision.  I guess if large grocery stores started doing this it would
	// really tick people off, but we will do it to protect our player from
	// accidental double clicks :)

	// First, let's reset the input from the menu delete all old buttons
	g_Shop.ResetMenuChoice();
	g_Shop.ResetButtons();

	// If we aren't buying, but selling, then give a gold quote to the player for their item
	if(!bBuying)
	{
		char szMessage[80] = {0};
		sprintf(szMessage, "I'll give you %d for it...", CalculateItemPrice(pItem, false));
		g_Shop.DrawMessageBox(szMessage);
		Sleep(1500);
	}

	// Now we actually want to draw the prompt menu for the user to confirm their decision
	g_Shop.DrawBox(kTileWidth*4, kTileHeight*3, kMenuX, kMenuY);
	
	// If we are buying, then display a buying button, otherwise create a selling button
	if(bBuying)
		g_Shop.DrawString("Buy", 3, kMenuX + kTileWidth, kMenuY + 29, NULL,  ItemPromptBuy);
	else
		g_Shop.DrawString("Sell", 4, kMenuX + kTileWidth, kMenuY + 29, NULL,  ItemPromptSell);

	// Add a cancel button (or you can just hit ESC to cancel)
	g_Shop.DrawString("Cancel", 6, kMenuX + kTileWidth, kMenuY + 61, NULL,  ItemPromptCancel);

	g_Buffer.SwapBackBuffer(FALSE);

	// Now we do our input loop where we wait for either escape or a menu choice
	while(!g_Shop.EscapePressed() && !g_Shop.ChoiceMade())
	{
		// Handle the input from the user (The same function as CMenu uses)
		g_Shop.HandleInput();
	}

	// If a button was clicked, let's go to it's associated function
	if(g_Shop.ChoiceMade())
		g_Shop.ChoiceMade()->m_function();

	g_Map.SetDrawFlag(true);
	g_Map.Draw();
}


///////////////////////////// DRAW ITEM STATS \\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This draws the player and selected item's stats in the shop menu
/////
///////////////////////////// DRAW ITEM STATS \\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void DrawItemStats()
{
	// Get the selected item to draw it's stats
	CItem *pItem = g_Shop.GetSelectedItem();
	char szStats[kStatsMenuWidth]  = {0};

	// If there is an item selected, display it's stats at the bottom of the menu
	if(pItem)
	{
		sprintf(szStats, "LifeInc: %d          strengthInc: %d         ProtectionInc: %d", 
						  pItem->GetLifeInc(), pItem->GetStrengthInc(), pItem->GetProtectionInc());
	}
	else
	{
		sprintf(szStats, "LifeInc:             strengthInc:            ProtectionInc:  ");
	}

	// Draw the stats string to the bottom menu dialog box
	g_Shop.DrawString(szStats, (int)strlen(szStats), kStatsMenuX + kTileWidth, kStatsMenuY + kTileHeight, NULL);

	// Display the players gold in the left dialog box
	sprintf(szStats, "$$$: %d",  g_Player.GetGold());
	g_Shop.DrawString(szStats, (int)strlen(szStats), kPlrStatsMenuX, kPlrStatsMenuY, NULL);
}


///////////////////////////// DRAW BUY SELL SCREEN \\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This draws the main part of the buy/sell menu and displays items and cost
/////
///////////////////////////// DRAW BUY SELL SCREEN \\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void DrawBuySellScreen(vector<CItem> *pItemList)
{
	// This function takes in a inventory list.  This could either
	// be the shop keeper's inventory (for buying) or the player's
	// inventory (for selling).  This way we cut down on code, since
	// we will use most of the same code for buying or selling.
	char szItem[25]   = {0};

	// Reset the menu's data to start a new menu
	g_Shop.ResetButtons();
	g_Shop.ResetEscapeFlag();
	g_Shop.ResetMenuChoice();

	// Draw the map screen again and then draw the inventory menu's dialog boxes
	g_Map.Draw();
	g_Shop.DrawBox(kEqMenuWidth, kEqMenuHeight, kEqMenuX, kEqMenuY);
	g_Shop.DrawBox(kInvMenuWidth, kInvMenuHeight, kInvMenuX, kInvMenuY);
	g_Shop.DrawBox(kStatsMenuWidth, kStatsMenuHeight, kStatsMenuX, kStatsMenuY);

	// Draw the item stats in the bottom dialog box (initially nothing)
	DrawItemStats();

	// Now we want to go through all the items in the inventory list passed in
	// and draw them to the screen in a formatted way, just like in a normal
	// inventory screen.
	for(int i = 0; i < (int)pItemList->size(); i++)
	{
		// Get the current item in the list (make sure to dereference the list to access it)
		CItem *pInvItem = &((*pItemList)[i]);

		// Initially we have these offsets for the item, but will change for different columns
		int xOffset = kTileWidth, yOffset = kTileHeight + i * kFontHeight;

		// Here we check if we need to move to another column for displaying the items
        if(i >= kMaxItemHeight)
		{
			// Set a new offset to create a new column for the next items
			xOffset += (kMaxItemWidth * (i / kMaxItemHeight)) * kTileWidth;
			yOffset = (i - ((i/kMaxItemHeight) * kMaxItemHeight)) * kFontHeight + kTileHeight;
		}
	
		// Here we store the retail price for the items in the list
		int salePrice = CalculateItemPrice(pInvItem, true);

		// If we are buying, we want to display the cost of the item next to it
		if(g_Shop.IsBuying())
            sprintf(szItem, "%s - $%d", pInvItem->GetItemName(), salePrice);
		else
			sprintf(szItem, "%s", pInvItem->GetItemName());

		// Draw the item to the screen as a button so the user can click on it to choose it
		g_Shop.DrawString(szItem, (int)strlen(szItem), kInvMenuX + xOffset, 
													   kInvMenuY + yOffset, pInvItem);
	}

	g_Buffer.SwapBackBuffer(FALSE);
}


///////////////////////////// SHOW BUY MENU \\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This shows the normal menu where the user can buy items
/////
///////////////////////////// SHOW BUY MENU \\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void ShowBuyMenu()
{
	// Let's tell the menu system that the user is buying items
	g_Shop.SetBuyFlag(true);

	// Draw the basic buy menu and display all the available items.
	// We pass in the shop keeper's inventory since we are buying.
	DrawBuySellScreen(g_Shop.GetInventory());

	// This pointer is used to tell if the player clicked on an item twice
	CItem *pItem = NULL;

	// Here we do our main loop for the buying menu
	while(!g_Shop.EscapePressed())
	{
		// We do a second loop to allow the user to choose items and not
		// have to leave the main buying menu.  Loop and check input.
		while(!g_Shop.EscapePressed() && !g_Shop.ChoiceMade())
			g_Shop.HandleInput();

		// "Let's do a happy little pause..."
		Sleep(100);

		// We want to quit this loop if the user hit escape
		if(g_Shop.EscapePressed())
			break;

		// If the user clicked a new item (the user clicked on this item for the first time)
		if(g_Shop.GetSelectedItem() && pItem != g_Shop.GetSelectedItem())
		{
			// Get the selected item and redraw the inventory screen and stats
			pItem = g_Shop.GetSelectedItem();
			g_Map.SetDrawFlag(true);
			g_Map.Draw();
			DrawBuySellScreen(g_Shop.GetInventory());
			g_Shop.ResetMenuChoice();
			g_Buffer.SwapBackBuffer(FALSE);

		}
		// If the user clicked a second time on the item, let's confirm their choice
		else if(pItem && pItem == g_Shop.GetSelectedItem())
		{
			// Show a confirm prompt menu (true for buying), then check for input
			ShowConfirmPrompt(pItem, true);
			
			// After the user confirms their decision, it will come here, so let's redraw
			DrawBuySellScreen(g_Shop.GetInventory());
			pItem = NULL;
		}
	}
}


///////////////////////////// SHOW SELL MENU \\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This shows the selling menu where the user sells their items
/////
///////////////////////////// SHOW SELL MENU \\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void ShowSellMenu()
{
	// Let's tell the menu system that the user is selling items
	g_Shop.SetBuyFlag(false);

	// Draw the basic sell menu and display all the player's items.
	// We pass in the player's inventory since we are selling.
	DrawBuySellScreen(g_Player.GetInventory());

	// This pointer is used to tell if the player clicked on an item twice
	CItem *pItem = NULL;

	// Here we do our main loop for the selling menu
	while(!g_Shop.EscapePressed())
	{
		// We do a second loop to allow the user to choose items and not
		// have to leave the main selling menu.  Loop and check input.
		while(!g_Shop.EscapePressed() && !g_Shop.ChoiceMade())
			g_Shop.HandleInput();

		// Give the user some pause love
		Sleep(100);

		// Break out of this loop if the user hit escape
		if(g_Shop.EscapePressed())
			break;

		// If the user clicked a new item (the user clicked on this item for the first time)
		if(g_Shop.GetSelectedItem() && pItem != g_Shop.GetSelectedItem())
		{
			// Get the selected item and redraw the inventory screen and stats
			pItem = g_Shop.GetSelectedItem();
			g_Map.SetDrawFlag(true);
			g_Map.Draw();
			DrawBuySellScreen(g_Player.GetInventory());
			g_Shop.ResetMenuChoice();
			g_Buffer.SwapBackBuffer(FALSE);
		}
		// If the user clicked a second time on the item, let's confirm their choice
		else if(pItem && pItem == g_Shop.GetSelectedItem())
		{
			// Show a confirm prompt menu (false for selling), then check for input
			ShowConfirmPrompt(pItem, false);
			
			// After the user confirms their decision, it will come here, so let's redraw
			DrawBuySellScreen(g_Player.GetInventory());
			pItem = NULL;
		}
	}
}


///////////////////////////////// DRAW \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This draws the main buy and sell menu and waits for input
/////
///////////////////////////////// DRAW \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CShop::Draw()
{
	// This draws the first menu to the player to have them select if they
	// want to buy or sell items.  First, draw menu and it's buy/sell buttons
	DrawBox(kShopPromptWidth, kShopPromptHeight, kMenuX, kMenuY);

	DrawString("Buy", 3, kMenuX + kTileWidth, kMenuY + kTileWidth-3, NULL,  ShowBuyMenu);
	DrawString("Sell", 4, kMenuX + kTileWidth*3, kMenuY + kTileWidth-3, NULL,  ShowSellMenu);

	// Swap the buffers to display the change
	g_Buffer.SwapBackBuffer(FALSE);

	// Loop until the user escapes or clicks on one of the buttons
	while(!EscapePressed() && !ChoiceMade())
	{
		HandleInput();
	}

	// Reset the flag that says we hit escape and redraw the map
	ResetEscapeFlag();
	g_Map.SetDrawFlag(true);
	g_Map.Draw();
	g_Buffer.SwapBackBuffer(FALSE);

	// If there was a button clicked, let's go to it's associated function
	if(ChoiceMade())
		ChoiceMade()->m_function();

	// More pausing for feeling :)
	Sleep(100);
}


///////////////////////////////// RESET \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This resets all the data in the shop menu
/////
///////////////////////////////// RESET \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CShop::Reset()
{
	// This clears all the data in our menu to start from scratch
	m_vItems.clear();
	ResetButtons();
	ResetEscapeFlag();
	ResetMenuChoice();
	ResetItemChoice();
}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////



////////////////////////////////////////////////////////////////////
//
// *Quick Notes*
//  
// We added all the code necessary code for our shop menus.  The
// CShop class inherits from the CMenu class, so there is many
// functions that didn't need to be coded, since it was already
// part of the normal CMenu class, like the drawing functions
// and the input functions.  You can now go to Shop.h to see
// the shop class declarations.
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com
//
// © 2000-2005 GameTutorials  
