/*Author: Gunnar Miller
Date: July 2023

Project for fuel efficiency for interstellar rocket travel. See related Word document for details.
*/

#include <iostream>
#include <math.h>
#include <cmath>
#include <fstream>
#include <cstdlib>
#include <windows.h>
#include <chrono>
#include <thread>
#include <stack>
#include <iomanip>


using namespace std;

const double LIGHT_SPEED = 300000000;
const double EXHAUST_VELOCITY = 30000000; //exhaust velocity for the fuel in m/s
const double METERS_PER_LIGHTYEAR = 9460800000000000;
const double SECONDS_PER_YEAR = 31536000;
const double RADIANS_PER_DEGREE = 3.14159/180;
const double SECONDS_PER_DAY = 86400;


/*==========================================================
						Destination Struct
============================================================
something something something
*/

struct Destination{
	
		string name;
		double distance; //distance to destination in light-years
		double density;
	
};


/*==========================================================
						Ship Struct
============================================================
something something something
*/

struct Spaceship{
	
		string shipType;
		double mass; //in kg
		double maxAccel; //in m/(s^2)
	
};

/*==========================================================
						Trip Class
============================================================
something something something
*/

class Trip{
	
	private:
	
		Spaceship ship;
		Destination goal;
		double currentPosition; //in light-years
	
		double launchSpeed; //in m/s
		double cruisingSpeed; //in m/s
		double currentSpeed = 0; //in m/s
	
		double targetTime; //in years
		double timeElapsed = 0; //in years
		double totalMass;  //in kg
		double hazardFuel = 0;
		int courseCorrections = 0;
		int dragZones = 0;
		
		stack<double> hazards;
		
		static double HAZARD_BASE;
		static double HAZARD_FALLOFF; //factor reduces the chance of new hazards after each time a hazard is generated
		
		static string progressScale;
		
		bool lowOnFuel = false; //flag to track when the craft doesn't have enough fuel remaining to decelerate.
		bool liveMode = true;
		
	public:
		Trip(){};
		
		//mutators
		void setShip(Spaceship shipIn);
		void setGoal(Destination goalIn);
		void setLaunchSpeed(double speed);
		void setMode(bool live);
		
		//accessors
		double getCurrentPosition();
		double getCruisingSpeed();
		double getLaunchSpeed();
		double getCurrentSpeed();
		double getTimeElapsed();
		double getTotalMass();
		double getDryMass();
		double getDistance();
		double getHazardFuel();
		int getCourseCorrections();
		int getDragZones();
		bool getMode();
		string getDestination();
		
		//similar to accessors, but for quantities that must be calcualted instead of stored simply in attributes
		double findFuelMultiplier(double deltaV); //calculates the amount of fuel (as a fraction of ship total mass) needed for a maneuver
		double findReserveFuel(double speed); //calculates how much extra fuel you have, beyond what is needed to decelerate.
		double findFuelMass();
		double findTime(double maxSpeed);
		double findTopSpeed(double tripTime);
		
		void addFuel(double velocity); //modifies totalMass, adding enough mass of fuel to achieve the passed change in velocity
		double burnFuel(double deltaV); //like addFuel, but in reverse
		void enterTopSpeed(double time);
		void populateHazards();
		void resolveHazard();
		void cruisingPhase(); //handles the cruising phase of the trip, including dealing with hazards
		void drawProgressBar(int length);
		void drawCruising(double time, double distance);
		void drawAcceleration(double deltaV, double fuelUsed);
		void accelerationPhase(bool accelerating);
		void addReserveFuel(double fuelPercent);
		
	
	
};

double Trip::HAZARD_BASE = 10;
double Trip::HAZARD_FALLOFF = 0.5;
string Trip::progressScale = "|0%               |20%              |40%              |60%              |80%              |100%";

/*==========================================================
						Function Prototypes
============================================================*/

void append(int& length, Spaceship* &list);
void append(int& length, Destination* &list);

void drawProgressBar(int length);
void drawPhaseInfo(string phaseName, double time, double distance, int length);

Spaceship customShip();
Destination customDestination();

/**********************************************************
						Main
***********************************************************/

