/*
Author: Gunnar Miller
Date: July 2023

This program offers a facimile/product demo of an electronic book reader such as the Amazon Kindle.
It allows users to select any book in memery and displays the text of the book on screen, allowing the user to page through.
Beyond the book reading functionality, it has other menus that would be useful on an eReader for things like downloading books,
changing the language settings and turning on airplane mode. Most of these features are just there for display
and are currently non-functional, however, as they are beyond the scope of the current project. */


#include <iostream>
#include <string>
#include <algorithm>
#include <fstream>

using namespace std;

//global constants
const int BAR_HEIGHT = 3;
const int READER_WIDTH = 60;
const int SCREEN_HEIGHT = 10;
const int MAX_MENU_SIZE = SCREEN_HEIGHT/2;

string CLOSED[] = {
"          ============================================================",
"          | |                                                      | |",
"          | |                                                      | |",
"          | |                                                      | |",
"          | |                                                      | |",
"          | |              trans-Galactic eReader                  | |",
"          | |                                                      | |",
"          | |                                                      | |",
"          | |                                                      | |",
"          | |                                                      | |",
"          | |            by The Sirius Cybernetics Corp            | |",
"          | |                                                      | |",
"          | |                                                      | |",
"          | |                                                      | |",
"          | |                                                      | |",
"          | |                                                      | |",
"          | |                                                      | |",
"          | |                                                      | |",
"          | |                                                      | |",
"          | |                                                      | |",
"          | |                                                      | |",
"          ============================================================"

};

int closedHeight = 22;

string QUARTER[] = {
"                  /|                                                  ",
"                 / |                                                  ",
"                /  |                                                  ",
"               /   |                                                  ",
"              /    |                                                  ",
"             /     |                                                  ",
"            /      |                                                  ",
"           /       |                                                  ",	
"          /        |                                                  ",
"          |        |==================================================",
"          |        |                                               | |",
"          |        |                                               | |",
"          |        |elcome to the trans-Galactic eReader!          | |",
"          |        |                                               | |",
"          |        |                                               | |",
"          |        |                                               | |",
"          |        |                                               | |",
"          |        |                                               | |",
"          |        |                                               | |",
"          |        |                                               | |",
"          |        |                                               | |",
"          |        |alaxy's Best Books, All at Your Fingertips     | |",
"          |       /                                                | |",
"          |      /                                                 | |",
"          |     /                                                  | |",
"          |    /                                                   | |",
"          |   /                                                    | |",
"          |  /                                                     | |",
"          | /                                                      | |",
"          |/                                                       | |",
"          ============================================================"

};

int foldingHeight = 31;


string THREE_QUARTERS[] = {
"|\\                                                                   ",	
"| \\                                                                  ",	
"|  \\                                                                 ",	
"|   \\                                                                ",	
"|    \\                                                               ",	
"|     \\                                                              ",	
"|      \\                                                             ",
"|       \\                                                            ",
"|        \\============================================================",
"|         |                                                        | |",
"|         |                                                        | |",
"|         |                                                        | |",
"|         |        Welcome to the trans-Galactic eReader!          | |",
"|         |                                                        | |",
"|         |                                                        | |",
"|         |                                                        | |",
"|         |                                                        | |",
"|         |                                                        | |",
"|         |                                                        | |",
"|         |                                                        | |",
"|         |                                                        | |",
"\\         |                                                        | |",
" \\        |    The Galaxy's Best Books, All at Your Fingertips     | |",
"  \\       |                                                        | |",
"   \\      |                                                        | |",
"    \\     |                                                        | |",
"     \\    |                                                        | |",
"      \\   |                                                        | |",
"       \\  |                                                        | |",
"        \\ |                                                        | |",
"         \\============================================================"

};

string FULL_WIDTH[] = {

"|-----------------------------------------------------============================================================",
"|                                                     |                                                        | |",
"|                                                     |                                                        | |",
"|                                                     |                                                        | |",
"|                                                     |        Welcome to the trans-Galactic eReader!          | |",
"|                                                     |                                                        | |",
"|                                                     |                                                        | |",
"|              Property of:                           |                                                        | |",
"|             Tricia McMillian                        |                                                        | |",
"|          Sector ZZ9, Plural Z Alpha                 |                                                        | |",
"|          Islington, United Kingdom                  |                                                        | |",
"|                                                     |                                                        | |",
"|                                                     |                                                        | |",
"|                                                     |    The Galaxy's Best Books, All at Your Fingertips     | |",
"|                                                     |                                                        | |",
"|                                                     |                                                        | |",
"|                                                     |                                                        | |",
"|                                                     |                                                        | |",
"|                                                     |                                                        | |",
"|                                                     |                                                        | |",
"|                                                     |                                                        | |",
"|-----------------------------------------------------============================================================",


};

