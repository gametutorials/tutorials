//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	DigiBen@gametutorials.com		 //
//																		 //
//		$Program:		Ascii RPG (Part 6)								 //
//																		 //
//		$Description:	An RPG with ASCII graphics						 //
//																		 //
//***********************************************************************//

#include "Main.h"

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// Nothing new was added to this file.
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
	// Create a variable to store the item names
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
		CHAR_INFO image = {'I', FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE};

		// Read in the current line for this item in the info file
		fscanf(fp, "%s > lifeInc: %d strengthInc: %d protectionInc: %d type: %s\n", szName, &life, &str, &protection, szType);
			
		// Set the info read in for this item
		tempItem.SetChar(image);
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
	// Calculate the price for the item, according to it's stats
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

	// Drop the item and delete it from the map to get rid of it from the player
	g_Player.DropItem(pItem);
	g_Map.DeleteTile(kItemType, pItem->GetIndex().X, pItem->GetIndex().Y);

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
	g_Shop.DrawBox(kMenuWidth - 3, kMenuHeight - 6, kMenuX, kMenuY);
	
	// If we are buying, then display a buying button, otherwise create a selling button
	if(bBuying)
		g_Shop.DrawString("Buy", 3, kMenuX + 5, kMenuY + 3, NULL,  ItemPromptBuy);
	else
		g_Shop.DrawString("Sell", 4, kMenuX + 5, kMenuY + 3, NULL,  ItemPromptSell);

	// Add a cancel button (or you can just hit ESC to cancel)
	g_Shop.DrawString("Cancel", 6, kMenuX + 5, kMenuY + 6, NULL,  ItemPromptCancel);

	// Now we do our input loop where we wait for either escape or a menu choice
	while(!g_Shop.EscapePressed() && !g_Shop.ChoiceMade())
	{
		// Handle the input from the user (The same function as CMenu uses)
		g_Shop.HandleInput();
	}

	// If a button was clicked, let's go to it's associated function
	if(g_Shop.ChoiceMade())
		g_Shop.ChoiceMade()->m_function();

	// Let's go back to the main inventory screen after the user confirms their choice
	DrawInventoryScreen();
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
	g_Shop.DrawString(szStats, (int)strlen(szStats), kStatsMenuX + 3, kStatsMenuY + 2, NULL);

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
	char szItem[25]   = {0};

	// Reset the menu's data to start a new menu
	g_Shop.ResetButtons();
	g_Shop.ResetEscapeFlag();
	g_Shop.ResetMenuChoice();
	g_Shop.ResetItemChoice();

	// Draw the map screen again and then draw the inventory menu's dialog boxes
	g_Map.Draw();
	g_Shop.DrawBox(kEqMenuWidth, kEqMenuHeight, kEqMenuX, kEqMenuY);
	g_Shop.DrawBox(kInvMenuWidth, kInvMenuHeight, kInvMenuX, kInvMenuY);
	g_Shop.DrawBox(kStatsMenuWidth, kStatsMenuHeight, kStatsMenuX, kStatsMenuY);

	// Draw the item stats in the bottom dialog box (initially nothing)
	DrawItemStats();

	// Go through all of the items and display them
	for(int i = 0; i < (int)pItemList->size(); i++)
	{
		// Get the current item in the list (make sure to dereference the list to access it)
		CItem *pInvItem = &((*pItemList)[i]);

		// Initially we have these offsets for the item, but will change for different columns
		int xOffset = 3, yOffset = i + 2;

		// Here we check if we need to move to another column for displaying the items
        if(i >= kMaxItemHeight)
		{
			// Set a new offset to create a new column for the next items
			xOffset += kMaxItemWidth * (i / kMaxItemHeight);
			yOffset = i - ((i/kMaxItemHeight) * kMaxItemHeight) + 2;
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

		// If the user clicked a new item (the user clicked on this item for the first time)
		if(g_Shop.GetSelectedItem() && pItem != g_Shop.GetSelectedItem())
		{
			// Get the selected item and draw it's stats in the bottom dialog box
			pItem = g_Shop.GetSelectedItem();
			DrawItemStats();
			g_Shop.ResetMenuChoice();
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

		// If the user clicked a new item (the user clicked on this item for the first time)
		if(g_Shop.GetSelectedItem() && pItem != g_Shop.GetSelectedItem())
		{
			// Get the selected item and draw it's stats in the bottom dialog box
			pItem = g_Shop.GetSelectedItem();
			DrawItemStats();
			g_Shop.ResetMenuChoice();
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

	DrawString("Buy", 3, kMenuX + 3, kMenuY + 2, NULL,  ShowBuyMenu);
	DrawString("Sell", 4, kMenuX + 12, kMenuY + 2, NULL,  ShowSellMenu);

	// Loop until the user escapes or clicks on one of the buttons
	while(!EscapePressed() && !ChoiceMade())
	{
		HandleInput();
	}

	// Reset the flag that says we hit escape and redraw the map
	ResetEscapeFlag();
	g_Map.SetDrawFlag(true);
	g_Map.Draw();

	// If there was a button clicked, let's go to it's associated function
	if(ChoiceMade())
		ChoiceMade()->m_function();
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


////////////////////////////////////////////////////////////////////
//
// *Quick Notes*
//  
// Nothing new was added to this file.
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com
//
// © 2000-2005 GameTutorials  