int main(){
	
	//Constants
	const double LAUNCH_LIMIT = 0.00085 * LIGHT_SPEED; //max launch speed is 85% of light speed, but expressed in km/s (because that's how the user will input it
	const double TIME_LIMIT_MULT = 1.1; //multiplicative factor used to calculate an extreme lower bound for trip lengths, to make sure nobody goes faster than light
	
	//Menu Control Variables
	char fastestTrip = 'n';
	char keepRunning = 'n';
	bool liveMode = false;
	int menuSelect = 0, tripCount = 0;
	
	//destination variables
	Destination currentDestination;
	Destination *starChart;
	int chartSize = 3; //tracks how many destinations are in the star chart
	starChart = new Destination[chartSize];
	
	//Preset Destinations [NUMBER SHOULD MATCH INITIAL STAR CHART SIZE]
	starChart[0] = {"Wolf 359", 8, 1};
	starChart[1] = {"Algol", 90, 0.3};
	starChart[2] = {"Helix Nebula", 650, 0.4};
	
	//Ship Variables
	Spaceship currentShip;
	int rosterSize = 3; //tracks how many ships are on the roster
	Spaceship *shipRoster; 
	shipRoster = new Spaceship[rosterSize];
	
	//Preset Ship Classes [NUMER SHOULD MATCH INITIAL ROSTER SIZE]
	shipRoster[0] = {"Rubin Class Exploration Vessel", 30000000, 20};
	shipRoster[1] = {"Underhill Class Uncrewed Probe", 10000000, 80};
	shipRoster[2] = {"Chandrasekhar Class Colony Ship", 600000000, 10};
	
	//Launch variables and presets
	string launchOptions[] = {"Cold Launch (0 km/s)", "Linear Accelerator (2000 km/s)", "Laser-Pushed Light Sail (10,000 km/s)" , "Custom Launch"}; //'Custom launch' should always be the last entry
	double launchSpeeds[] = {0, 2000000, 10000000, 0}; //launch speeds stored in meters per second
	const int NUM_LAUNCH_OPTS = 4;
	double launchSpeedIn = 0;
	double launchMass = 0;
	
	double reserveFuel[] = {20, 50, 100};
	int reserveFuelChoices = 4; //last choice is custom
	double reserveFuelPercent;
	
	Trip currentTrip;
	double targetTripTime = 0;
	
	
	ofstream outputFile("report.txt");
	
	cout << "Welcome to the interstellar travel simulator!" << endl << endl;
	cout << "This simulator will allow you to plan and simulate flights to nearby star systems and" << endl;
	cout << "other destinations of interest. You will be able to select from various preprogrammed ships" << endl;
	cout << "or enter specifications for your own custom craft." << endl << endl;
	
	srand((unsigned)time(NULL));
	
	
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
									Main Control Loop                                      */
	
	do{
		
		currentTrip = Trip(); //resets currentTrip to clear old data
		tripCount++; //each run through this loop is one trip
		outputFile << "Summary of Trip " << tripCount << ":" << endl << endl;
	
		cout << "First, choose whether you want this flight to be in Test Mode or Live Mode." << endl;
		cout << "In test mode, navigational hazards you encounter will not use any fuel. In live mode they will, " << endl;
		cout << "and you will need to bring reserve fuel in order to deal with them." << endl;
	
	
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
							Mode Selection                            */ 
		do{
		cout << "Enter 1 for Test Mode or 2 for Live Mode:" << endl;
		cin >> menuSelect;
		}while(menuSelect != 1 && menuSelect != 2);
		
		if(menuSelect == 1){
		currentTrip.setMode(false); //turns live mode off
		outputFile << "Test Mode" << endl << endl;
		}
		if(menuSelect == 2){
		currentTrip.setMode(true); //turns live mode on
		outputFile << "Live Mode" << endl << endl;
		}
		
		cout << endl << endl;
		
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
								Destination Selection 							*/
		
		cout << "Now please select your destination. Where do you want to go?" << endl;
		
		//print out the names of the entries in the star chart
		for(int i = 0; i < chartSize; i++){
			cout << i + 1 << ". " << starChart[i].name << "  [Distance: " << starChart[i].distance << " light-years ";
			cout << "Density: " << starChart[i].density << "]" << endl;
		}
		cout << chartSize + 1 << ". Custom Destination " << endl;
		
		do{ //menu selection for destination
			cin >> menuSelect;
			if(menuSelect < 0 || menuSelect > chartSize + 1) cout << "Error, Invalid Selection! Please try again:" << endl;
		
		} while(menuSelect < 0 || menuSelect > chartSize + 1);
		
		if(menuSelect == chartSize + 1){
			append(chartSize, starChart);
			starChart[chartSize -1] = customDestination();
			
		}
		
		cout << endl << endl;
		
		//record destination data in output file
		outputFile << "Desination Info: " << endl;
		outputFile << "Name: " << starChart[menuSelect -1].name << endl;
		outputFile << "Distance: " << starChart[menuSelect - 1].distance << " light-years" << endl;
		outputFile << "Density: " << starChart[menuSelect -1].density << endl << endl;
		
		currentTrip.setGoal(starChart[menuSelect - 1]); //store the destination info to currentTrip
		
		
		
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
									Ship Selection										*/
		
		cout << "Now please select your ship." << endl;
		
		//print out the names of the entries in the ship roster
		for(int i = 0; i < rosterSize; i++){
			cout << i + 1 << ". " << shipRoster[i].shipType << "  [Mass: " << shipRoster[i].mass/1000 << " metric tons / ";
			cout << "Maximum Acceleration: " << shipRoster[i].maxAccel/10 << "g]" << endl;
		}
		cout << rosterSize + 1 << ". Custom Ship " << endl;
		
		do{ //menu selection for ship
			cin >> menuSelect;
			if(menuSelect < 0 || menuSelect > rosterSize + 1) cout << "Error, Invalid Selection! Please try again:" << endl;
		} while(menuSelect < 0 || menuSelect > rosterSize + 1);
		
		if(menuSelect == rosterSize + 1){
			append(rosterSize, shipRoster);
			shipRoster[rosterSize - 1] = customShip();
		} 
		
		cout << endl << endl;
		
		//record ship info in output file
		outputFile << "Ship Info: " << endl;
		outputFile << "Type: " << shipRoster[menuSelect - 1].shipType << endl;
		outputFile << "Mass: " << shipRoster[menuSelect - 1].mass/1000 << " metric tons" << endl;
		outputFile << "Maximum Acceleration: " << shipRoster[menuSelect -1].maxAccel/10 << "g" << endl <<endl;
		
		currentTrip.setShip(shipRoster[menuSelect - 1]); //Store the ship selection to currentTrip
		
		
		
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
									Launch Selection									 */
		
		cout << "Next select your launch option. This will determine how fast your ship is going when it launches." << endl;
		
		//print out the names of the launch options
		for(int i = 0; i < NUM_LAUNCH_OPTS; i++){
			cout << i + 1 << ". " << launchOptions[i] << endl;
		}
		
		do{ //menu selection for launch
			cin >> menuSelect;
			if(menuSelect < 0 || menuSelect > NUM_LAUNCH_OPTS) cout << "Error, Invalid Selection! Please try again:" << endl;
		} while(menuSelect < 0 || menuSelect > NUM_LAUNCH_OPTS);
		
		if(menuSelect == NUM_LAUNCH_OPTS){ //allow the user to enter a custom launch speed
		
			do{ //make sure the custom launch speed is
			cout << "Please enter the speed (in kilometers per second) for your custom launch option: ";
			cin >> launchSpeedIn;
			if(launchSpeedIn > LAUNCH_LIMIT) cout << "Sorry, can't launch faster than 255,000 km/s." << endl;
			if(launchSpeedIn < 0) cout << "Sorry, can't accept a negative launch speed." << endl;
			}while(launchSpeedIn > LAUNCH_LIMIT || launchSpeedIn < 0);
			
			launchSpeeds[menuSelect - 1] = 1000 *launchSpeedIn; //convert the input to m/s before storing
		}
		
		outputFile << "Trip Info: " << endl;
		outputFile << "Launch Option: " << launchOptions[menuSelect - 1] << " [" << launchSpeeds[menuSelect - 1] / 1000 << " km/s]" << endl;
		
		currentTrip.setLaunchSpeed(launchSpeeds[menuSelect - 1]); //Store the launch speed to currentTrip
		
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
									Reserve Fuel Selection						 		*/
									
		if(currentTrip.getMode()){
			
			cout << "Finally, choose how much reserve fuel you want to carry. Reserve fuel will allow you to deal with" << endl;
			cout << "hazards, but it will proportionally increase your fuel consumption for accelerating and deccelerating." << endl;
			cout << "For example, if you carry enough reserve fuel to double your ship's starting mass, you will also double" << endl;
			cout << "the amount of primary fuel you need to carry." << endl;
			cout << "Choose an amount of reserve fuel (as a percentage of your ship's starting mass): " << endl;
			
			for(int i = 0; i < reserveFuelChoices - 1; i++){
				
				cout << i + 1 << ". " << reserveFuel[i] << "%" << endl;
				
			}
			
			cout << "4. Custom" << endl;
			
			do{
				cout << "Choose fuel amount: ";
				cin >> menuSelect;
			}while(menuSelect < 0 || menuSelect > reserveFuelChoices);
			
			if(menuSelect > 0 && menuSelect < 4) reserveFuelPercent = reserveFuel[menuSelect - 1];
			else if(menuSelect == 4){
				do{
					cout << "Enter a fuel amount: ";
					cin >> reserveFuelPercent;
					if(reserveFuelPercent < 0) cout << "Error, fuel amount can't be negative!" << endl;	
				}while(reserveFuelPercent < 0);
			}
			
			currentTrip.addReserveFuel(reserveFuelPercent);
			
			cout << "Reserve fuel added! Your ship now masses " << currentTrip.getTotalMass() << " before adding primary fuel." << endl;
			
			outputFile << "Reserve Fuel Percentage: " << reserveFuelPercent << "%" << endl << endl;
			
		}
		
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
									Speed Selection										*/
		
		cout << "Now let's decide how fast you want to get there." << endl;
		cout << "You're travelling to " << currentTrip.getDestination() << " which is " << currentTrip.getDistance() << " light years away." << endl;
		cout << "How many long do you want this trip to take, at most? Keep in mind faster trips use more fuel. " << endl;
		
		do{ //while loop makes sure a valid trip time is entered
			cout << "Enter your trip time in years: ";
			cin >> targetTripTime;
			if(targetTripTime < currentTrip.getDistance() * TIME_LIMIT_MULT){
				cout << "I'm sorry, it's not possible to make the trip that fast." << endl;
				cout << "The shortest possible time for this trip is " << currentTrip.getDistance() * TIME_LIMIT_MULT << "," << endl;
				cout << "Would you like to make that your trip time instead? (y/n)?";
				cin >> fastestTrip;
				if(fastestTrip == 'y') targetTripTime = currentTrip.getDistance() * TIME_LIMIT_MULT;
			}
			
		}while(targetTripTime < currentTrip.getDistance() * TIME_LIMIT_MULT);
		
		outputFile << "Target Trip Time: " << targetTripTime << " years" << endl;
		
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
									Calculate Trip Parameters							*/
									
		currentTrip.enterTopSpeed(targetTripTime);
		//update targetTime
		currentTrip.addFuel(currentTrip.getCruisingSpeed()); //add fuel for slowing down
		currentTrip.addFuel(currentTrip.getCruisingSpeed() - currentTrip.getLaunchSpeed()); //then add fuel for speeding up
		launchMass = currentTrip.getTotalMass();
		
		cout << "To reach your destination in the requested time, your ship will need to accelerate to a top speed of ";
		cout << currentTrip.getCruisingSpeed()/1000 << " km/s." << endl;
		cout << "To reach this speed (and slow down again) you will need to carry a fuel mass of ";
		cout << currentTrip.findFuelMass()/1000 << " metric tons." << endl;
		
	
		outputFile << "Cruising Speed: " << currentTrip.getCruisingSpeed()/1000 << " km/s" << endl;
		outputFile << "Launch Mass: " << launchMass/1000 << " metric tons" << endl << endl;
		
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
									Launch Animation	                         */		
															
		cout << endl << "Your ship is prepped and ready to launch." << endl;
		
		system("pause");
		
		for(int i = 5; i > 0; i--){
			cout << i;
			std::this_thread::sleep_for(std::chrono::milliseconds(250));
			cout<< " .";
			std::this_thread::sleep_for(std::chrono::milliseconds(250));
			cout << " .";
			std::this_thread::sleep_for(std::chrono::milliseconds(250));
			cout << " . ";
			std::this_thread::sleep_for(std::chrono::milliseconds(250));	
		}
		
		
		cout << endl << endl << "LAUNCH!" << endl << endl;
		
		currentTrip.accelerationPhase(true);
		currentTrip.cruisingPhase();
		currentTrip.accelerationPhase(false);
		
		outputFile << "Trip Results: " << endl;
		outputFile << "Total Trip Time: " << currentTrip.getTimeElapsed() << " years" << endl;
		outputFile << "Total Fuel Used: " << (launchMass - currentTrip.getTotalMass()) << " metric tons" << endl;
		outputFile << "Course Corrections Required: " << currentTrip.getCourseCorrections() << endl;
		outputFile << "Drag Zones Encountered: " << currentTrip.getDragZones() << endl;
		outputFile << "Fuel Used Dealing With Hazards: " << currentTrip.getHazardFuel()/1000 << " metric tons" << endl;
		outputFile << "Trip Successful? ";
		if(currentTrip.getCurrentSpeed() == 0){
			outputFile << "Yes." << endl;
			outputFile << "Fuel Remaining: " << currentTrip.findFuelMass()/1000 << " metric tons";
		}else outputFile << "No.";
		outputFile << endl << endl << endl;
		
		cout << "Would you like to launch another mission (y/n)?";
		cin >> keepRunning;
		
	}while(keepRunning == 'y');
	
	return 0;
}