string FOLD_BACK[] = {
"          /===========================================================",
"         / |                                                       | |",
"        /  |                                                       | |",
"       /   |                                                       | |",
"      /    |       Welcome to the trans-Galactic eReader!          | |",
"     /     |                                                       | |",
"    /      |                                                       | |",
"   /       |                                                       | |",
"  /        |                                                       | |",
" /         |                                                       | |",
"/          |                                                       | |",
"|          |                                                       | |",
"|          |                                                       | |",
"|          |   The Galaxy's Best Books, All at Your Fingertips     | |",
"|          |                                                       | |",
"|          |                                                       | |",
"|          |                                                       | |",
"|          |                                                       | |",
"|          |                                                       | |",
"|          |                                                       | |",
"|          |                                                       | |",
"|          |                                                       | |",
"|         ============================================================",
"|        /                                                            ",
"|       /                                                             ",
"|      /                                                              ",
"|     /                                                               ",
"|    /                                                                ",
"|   /                                                                 ",
"|  /                                                                  ",
"| /                                                                   ",	
"|/                                                                    "
};

string OPEN[] = {
"          ============================================================",
"          | |                                                      | |",
"          | |                                                      | |",
"          | |                                                      | |",
"          | |                                                      | |",
"          | |      Welcome to the trans-Galactic eReader!          | |",
"          | |                                                      | |",
"          | |                                                      | |",
"          | |                                                      | |",
"          | |                                                      | |",
"          | |   The Galaxy's Best Books, All at Your Fingertips    | |",
"          | |                                                      | |",
"          | |                                                      | |",
"          | |                                                      | |",
"          | |                                                      | |",
"          | |                                                      | |",
"          | |                                                      | |",
"          | |                                                      | |",
"          | |                                                      | |",
"          | |                                                      | |",
"          | |                                                      | |",
"          ============================================================"

};

string getLine(int length, int& startIndex, string text);

int drawPage(int& startIndex, string text);

int drawMenu(string menu[], int menuSize, bool power, int menuIndex = 0); 

void drawBottom(int numOpts, bool pages, bool power);

void drawTop(string displayBar[]);

void drawDevice(string picture[], int size);



/*=============================================================================
					START OF MAIN()
===============================================================================*/

