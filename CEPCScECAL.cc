//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id: CEPCScECAL.cc  2015-09-10 19:07:37  $
// 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
//#else
#include "G4RunManager.hh"
//#endif

#include "G4UImanager.hh"
#include "Randomize.hh"

#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
//#include "ActionInitialization.hh"
#include "SteppingVerbose.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"
#include "PrimaryGeneratorAction.hh"
#include "TrackingAction.hh"
#include "QGSP_BERT.hh"

//#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
//#endif	

//#ifdef G4UI_USE
#include "G4UIExecutive.hh"
//#endif

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

int main(int argc,char** argv) {
 
  	//choose the Random engine
  	CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);

  	G4long seed = time(0)+getpid();
  	CLHEP::HepRandom::setTheSeed(seed);
  	CLHEP::HepRandom::showEngineStatus();
  
  	// Construct the default run manager
	//#ifdef G4MULTITHREADED
	//  G4MTRunManager* runManager = new G4MTRunManager;
	//  runManager->SetNumberOfThreads(G4Threading::G4GetNumberOfCores());
	//#else
  	//my Verbose output class
  	G4VSteppingVerbose::SetInstance(new SteppingVerbose);
  	G4RunManager* runManager = new G4RunManager;
	//#endif  

  	// set mandatory initialization classes
  	//
  	DetectorConstruction* detector = new DetectorConstruction;
  	runManager->SetUserInitialization(detector);
	
	// PhysicsList* physics = new PhysicsList;
  	G4VUserPhysicsList* physics = new QGSP_BERT();
  	runManager->SetUserInitialization(physics);
  	
	// set user action classes
  	// 
  	/*
  	ActionInitialization* actionInitialization = new ActionInitialization(detector);
  	runManager->SetUserInitialization(actionInitialization);  
  	*/
  	HistoManager* histo = new HistoManager(argv[2]);
  	//SteppingVerbose* stepV = new SteppingVerbose();

  	PrimaryGeneratorAction* primary = new PrimaryGeneratorAction(detector, histo);
  	runManager->SetUserAction(primary);
  	RunAction* runAction = new RunAction(primary,histo);
  	runManager->SetUserAction(runAction);
  	EventAction* eventAction = new EventAction(histo);
  	runManager->SetUserAction(eventAction);
  	TrackingAction* trackingAction = new TrackingAction(runAction,eventAction);
  	runManager->SetUserAction(trackingAction);
  	SteppingAction* steppingAction = new SteppingAction(detector,eventAction);
  	runManager->SetUserAction(steppingAction);
  
  	//Initialize G4 kernel
  	runManager->Initialize();
    
  	// get the pointer to the User Interface manager 
  	G4UImanager* UI = G4UImanager::GetUIpointer();  

	//#ifdef G4VIS_USE
  	G4VisManager* visManager = new G4VisExecutive;
  	visManager->Initialize();
	//#endif

  	if (argc!=1) { // batch mode 
     	G4String command = "/control/execute ";
     	G4String fileName = argv[1];
     	UI->ApplyCommand(command+fileName);  
    } else {           // define visualization and UI terminal for interactive mode 
		//#ifdef G4UI_USE
     	G4UIExecutive * ui = new G4UIExecutive(argc,argv);      
		//#ifdef G4VIS_USE
     	UI->ApplyCommand("/control/execute vis.mac");          
		//#endif
     	ui->SessionStart();
     	delete ui;
		//#endif
    }

	//#ifdef G4VIS_USE
  	delete visManager;
	//#endif     

  	// job termination
  	delete runManager;
  	return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo..... 