/***********************************************************
					Function Definitions
************************************************************/


/*==========================================================
						customShip() function
============================================================
Gets input from the user for the specs for a custom Spaceship.
*/

Spaceship customShip(){
	
	Spaceship tempShip;
	string tempName = "";
	double tempNum = 0;
	char accept = 'y';
	
	cout << "Please enter a name for this type of ship: ";
	getline(std::cin, tempName);
	getline(std::cin, tempShip.shipType);
	
	do{ //get mass from user and validate
		cout << "Please enter a mass for the ship in metric tons: ";
		cin >> tempNum;
		if(tempNum <= 0) cout << "Error, the ship must have a positive mass!" << endl;
	}while(tempNum <= 0);
	
	tempShip.mass = tempNum * 1000; //convert the entry to kg before storing in tempShip
	
	do{
		accept = 'y'; //reset accept so the user doesn't get stuck in the loop
		cout << "Please enter a maximum acceleration for the ship in units of g: ";
		cin >> tempNum;
		
		if(tempNum <= 0) cout << "Error, the acceleration must be positive!" << endl;
		
		if(tempNum > 10){
			cout << "Warning! Anything above 10g puts all parts of the ship under extreme stress. It must be uncrewed and designed to withstand such forces."<< endl;
			cout << "Do you wish to proceed anyway (y/n)?";
			cin >> accept;
		}
		else if(tempNum > 2){
			cout << "Warning! Humans crews cannot survive sustained acceleration above 2g. Your ship must be uncrewed." << endl;
			cout << "Do you wish to proceed anyway (y/n)?";
			cin >> accept;
		}
		
	}while(accept != 'y' && tempNum <= 0);
	
	tempShip.maxAccel = tempNum * 10; //convert from gs to m/(s^2) before storing
	
	return tempShip;

}