int main(){
	
	string displayBar[] = { "Battery:        Date:         Time:         Mode:            ",
	                        "  95%         02/07/2023      9:45         Standard          ",
							"                                                             "};
	
	string titleCatalog[100] = {"The Left Hand of Darkness", "Dune", "Much Ado About Nothing",  "The Cask of Amontillado",  "The Hobbit", "Steppenwolf"};
	string authorCatalog[100] = {"Ursala Le Guin", "Frank Herbert", "William Shakespeare", "Edgar Allen Poe", "JRR Tolkien", "Herman Hesse"};
	string alphaCatalog[100], titleSearchList[100], authorSearchList[100];
	int sortedListIndex[100];
	int catalogLength = 6;
	
	//this will be a very long section. Consider moving to a header file for readability						
	string texts[6];
	
	texts[0] = "                 The Left Hand of Darkness             "
		"                  by Ursala LeGuin               "
		"1. A Parade in Erhenrang                         "
		"Contents-From the Archives of Hain.              "
		"Transcript of Ansible Document 01-01101-934-2-Gethen:"
		"To the Stabile on Ollul:                          "
		"Report from Genly Ai, FirstMobileon Gethen/Winter, "
		"Hainish Cycle 93, Ekumenical Year 1490-97.         "
		"I'll make my reportas if I told a story, for I was taught as a child on my homeworld that Truth is a "
		"matter of the imagination. The soundest fact may fail or prevail in the style of its telling: like "
		"that singular organic jewel of our seas, which grows brighter as one woman wears it and, worn "
		"by another, dulls and goes to dust. Facts are no more solid, coherent, round, and real than "
		"pearls are. But both are sensitive.The story is not all mine, nor told by me alone. Indeed "
		"I am not sure whose story it is; you can judge better. But it is all one, and if at moments "
		"the facts seem to alter with an altered voice, why then you can choose the fact you like best; "
		"yet none of them are false, and it is all one story. "
		"It starts on the 44th diurnal of the Year 1491, which on the planet Winter in the nation Karhide "
		"was Odhar-hahad Tuwa or the twenty-second day of the third month of spring in the Year One. "
		"It is always the Year One here. Only the dating of every past and future year changes each "
		"New Year's Day, as one counts backwards or forwards from the unitary Now. So it was spring of the "
		"Year One in Erhenrang, capital city of Karhide, and I was in peril of my life, and did not know it";
	
	
	
	
	texts [1] =  "                          Dune                     "
	              "                by Frank Herbert                   "
	    "In the week before their departure to Arrakis, when all the final scurrying about had reached "
	    "a nearly unbearable frenzy, an old crone came to visit the mother of the boy, Paul. "
	    "It was a warm night at Castle Caladan, and the ancient pile of stone that had served the Atreides family "
	    "as home for twenty-six generations bore that cooled-sweat feeling it acquired before a change in the weather. "
	    "The old woman was let in by the side door down the vaulted passage by Paul's room and she was allowed a moment "
	    "to peer in at him where he lay in his bed. By the half-light of a suspensor lamp, dimmed and hanging near the floor, "
	    "the awakened boy could see a bulky female shape at his door, standing one step ahead of his mother. The old woman "
	    "was a witch shadow - hair like matted spiderwebs, hooded 'round darkness of features, eyes like glittering jewels. "
	    "\"Is he not small for his age, Jessica?\" the old woman asked. Her voice wheezed and twanged like an untuned baliset. "
	    "Paul's mother answered in her soft contralto: \"The Atreides are known to start late getting their growth, Your Reverence.\" "
	    "\"So I've heard, so I've heard,\" wheezed the old woman. \"Yet he's already fifteen.\" \"Yes, Your Reverence.\" "
	    "\"He's awake and listening to us,\" said the old woman. \"Sly little rascal.\" She chuckled. \"But royalty has need "
	    "of slyness. And if he's really the Kwisatz Haderach . . . well . . .\"";
	    
	    

	    
	              
	              
	              
	              
	texts [2]  = "         Much Ado About Nothing       " 
		"            by William Shakespeare           "
		"ACT I SCENE I. Before LEONATO'S house."
		"Enter LEONATO, HERO, and BEATRICE, with a Messenger "
		"LEONATO: I learn in this letter that Don Peter of Arragon comes this night to Messina."
		"Messenger: He is very near by this: he was not three leagues off when I left him."
		"LEONATO: How many gentlemen have you lost in this action?"
		"Messenger: But few of any sort, and none of name."
		"LEONATO: A victory is twice itself when the achiever bringshome full numbers."
		"I find here that Don Peter hath bestowed much honour on a young Florentine called Claudio."
		"Messenger: Much deserved on his part and equally remembered by Don Pedro: he hath borne himself beyond the "
		"promise of his age, doing, in the figure of a lamb, the feats of a lion: he hath indeed better "
		"bettered expectation than you must expect of me to tell you how."
		"LEONATO: He hath an uncle here in Messina will be very much glad of it."
		"Messenger: I have already delivered him letters, and there appears much joy in him; even so much that joy "
		"could not show itself modest enough without a badge of bitterness."
		"LEONATO: Did he break out into tears?"
		"Messenger: In great measure."
		"LEONATO: A kind overflow of kindness: there are no faces truer than those that are so washed. How much"
		"better is it to weep at joy than to joy at weeping!"
		"BEATRICE: I pray you, is Signior Mountanto returned from the wars or no?"
		"Messenger: I know none of that name, lady: there was none such in the army of any sort."
		"LEONATO: What is he that you ask for, niece?"
		"HERO: My cousin means Signior Benedick of Padua."
		"Messenger: O, he's returned; and as pleasant as ever he was.";
	
	
	texts [3] = "                 The Cask of Amontillado            "
		"                   by Edgar Allen Poe               "
		"THE thousand injuries of Fortunato I had borne as I best could, but when he ventured upon insult I vowed revenge."
		" You, who so well know the nature of my soul, will not suppose, however, that gave utterance to a threat. At length I"
		" would be avenged; this was a point definitely, settled --but the very definitiveness with which it was resolved precluded the idea of risk."
		"I must not only punish but punish with impunity. A wrong is unredressed when retribution overtakes its redresser. It is equally unredressed"
		" when the avenger fails to make himself felt as such to him who has done the wrong. "
		"It must be understood that neither by word nor deed had I given Fortunato cause to doubt my good will. I continued, as was my in to smile in his face,"
		"and he did not perceive that my to smile now was at the thought of his immolation. "
		"He had a weak point --this Fortunato --although in other regards he was a man to be respected and even feared. He prided himself on his connoisseurship"
		"in wine. Few Italians have the true virtuoso spirit. For the most part their enthusiasm is adopted to suit the time and opportunity,"
		"to practise imposture upon the British and Austrian millionaires. In painting and gemmary, Fortunato, like his countrymen, was a quack, but in the matter"
		"of old wines he was sincere. In this respect I did not differ from him materially; --I was skilful in the Italian vintages myself,"
		"and bought largely whenever I could. ";
		
	texts[4] = "                     The Hobbit                "
		"                     by JRR Tolkien                 "
		" Chapter I: An Unexpected Party                     "
		"In a hole in the ground there lived a hobbit. Not a nasty, dirty, wet hole, filled with the "
		"ends of worms and an oozy smell, nor yet a dry, bare, sandy hole with nothing in it to sit down "
		"on or to eat: it was a hobbit hole, and that means comfort. "
		"It had a perfectly round door like a porthole, painted green, with a shiny yellow brass knob "
		"in the exact middle. The door opened on to a tube shaped hall like a tunnel: a very comfortable "
		"tunnel without smoke, with panelled walls, and floors tiled and carpeted, provided with polished "
		"chairs, and lots and lots of pegs for hats and coats the hobbit was fond of visitors. The tunnel "
		"wound on and on, going fairly but not quite straight into the side of the hill The Hill, as all the "
		"people for many miles round called it and many little round doors opened out of it, first on "
		"one side and then on another. No going upstairs for the hobbit: bedrooms, bathrooms, cellars, "
		"pantries (lots of these), wardrobes (he had whole rooms devoted to clothes), kitchens, "
		"dining rooms, all were on the same floor, and indeed on the same passage. The best rooms "
		"were all on the left hand side (going in), for these were the only ones to have windows, "
		"deep set round windows looking over his garden and meadows beyond, sloping down to the river. ";
	
	
	
	
	texts[5] = "                     Steppenwolf                "
		"                    by Herman Hesse                "
		"       translated from German by Basil Creighton     "
		"THIS BOOK CONTAINS THE RECORDS LEFT US by a man whom, according to the "	
		"expression he often used himself, we called the Steppenwolf. Whether this manuscript needs any "
		"introductory remarks may be open to question. I, however, feel the need of adding a few pages to "
		"those of the Steppenwolf in which I try to record my recollections of him. What I know of him is "
		"little enough. Indeed, of his past life and origins I know nothing at all. Yet the impression left by "
		"his personality has remained, in spite of all, a deep and sympathetic one. "
		"Some years ago the Steppenwolf, who was then approaching fifty, called on my aunt to "
		"inquire for a furnished room. He took the attic room on the top floor and the bedroom next it, "
		"returned a day or two later with two trunks and a big case of books and stayed nine or ten months "
		"with us. He lived by himself very quietly, and but for the fact that our bedrooms were next door "
		"to each other\97which occasioned a good many chance encounters on the stairs and in the "
		"passage\97we should have remained practically unacquainted. For he was not a sociable man. "
		"Indeed, he was unsociable to a degree I had never before experienced in anybody. He was, in "
		"fact, as he called himself, a real wolf of the Steppes, a strange, wild, shy\97very shy\97being from "
		"another world than mine. How deep the loneliness into which his life had drifted on account of "
		"his disposition and destiny and how consciously he accepted this loneliness as his destiny, I "
		"certainly did not know until I read the records he left behind him. Yet, before that, from our "
		"occasional talks and encounters, I became gradually acquainted with him, and I found that the "
		"portrait in his records was in substantial agreement with the paler and less complete one that our "
		"personal acquaintance had given me. ";
		
	
	 
	   
	string notYetAdded[] = {"This feature is not yet available."};
	
	//text and controls for main menu
	string mainMenu[] = {"Download Books", "Book Library", "Device Settings"};
	int mainMenuLength = 3;
	int mainMenuButton = 0; 
	
	//text and controls for download menus
	string downloadMenu[] = {"Online Store", "Other Online Source", "Peer to Peer"};
	int downloadMenuLength = 3;
	string onlineDownloads[] = {"Sorry, no internet connection!"};
	int downloadMenuButton = 0, downloadSubMenuButton = 0; 
	
	
	//text and controls for library menus
	string libraryMenu[] = {"Display Alphabetical", "Display Chronological", "Search By Title", "Search by Author"};
	int libraryMenuLength = 4;
	int libraryMenuButton = 0, readerMenuButton = 0, bookMenuButton = 0; 
	int pageIndex = 0, pageNumber = 0; //keep track of current place in text
	int pageSizes[1000]; //keeps track of the size of pages in current text to allow paging backwards
	int menuIndex = 0, currentMenuSize = 0, lastOpt = 0; //variables to keep track of place in large menus
	ofstream outputFile("project01su23output_MillerG.txt");
	outputFile << "Summary of Books Accessed:" << endl << endl;
	
	//text and controls for settings menus
	string settingsMenu[] = {"Airplane Mode", "Language Select", "Text to Speech", "Storage Space"};
	int settingsMenuLength = 4;
	int settingsMenuButton = 0, settingsSubMenuButton = 0;  
	string languageMenu[] = {"English", "Espanol", "Deutsch"};
	int languageMenuLength = 3;
	string languageMenuText[][1] = {{"Language is already set to English"}, {"El idioma espanol no esta disponible."}, {"Deutsche Sprache ist leider nicht verfuegbar."}};
	string storageArray[] = {""};
	string textToSpeech[] = {"Text-to-speech is unavailable at this time."};
	string airplaneMode[] = {"Can't enter airplane mode now."};
	
	
	//draw animation for opening the reader
	drawDevice(CLOSED, closedHeight);
	
	drawDevice(QUARTER, foldingHeight);
	
	drawDevice(THREE_QUARTERS, foldingHeight);
	
	drawDevice(FULL_WIDTH, closedHeight);
	
	drawDevice(FOLD_BACK, foldingHeight);
	
	drawDevice(OPEN, closedHeight);

	//MAIN MENU CONTROL LOOP
	do{ 
	
		drawTop(displayBar);
		drawMenu(mainMenu, mainMenuLength, true);
		
		do{ cin >> mainMenuButton;	//keep accepting user input until user inputs until a valid input is entered
		} while (mainMenuButton < 1 || mainMenuButton > mainMenuLength + 1);
		
		switch(mainMenuButton){
		
		
		//DOWNLOAD MENU CONTROL BLOCK
		case 1:
		
			do{ 
				//display the configuration for the download menu
				drawTop(displayBar);
				drawMenu(downloadMenu, downloadMenuLength, false);
			
				do{  cin >> downloadMenuButton; //get menu input (keep asking until a valid input is entered)
				}while(downloadMenuButton < 1 || downloadMenuButton > downloadMenuLength + 1);
		
				switch(downloadMenuButton){
					
					case 1: //Online Store
						drawTop(displayBar);
						drawMenu(onlineDownloads, 1, false);
						do{cin >> downloadSubMenuButton;
						}while(downloadSubMenuButton != 2);
						downloadSubMenuButton = 0;
						break;
						
					case 2: //Other Online Source
						drawTop(displayBar);
						drawMenu(onlineDownloads, 1, false);
						do{cin >> downloadSubMenuButton;
						}while(downloadSubMenuButton != 2);
						downloadSubMenuButton = 0;
						break;
					
					case 3: //Peer to Peer
						drawTop(displayBar);
						drawMenu(notYetAdded, 1, false);
						do{cin >> downloadSubMenuButton;
						}while(downloadSubMenuButton != 2);
						downloadSubMenuButton = 0;
						break;
						
				}
		
			}while(downloadMenuButton != downloadMenuLength + 1);
			
			//reset menu buttons after exiting download menu
			downloadMenuButton = 0;
			break; 
		
		
		//LIBRARY MENU CONTROL BLOCK
		case 2: 
		
		
			do{ 
				
				//display the configuration for the library menu
				drawTop(displayBar);
				drawMenu(libraryMenu, libraryMenuLength, false);
				
				do{ cin >> libraryMenuButton; //get menu input (keep asking until a valid input is entered)
				}while(libraryMenuButton < 1 || libraryMenuButton > libraryMenuLength + 1);
				
				
				switch(libraryMenuButton){
					
					case 1: //display books alphabetically
					
						//copy the title catalog into a new array
						for(int i = 0; i < catalogLength; i++){
							alphaCatalog[i] = titleCatalog[i];
						}
						
						//sort the new array alphabetically
						sort(alphaCatalog, alphaCatalog + catalogLength);
						
						//for each entry in the alpha-sorted array, step through the title catalog and 
						//record the index of the corresponding entry
						for(int i = 0; i < catalogLength; i++){
							
							for(int j = 0; j < catalogLength; j++){
								
								if(alphaCatalog[i] == titleCatalog[j]){
									sortedListIndex[i] = j;
									break;	
								}		
							}	
						}
					
						do{
							drawTop(displayBar); //draw alphabetical book menu
							currentMenuSize = drawMenu(alphaCatalog, catalogLength, false, menuIndex);
							lastOpt = currentMenuSize + 3; //3 buttons besides flex menu choices
							
							do{cin >> readerMenuButton;
							}while(readerMenuButton < 1 || readerMenuButton > lastOpt);
							
							if(readerMenuButton <= lastOpt -3){ 
								
								//when the reader chooses a book, write the title and author to the summary document
								outputFile << titleCatalog[sortedListIndex[menuIndex + readerMenuButton - 1]] << " by ";
								outputFile << authorCatalog[sortedListIndex[menuIndex + readerMenuButton - 1]] << endl << endl;
								
								do{
									drawTop(displayBar);
									pageSizes[pageNumber] = drawPage(pageIndex, texts[sortedListIndex[menuIndex + readerMenuButton - 1]]);
									
									do{cin >> bookMenuButton;
									}while(bookMenuButton < 1 || bookMenuButton > 3);
									
									if(bookMenuButton == 1) pageNumber += 1; //page forward
									else if(bookMenuButton == 2){
										pageIndex -= pageSizes[pageNumber];
										
										if(pageNumber > 0){
											pageNumber -= 1;
											pageIndex -=pageSizes[pageNumber];
										}
									}
									
								}while(bookMenuButton != 3); //should only ever be three buttons in the book menu
								
								pageNumber = 0;
								pageIndex = 0;
								
							}else if(readerMenuButton == lastOpt -2){ //page forward
								if(menuIndex + MAX_MENU_SIZE < catalogLength) menuIndex += MAX_MENU_SIZE;
							}else if(readerMenuButton == lastOpt - 1){ //page back
								menuIndex = max(menuIndex  - MAX_MENU_SIZE, 0);
							}else if(readerMenuButton == lastOpt){ //previous menu
								menuIndex = 0;
								currentMenuSize = 0;
							}
							
						}while(readerMenuButton != lastOpt); 
						
						break;
						
					case 2: //display books chronologically
						do{
							drawTop(displayBar); //draw chronological book menu
							currentMenuSize = drawMenu(titleCatalog, catalogLength, false, menuIndex);
							lastOpt = currentMenuSize + 3; //3 buttons besides flex menu choices
							
							do{cin >> readerMenuButton;
							}while(readerMenuButton < 1 || readerMenuButton > lastOpt);
							
							if(readerMenuButton <= lastOpt -3){ //select title
							
								//when the reader chooses a book, write the title and author to the summary document
								outputFile << titleCatalog[menuIndex + readerMenuButton - 1] << " by ";
								outputFile << authorCatalog[menuIndex + readerMenuButton - 1] << endl << endl;
								
								do{
									drawTop(displayBar);
									pageSizes[pageNumber] = drawPage(pageIndex, texts[menuIndex + readerMenuButton - 1]);
									
									do{cin >> bookMenuButton;
									}while(bookMenuButton < 1 || bookMenuButton > 3);
									
									if(bookMenuButton == 1) pageNumber += 1; //page forward
									else if(bookMenuButton == 2){
										pageIndex -= pageSizes[pageNumber];
										
										if(pageNumber > 0){
											pageNumber -= 1;
											pageIndex -=pageSizes[pageNumber];
										}
									}
									
								}while(bookMenuButton != 3); //should only ever be three buttons in the book menu
								
								pageIndex = 0;
								pageNumber = 0;
								
							}else if(readerMenuButton == lastOpt -2){ //page forward
								if(menuIndex + MAX_MENU_SIZE < catalogLength) menuIndex += MAX_MENU_SIZE;
							}else if(readerMenuButton == lastOpt - 1){ //page back
								menuIndex = max(menuIndex  - MAX_MENU_SIZE, 0);
							}else if(readerMenuButton == lastOpt){ //previous menu
								menuIndex = 0;
								currentMenuSize = 0;
							}
							
						}while(readerMenuButton != lastOpt); 
						break;
						
					case 3: //search by title
						drawTop(displayBar);
						drawMenu(notYetAdded, 1, false);
						do{cin >> readerMenuButton;
						}while(readerMenuButton != 2);
						readerMenuButton = 0;
						break;
						
					case 4: //search by author
						drawTop(displayBar);
						drawMenu(notYetAdded, 1, false);
						do{cin >> readerMenuButton;
						}while(readerMenuButton != 2);
						readerMenuButton = 0;
						break;
					
				}
		
			}while(libraryMenuButton != libraryMenuLength + 1);
			
			//reset menu buttons after exiting download menu
			libraryMenuButton = 0; 
			break;
		
		
		
		//SETTINGS MENU CONTROL BLOCK
		case 3: 
		
			//display the configuration for the settings menu
		
			do{ 
				
				drawTop(displayBar);
				drawMenu(settingsMenu, settingsMenuLength, false);
			
				do{ cin >> settingsMenuButton; //get menu input (keep asking until a valid input is entered)
				}while(settingsMenuButton < 1 || settingsMenuButton > settingsMenuLength + 1);
		
				switch(settingsMenuButton){
				
					case 1: //airplane mode
						drawTop(displayBar);
						drawMenu(airplaneMode, 1, false);
						do{cin >> settingsSubMenuButton;
						}while(settingsSubMenuButton != 2);
						settingsSubMenuButton = 0;
						break;
				
					case 2: //language select
						do{
							drawTop(displayBar);
							drawMenu(languageMenu, languageMenuLength, false);
						
							do{cin >> settingsSubMenuButton;
							}while(settingsSubMenuButton < 1 || settingsSubMenuButton > languageMenuLength + 1);
						
							if(settingsSubMenuButton != languageMenuLength + 1){
								drawTop(displayBar);
								drawMenu(languageMenuText[settingsSubMenuButton - 1], 1, false);
								settingsSubMenuButton = 0;
								do{cin >> settingsSubMenuButton;
								}while(settingsSubMenuButton != 2);
							}
						
						}while(settingsSubMenuButton != languageMenuLength + 1);
						
						break;
				
					case 3: //text to speech
						drawTop(displayBar);
						drawMenu(textToSpeech, 1, false);
						do{cin >> settingsSubMenuButton;
						}while(settingsSubMenuButton != 2);
						settingsSubMenuButton = 0;
						break;
					
					case 4: //storage space
						storageArray[0] = "Storage is " + to_string(catalogLength) + "% full.";
						drawTop(displayBar);
						drawMenu(storageArray, 1, false);
						do{cin >> settingsSubMenuButton;
						}while(settingsSubMenuButton != 2);
						settingsSubMenuButton = 0;
						break;
				}
		
			}while(settingsMenuButton != settingsMenuLength + 1);
			
			//reset menu buttons after exiting download menu
			settingsMenuButton = 0;
			break;
		
		}
		
		
		
	}while(mainMenuButton != mainMenuLength + 1);
	
	//closing animation is just opening animation in reverse
	drawDevice(OPEN, closedHeight);
	
	drawDevice(FOLD_BACK, foldingHeight);
	
	drawDevice(FULL_WIDTH, closedHeight);
	
	drawDevice(THREE_QUARTERS, foldingHeight);
	
	drawDevice(QUARTER, foldingHeight);
	
	drawDevice(CLOSED, closedHeight);

	return 0;	
	
}