/*==========================================================
						customDestination() function
============================================================
Gets input from the user for the specs for a custom Destination.
*/

Destination customDestination(){
	
	Destination tempPlace;
	string tempName = "";
	double tempNum = 0;
	
	cout << "Please enter the name of your destination: ";
	getline(std::cin, tempName);
	getline(std::cin, tempPlace.name);
	
	do{
		cout << "Please enter the distance to your destination in light-years: ";
		cin >> tempNum;
		if(tempNum <= 0) cout << "Error! Your distance must be positive." << endl;	
	}while(tempNum <= 0);
	
	tempPlace.distance = tempNum;
	tempNum = 1;
	
	cout << "Next you will enter the density parameter for your destination. This determines how likely" << endl;
	cout << "you are to run into hazards. The base chance is (on average) 10% per light-year travelled." << endl;
	cout << "The density parameter will multiply this chance: if it's greater than 1 you'll run into more" << endl;
	cout << "hazards. If it's less than 1 you'll run into fewer." << endl;
	
	do{
		cout << "Please enter the density parameter for your destination: ";
		cin >> tempNum;
		if(tempNum < 0) cout << "Error! Your density cannot be negative." << endl;	
	}while(tempNum < 0);
	
	tempPlace.density = tempNum;
	
	return tempPlace;
	
}