/*=============================================================================
					END OF MAIN()
			 START OF FUNCTION DECLARATIONS
===============================================================================*/




/*==============================================================================================
                                getLine Function
================================================================================================
Finds the longest substring of a given string that stars from the specified position,
ends in whitespace and is less than the specified line length. Fills the remaining length of
the line with whitespace so that the returned string is always the same length. If the remainder
of the string is less than the line length, returns all of it. If no whitespace is found, returns
the maximum number of characters that will fit on the line. In all of these places it updates the 
index variable to reflect how much of the text it has printed. If the start index is out of bounds,
returns a full line of only whitespace and doesn't change the index variable.
*/
string getLine(int lineLength, int& startIndex, string text){
	
	int backSteps = 0;
	string line = "";
	
	if(text.length() <= startIndex){ //check if the index is out of bounds
	
		for(int i = 0; i < lineLength; i++){
			line += " "; //build up a line entirely out of whitespace
			}
		return line; //and return it
	}
	
	//check and see if all the remaining text will fit on one line. If so, output all of it (plus whitespace to fill out the line)
	if(lineLength >= text.length() - startIndex){
		
		line = text.substr(startIndex, text.length() - startIndex);
		while(line.length() < lineLength){ //fill out the remaining line with whitepsace
			line += " ";
		}
		startIndex = text.length();
		return line;
		
	}
	
	//step backwards through the text until you find the first space
	while((backSteps < lineLength) && text[startIndex + lineLength - backSteps - 1] != ' '){
		backSteps++;
	}
	
		
	if(backSteps == lineLength){ //if you didn't find a space before reaching the start position, return a full-length line of text
		line = text.substr(startIndex, lineLength);
		startIndex += lineLength;
	}
	else{
		line = text.substr(startIndex, lineLength - backSteps); //take just the portion of the text from the start position to the last space
		while(line.length() < lineLength){ //fill out the remaining line with whitepsace
			line += " ";
		}
		startIndex += (lineLength - backSteps);
	}
		
	
	return line;
	
}
/* =========================================================================================
						     drawPage Function
============================================================================================						     
 Draws a page of text to fill the main "screen" of the eReader. Starting point in the text 
is defined by the current value of the global index variable. Uses getLine to take equal-width 
chunks of text and whitespace, writes a number of lines of the text to the screen as defined by 
the height variable, with ASCII borders on either side of a line. Returns the total length of the
printed page in characters*/
int drawPage(int& startIndex, string text){
	
	int pageStart = startIndex;
	string leftEdge = "| | ";
	string rightEdge = " | |";
	int lineWidth = READER_WIDTH - leftEdge.length() - rightEdge.length(); //calculate how much room there is for text between the borders
	
	for(int i = 0; i < SCREEN_HEIGHT; i++){ //each line consists of the left border, the text grabbed by getLine and the right border
		
		cout << leftEdge << getLine(lineWidth, startIndex, text) << rightEdge << endl;
		
	}
	
	drawBottom(0, true, false);
	
	return startIndex - pageStart;
	
}

/*========================================================================================
                               drawMenu Function
==========================================================================================
Draws a menu to the screen based to the array of menu items passed in. Inserts a blank line
before each menu item, and fills the remaining screen height with blank lines when all                               
menu items have been displayed. Calls drawBottom function to populate the control field
with the buttons appropriate to the menu*/
int drawMenu(string menu[], int menuSize, bool power, int menuIndex){
	
	string leftEdge = "| | ";
	string rightEdge = " | |";
	int lineWidth = READER_WIDTH - leftEdge.length() - rightEdge.length(); //calculate how much room there is for text between the borders
	string blankLine = leftEdge;
	string menuLine;
	int linesLeft = SCREEN_HEIGHT;
	int displayOpts = min(MAX_MENU_SIZE, menuSize - menuIndex); //number of options this menu will display
	bool multiPage = false;
	if(menuSize > MAX_MENU_SIZE) multiPage = true;
	
	for(int i = 0; i < lineWidth; i++){  //add spoces to blankLine until it reaches the correct wdith
		
		blankLine += " ";
		
	}
	
	blankLine += rightEdge;
	
	/*For each item in the menu, build up the row for that item using the edge pieces, the appropriate 
	number and the text of the row. Then fill the rest of the line with whitespace. Print a blank line
	then print the row */
	for(int i = 0; i < displayOpts; i++){
		
		menuLine = leftEdge + to_string(i + 1) + ": "; //set up the edge and numbering
		menuLine += menu[menuIndex + i];                           //then add the text for that menu item
		
		while(menuLine.length() < lineWidth + leftEdge.length()){ //add blank lines until row reaches correct width
			menuLine += " ";
		}
		
		menuLine += rightEdge;   //add the right edge
		
		//print a blank line and then the rows
		cout << blankLine << endl;
		cout << menuLine << endl;
		linesLeft -= 2;  //decrement linesLeft to track the two rows just printed
		
	}
	
	while(linesLeft > 0){ //fill out the remainder of the screen with whitespace
		cout << blankLine << endl;
		linesLeft--;
	}
	
	drawBottom(displayOpts, multiPage, power);
	
	return displayOpts;
	
}