/*==========================================================
						appendArray() functions
============================================================
Increases the length of an array by one.

User must pass in a pointer to the array and the variable that tracks the length. Dynamically allocates
a new array of length one greater, copies all the values from the passed orray to the new array, then deletes
the passed array and re-assignes its pointer to the address of the new array. Increments the length variable (by reference)
to reflect the new size.

Function is overloaded so there is one version for each relevant type of array, Ship, Trip and Destination
*/

void append(int& length, Spaceship* &list){
	
	Spaceship *temp = new Spaceship[length + 1]; //allocate new memory
	
	for(int i = 0; i < length; i++){ //copy values to new array
		temp[i] = list[i];
	}
	
	delete [] list; //free up memory from old array
	list = temp; 
	length++;
	
}

void append(int& length, Destination* &list){
	
	Destination *temp = new Destination[length + 1];
	
	for(int i = 0; i < length; i++){
		temp[i] = list[i];
	}
	
	delete [] list;
	list = temp;
	length++;
	
}


/***********************************************************
************************************************************
					Trip Method Definitions
************************************************************
***********************************************************/

/*==========================================================
						drawCruising()
============================================================
Draws the display bar for the cruising phase of the trip.
Takes in the remaining time and distance and calculates the
spacing needed for everything to fit evenly on the bar.
*/

void Trip::drawCruising(double time, double distance){
	
	string displayString = "|Cruising Phase";
	int textLength = displayString.length(); //tracks the length of everything that must be written to the screen
	
	string distanceString = "Distance Remaining: ";
	string timeString = "Time Remaining: "; 
	
	string yearUnit = " years";
	string distanceUnit = " light-years";
	
	int timeYears, timeDeciYears, distanceLyrs, distanceDeciLyrs, spaceCounter, spacesRemaining;
	
	//convert the distance to readable form, then append to distanceString along with unit
	distanceLyrs = floor(distance);
	distanceString += std::to_string(distanceLyrs);
	distanceString += ".";
	distanceDeciLyrs = round(((distance - (double)distanceLyrs) * 10));
	distanceString += std::to_string(distanceDeciLyrs);
	distanceString += distanceUnit;
	textLength += distanceString.length();
	
	//convert the time to readable form, then append to timeString along with unit
	timeYears = floor(time);
	timeString += std::to_string(timeYears);
	timeString += ".";
	timeDeciYears = round(((time - (double)timeYears) * 10));
	timeString += std::to_string(timeDeciYears);
	timeString += yearUnit;
	textLength += timeString.length();	
	
	spacesRemaining = progressScale.length() - textLength;
	
	if(spacesRemaining < 4){
		displayString = displayString + " " + distanceString + " " + timeString + "|";
	}
	else{
		spaceCounter = spacesRemaining/2;
		for(int i = 0; i < spaceCounter; i++){
			displayString += " ";
			spacesRemaining--;
		}
		
		displayString += distanceString;
		spaceCounter = spacesRemaining;
		
		for(int i = 0; i < spaceCounter; i++){
			displayString += " ";
			spacesRemaining--;
		}
		
		displayString += timeString;
		displayString += "|";
		
	}

		
	cout << displayString << endl;		
	
	
}



/*==========================================================
						drawAcceleration()
============================================================
Draws the display bar for the cruising phase of the trip.
Takes in the change in velocity INCLUDING THE SIGN: positive
if accelerating, negative if decellerating. Calculates
the spacing needed for everything to fit evenly on the bar.
*/
void Trip::drawAcceleration(double signedDeltaV, double fuelUsed){
	
	string displayString = "";
	
	if(signedDeltaV >= 0){
		displayString += "|Acceleration Phase";
	}else{
		displayString += "|Decceleration Phase";
	}
	
	int textLength = displayString.length();

	string timeString = "Duration: "; 
	string massString = "Fuel Used: ";
	
	string timeUnit = " days";
	string massUnit = " metric tons";
	
	int timeDays, spacesRemaining, spaceCounter;
	int massTons = round((fuelUsed)/1000); //convert fuel used to tons and round to integer

	timeDays = round(((abs(signedDeltaV)/ship.maxAccel)/SECONDS_PER_DAY));
	timeString += std::to_string(timeDays);
	timeString += timeUnit;
	textLength += timeString.length();
	
	massString += std::to_string(massTons);
	massString += massUnit;
	textLength += massString.length();

	
	spacesRemaining = progressScale.length() - textLength;
	
	if(spacesRemaining < 4){
		displayString = displayString + " " + timeString + " " + massString + "|";
	}
	else{
		spaceCounter = spacesRemaining/2;
		for(int i = 0; i < spaceCounter; i++){
			displayString += " ";
			spacesRemaining--;
		}
		
		displayString += timeString;
		spaceCounter = spacesRemaining;
		
		for(int i = 0; i < spaceCounter; i++){
			displayString += " ";
			spacesRemaining--;
		}
		
		displayString += massString;
		displayString += "|";
		
	}

		
	cout << displayString << endl;
	cout << progressScale << endl;
	drawProgressBar(progressScale.length() - 5);	
	
}

/*========================================================
						populateHazards()
==========================================================
Randomly generates elements for the 'hazards' queue as a function
of density and distance travelled. */

void Trip::populateHazards(){
	
	int totalLyrs = floor(goal.distance);
	double hazardChance = HAZARD_BASE * goal.density;
	double hazardRoll;
	
	for(int i = totalLyrs; i > 0; i--){
		hazardRoll =((double)(rand()%1000))/10; //generates a random number between 1 and 100 with one decimal place of precision
		
		/*roll is weighted so that higher distances (closer to the destination) are more likely to have hazards
		but the average chance per lightyear is still hazardChance*/
		if(hazardRoll < 2 * hazardChance * ((double)i)/((double)(totalLyrs + 1))){
			hazards.push((double)i); //if a hazard was generated, push the distance it occurs onto the stack
			hazardChance *= HAZARD_FALLOFF; //each hazard makes further hazards less likely
		}
		
	}
	
	
}

/*========================================================
						cruisingPhase()
==========================================================
Handles the cruising phase of the voyage, when the ship is neither accelerating nor decelerating */

void Trip::cruisingPhase(){
	
	double cruisingDistRem = goal.distance - currentPosition - (0.5*pow(currentSpeed, 2)/ship.maxAccel)/METERS_PER_LIGHTYEAR; //distance remaining before deceleration
	double cruisingTimeRem = (METERS_PER_LIGHTYEAR * cruisingDistRem/currentSpeed)/SECONDS_PER_YEAR;
	int nextStop, progressBarLength;
	double timeToHazard;
	
	populateHazards();
	
	cout << "Cruising Dist: " << cruisingDistRem << "  Cruising Time " << cruisingTimeRem << endl; 
	
	while(!hazards.empty()){
		//calculate where and when the next hazard will appear
		nextStop = (hazards.top() - currentPosition);
		timeToHazard = (METERS_PER_LIGHTYEAR * nextStop/currentSpeed)/SECONDS_PER_YEAR;
		
		//draw the usual display for cruising phase, but "interrupt" the progress bar before it's full to indicate the hazard has been encountered
		progressBarLength =((progressScale.length() - 5) * nextStop )/cruisingDistRem;
		drawCruising(cruisingTimeRem, cruisingDistRem);
		drawProgressBar(progressBarLength);
		
		//update distance and time variables as appropriate
		timeElapsed += timeToHazard;
		cruisingTimeRem -=timeToHazard;
		currentPosition += nextStop;
		cruisingDistRem -= nextStop;
		
		//resolve hazard, then remove it from the stack
		resolveHazard();		
		hazards.pop();
	}
	
	//when all hazards have been resolved, do one final pass to finish out the cruising phase
	
	progressBarLength = progressScale.length() - 5;
	timeElapsed += cruisingTimeRem;
	currentPosition += cruisingDistRem;
	drawCruising(cruisingTimeRem, cruisingDistRem);
	cout << progressScale << endl;
	drawProgressBar(progressScale.length() - 5);
	
	
}


/*========================================================
						resolveHazard
==========================================================
Resolves the hazard at the top of the stack. If the stack is empty, does nothing and returns false.
Otherwise returns true. */