/* ========================================================================================
                               drawBottom Function
===========================================================================================
Draws the control field of the eReader, which contains a variable number of buttons. The numOpts
parameter indicates how many buttons must be drawn for the corresponding menu. In addition, it will
always draw either a "prev menu" button or a "power off" button as controlled by the power flag and
will draw "Next Page" and "Prev Page" buttons if the pages flag is set */

void drawBottom(int numOpts, bool pages, bool power){
	
	string leftEdge = "| | ";
	string rightEdge = " | |";
	char bottomEdge = '=';
	char borderElement = '_';
	int menuWidth = READER_WIDTH - leftEdge.length() - rightEdge.length();
	int topButtonSpacing = 0;
	if(numOpts > 0) topButtonSpacing = menuWidth/numOpts - 3; //tracks how much whitespace to draw between top buttons. Each top button has width 3
	int numBottomButtons = 1; //always a prev menu/power off botton on the bottom
	if(pages) numBottomButtons += 2; //may be page buttons as well
	int bottomButtonInterval = menuWidth/numBottomButtons; //tracks the width allocated for each bottom button
	string topRow = "", bottomRow = "", bottomButton = "";
	
	
	//build a spacer row string of the appropriate width
	string spacerRow = leftEdge;
	string bottomBorder = "========";
	string areaBorder = "| |_";
	
	for(int i = 0; i < menuWidth; i++){
		spacerRow += " ";
		bottomBorder += bottomEdge;
		areaBorder += borderElement;
	}
	
	spacerRow += rightEdge;
	areaBorder = areaBorder + borderElement + "| |";
	
	//building up the top row
	if(numOpts > 0){
	
		for(int i = 0; i < numOpts; i++){
			topRow += "[" + to_string(i + 1) + "]";
			for(int j = 0; j < topButtonSpacing; j++){
				topRow += " ";
			}
		}
	
		while(topRow.length() < menuWidth){ //add extra whitespace to the top row to reach the right width
			topRow += " ";
		}
	
		topRow = leftEdge + topRow + rightEdge; //add the edges
	}
	else topRow = spacerRow;
	
		
	//building up the bottom row
	
	//If the screen has pages, the first two bottom buttons are page forward and page back
	if(pages){
		
		bottomButton = "[" + to_string(numOpts + 1) + " (Next Page)" + "]";
		while(bottomButton.length() < bottomButtonInterval){
			bottomButton += " ";
		}
		
		bottomRow += bottomButton;
		
		bottomButton = "[" + to_string(numOpts + 2) + " (Prev Page)" + "]";
		while(bottomButton.length() < bottomButtonInterval){
			bottomButton += " ";
		}
		
		bottomRow += bottomButton;
		
	}
	
	//the final button on the bottom will either be the power button or the previous menu button

	bottomButton = "[" + to_string(numOpts + numBottomButtons);
	
	if(power) bottomButton += " (Power Off)";
	else bottomButton += " (Prev Menu)";
	bottomButton += "]";	
	bottomRow += bottomButton;
	
	while(bottomRow.length() < menuWidth){ //add extra whitespace to reach the right width
		bottomRow += " ";
	}
	
	bottomRow = leftEdge + bottomRow + rightEdge; //add the edges
	
	cout << areaBorder << endl;
	cout << spacerRow << endl;
	cout << topRow << endl;
	cout << spacerRow << endl;
	cout << bottomRow << endl;
	cout << spacerRow << endl;
	cout << bottomBorder << endl << endl;

	
}

/* ==============================================================================================
                               drawTop Function
=================================================================================================
Draws the top of e-Reader, including the ASCII border and a display bar whose contents are defined
by a string array passed in. Will cause an error if it encounters an array element with length less
than the width of the display bar. */
void drawTop(string displayBar[]){

	char topEdge = '=';
	char borderElement = '_';
	string leftEdge = "| | ";
	string rightEdge = " | |";
	int displayBarWidth = READER_WIDTH - leftEdge.length() - rightEdge.length();
	string displayBarSlice = "";
	string areaBorder = "| |_";
	string topRow = "========";
	
	cout << endl;
	for(int i = 0; i < displayBarWidth; i++){
		topRow += topEdge;
		areaBorder += borderElement;
	}
	
	areaBorder += borderElement;
	areaBorder += "| |";
	
	cout << endl << topRow << endl; 
	
	for(int i = 0; i < BAR_HEIGHT; i++){
		
		displayBarSlice = displayBar[i].substr(0,displayBarWidth);
		cout << leftEdge << displayBarSlice << rightEdge << endl;
		
	}
	
	cout << areaBorder << endl;


}

/* ==============================================================================================
                               drawDevice
=================================================================================================
This function takes in an array of strings and outputs the result to the screen along with a prompt to keep clicking. */

void drawDevice(string picture[], int size){
	
	for(int i = 0; i < size; i++){
		
		cout << picture[i] << endl;
	}
	
	cout << endl;
	system("pause");
}