void Trip::resolveHazard(){
	
	double correctionSize;
	double fuelUsedMult;
	double fuelUsedTotal;
	double reserveFuel;
	double startSpeed = currentSpeed;
	double speedReduction;
	
	int hazardType = rand()%2; //randomly chooses hazard type. 0 is a course correction, 1 is a drag zone
	
	if(hazardType == 0){ //code for course correction
	
		courseCorrections++;
		
		correctionSize = ((double)((rand()%15) + 5)) /10; //course correction randomly generated as being between 0.5 degrees and 2 degrees.
		fuelUsedMult = findFuelMultiplier(-cruisingSpeed * correctionSize * RADIANS_PER_DEGREE); //negative in exponent should mean it comes out to less than one
		fuelUsedTotal = totalMass*(1 - fuelUsedMult); //fuel used is the difference between mass before the correction and mass after. Convert to metric tons for output
		if(liveMode) totalMass *= fuelUsedMult; //multiply the total mass by the multiplier to "remove" the used fuel
		
		cout << "You encountered a hazard at " << currentPosition << " light-years. You needed to make a course correction of " << correctionSize << " degrees." << endl;
		
		reserveFuel = findReserveFuel(currentSpeed);
		hazardFuel += fuelUsedTotal;
		
		if(!liveMode) cout << "In a live run, this maneuver would have used at least " << fuelUsedTotal/1000 << " metric tons of fuel." << endl;
		else if(reserveFuel > 0) cout << "This maneuver used up " << fuelUsedTotal/1000 << " metric tons of fuel, leaving you with " << reserveFuel/1000 << " metric tons of reserve fuel." << endl;
		else if(reserveFuel < 0){
			cout << "This maneuver used up " << fuelUsedTotal/1000 << " metric tons of fuel, depleting your reserves and leaving you too little fuel to decelerate." << endl;
			cout << "You'll miss your destination unless you slow down in some other fashion." << endl;
		}else cout << "This maneuver used up " << fuelUsedTotal/1000 << " metric tons of fuel, which was the very last of your reserve fuel" << endl;
		
		
		
		
		
	}
	
	if(hazardType == 1){ //code for drag zone
	
		dragZones++;
		
		speedReduction = ((double)((rand()%5) + 5))/100; //slows the ship by 5-10%
		currentSpeed *= (1 - speedReduction);
			
		
		if( (totalMass/findFuelMultiplier(cruisingSpeed - currentSpeed)) /findFuelMultiplier(cruisingSpeed) < ship.mass && liveMode){
			cout << "You hit a drag zone which slowed you by " << ((startSpeed - currentSpeed)/1000) << " km/s." << endl;
			cout << "You can't afford the fuel to speed back up again, you'll just have to continue at this speed." << endl;
		}else{
			
			cout << "You hit a drag zone which slowed you by " << ((startSpeed - currentSpeed)/1000) << " km/s." << endl;
			fuelUsedTotal = burnFuel(cruisingSpeed - currentSpeed);
			currentSpeed = cruisingSpeed;
			hazardFuel += fuelUsedTotal;
			if(liveMode){
				cout << "Speeding back up cost you " << fuelUsedTotal/1000 << " metric tons of fuel, leaving you with " << findReserveFuel(currentSpeed) << " metric tons of reserve fuel remaining." << endl;
			}else{
				cout << "In a live run, speeding back up would have used at least " << fuelUsedTotal/1000 << " metric tons of fuel." << endl;
				totalMass += fuelUsedTotal; //replace the fuel removed by burnFuel function
				
			}
		}
		
		
		
	}
	
	cout << endl;
	system("pause");
	
}

/*==========================================================
						accelerationPhase()
============================================================
*/

void Trip::accelerationPhase(bool accelerating){
	
	double deltaV, deltaT, distTravelled, fuelUsed, sign = 1;
	
	if(!accelerating) sign = -1;
	
	if(accelerating) deltaV = cruisingSpeed - currentSpeed; //always accelearate up to cruising speed
	else deltaV = currentSpeed; //always decelerate down to zero
	
	fuelUsed = burnFuel(deltaV);
	
	if(findFuelMass() < 0){ //if trying to deccelerate results in a negative amount of fuel
		cout << "Oh no! You used up too much fuel and don't have enough left to decelerate!" << endl;
		cout << "Your ship will bypass its destination and keep flying forever. Your mission is a failure! " << endl;
		return;
	}
		
	deltaT = deltaV/ship.maxAccel; //acceleartion time IN SECONDS
	distTravelled = currentSpeed * deltaT + sign * 0.5 * ship.maxAccel * pow(deltaT, 2); //distance travelled IN METERS
	
	if(accelerating){
		currentSpeed = cruisingSpeed;}
	else {
		currentSpeed = 0;}
		
	currentPosition += distTravelled/METERS_PER_LIGHTYEAR;
	timeElapsed += deltaT/SECONDS_PER_YEAR;
	
	drawAcceleration(sign * deltaV, fuelUsed);
}

/*==========================================================
						drawProgressBar()
============================================================
Draws a progress bar of the specified length, with short pauses in between 
drawing elements. Intended to be displayed one line under the 'progressScale'
string. For a full progress bar, the length should be set to (progressScale.length() -5)
*/

void Trip::drawProgressBar(int length){
	
	cout < " ";
	
	for(int i = 0; i < length; i++){
		std::this_thread::sleep_for(std::chrono::milliseconds(65));
		cout << "=";
	}
	
	cout << endl << endl;
}

/*========================================================
						addFuel()
==========================================================
Adjusts the total mass of the spacecraft by the amount determined by findFuelMultiplier
Takes in velocity in m/s. */

void Trip::addFuel(double velocity){
	
	totalMass *= findFuelMultiplier(velocity);
	
}

/*========================================================
						addReserveFuel()
==========================================================
Adds reserve fuel as a percentage of the current mass */

void Trip::addReserveFuel(double fuelPercent){
	
	totalMass *= (1 + fuelPercent/100);
	
}

/*========================================================
						burnFuel()
==========================================================
Adjusts the total mass of the spacecraft by the amount determined by findFuelMultiplier
Takes in velocity in m/s. Returns the amount of fuel burned in kg*/

double Trip::burnFuel(double deltaV){
	
	double initMass = totalMass;
	totalMass = totalMass/findFuelMultiplier(deltaV);
	return initMass - totalMass; //amount of fuel burned is the difference in totalMass before and after the burn	
	
}


/*========================================================
						enterTopSpeed()
==========================================================
Sets cruisingSpeed to the value calculated by findTopSpeed
takes tripTime in years                                */

void Trip::enterTopSpeed(double time){
	
	cruisingSpeed = findTopSpeed(time);
	
}

/*========================================================
						findTopSpeed()
==========================================================
Calculates the maximum speed the ship will have to reach in order to get to its destination in the given time.
Takes in tripTime in years, returns speed in m/s   */

double Trip::findTopSpeed(double tripTime){
	
	return (ship.maxAccel * (tripTime * SECONDS_PER_YEAR )- sqrt(pow(ship.maxAccel * tripTime * SECONDS_PER_YEAR, 2) - 4 * ship.maxAccel * goal.distance*METERS_PER_LIGHTYEAR))/2;
};


/*========================================================
						findReserveFuel()
==========================================================
Used in hazard resolution to determine how much reserve fuel is remaining
after dealing with a particular hazard. If it returns a negative value, it means
the ship no longer has enough fuel left to decelerate at the end of the trip */

double Trip::findReserveFuel(double speed){
	
	double decelerationFuel;
	double remainingFuel;
	
	decelerationFuel = totalMass/findFuelMultiplier(speed) - ship.mass;
	remainingFuel = findFuelMass() - decelerationFuel;
	
	return remainingFuel;
}


/*========================================================
						findFuelMultiplier
==========================================================
Calculates how much to multiply the mass of the ship to account for the fuel for a given maneuver. For example, 
if the total mass of the ship (including all other fuel) was 1000 kg, and getFuelMultiplier returned a value of
1.5 for a given maneuver, the total ship mass would need to be 1500 kg to perform that manever (that is, it would
need 500 kg of additional fuel).
takes in deltaV in m/s. Returns a unitless multiplier*/

double Trip::findFuelMultiplier(double deltaV){
	
	return exp(deltaV/EXHAUST_VELOCITY);
	
};

/*========================================================
						findFuelMass()
==========================================================
Returns the mass of the fuel (the total mass minue the dry mass) in kg*/

double Trip::findFuelMass(){
	
	return totalMass - ship.mass;
	
}

/*========================================================
						findTime()
==========================================================
Finds the time the trip will take at a given maximum speed
Takes maxSpeed in m/s, returns time in years*/

double Trip::findTime(double maxSpeed){
	
	return ((goal.distance * METERS_PER_LIGHTYEAR)/maxSpeed + maxSpeed/ship.maxAccel)/SECONDS_PER_YEAR;
	
}

/*========================================================
						findRemDistance()
==========================================================
calculates the distance remaining to the goal           */

double Trip::getCurrentPosition(){
	return goal.distance - currentPosition;
}


/*========================================================
						setLaunchSpeed()
==========================================================
sets the launch speed and the current speed of the trip to the
passed value*/

void Trip::setLaunchSpeed(double speed){
	
	launchSpeed = speed;
	currentSpeed = speed;
	
};

/*========================================================
						setShip()
==========================================================
sets ship to a specific instance of Spaceship struct */

void Trip::setShip(Spaceship shipIn){
	
	ship = shipIn;
	totalMass = shipIn.mass;
	
}

/*========================================================
						setGoal()
==========================================================
sets goal to a specific instance of the Destination struct
and updates remainingDistance to the total distance to that destination*/

void Trip::setGoal(Destination goalIn){
	
	goal = goalIn;
	
}

/*========================================================
						setMode()
==========================================================
sets the value of liveMode attribute */

void Trip::setMode(bool live){
	
	liveMode = live;
}


/*========================================================
						getCruisingSpeed
==========================================================
accessor for cruisingSpeed attribute
cruisingSpeed should be in m/s                           */

double Trip::getCruisingSpeed(){
	return cruisingSpeed;
}

/*========================================================
						getLauncSpeed
==========================================================
accessor for launchSpeed attribute                     
launchSpeed should be in m/s                           */

double Trip::getLaunchSpeed(){
	return launchSpeed;
}

/*========================================================
						getCurrentSpeed
==========================================================
accessor for currentSpeed attribute
currentSpeed should be in m/s                           */

double Trip::getCurrentSpeed(){
	return currentSpeed;
}

/*========================================================
						getTimeElapsed
==========================================================
accessor for timeElapsed attribute
timeElapsed should be in years                            */

double Trip::getTimeElapsed(){
	return timeElapsed;
}



/*========================================================
						getTotalMass
==========================================================
accessor for totalMass attribute
totalMass should be in kg                               */

double Trip::getTotalMass(){
	return totalMass;
}

/*========================================================
						getDryMass
==========================================================
accessor for mass variable of the ship attribute
ship.mass should be in kg     */

double Trip::getDryMass(){
	return ship.mass;
}

/*========================================================
						getDestination()
==========================================================
accessor for name variable of goal attribute*/

string Trip::getDestination(){
	return goal.name;
}

/*========================================================
						getMode()
==========================================================
accessor for name variable of liveMode attribute*/

bool Trip::getMode(){
	
	return liveMode;
}

/*========================================================
						getDistance
==========================================================
accessor for name variable of distance member of goal struct*/

double Trip::getDistance(){
	return goal.distance;
}

/*========================================================
						getHazardFuel
==========================================================
accessor for hazardFuel attribute*/

double Trip::getHazardFuel(){
	return hazardFuel;
}

/*========================================================
						getCourseCorrections
==========================================================
accessor courseCorrections Attribute*/

int Trip::getCourseCorrections(){
	return courseCorrections;
}

/*========================================================
						getDragZones
==========================================================
accessor for dragZones attribute*/

int Trip::getDragZones(){
	return dragZones;
}
