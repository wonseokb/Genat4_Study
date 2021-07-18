#include "DetectorConstruction.hh"
#include "DetectorMessenger.hh"

#include "G4NistManager.hh"
#include "G4Material.hh"
#include "G4Element.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4OpticalSurface.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"

//=====================================================================================================================================================

DetectorConstruction::DetectorConstruction() : G4VUserDetectorConstruction(), fDetectorMessenger(nullptr)
{
  fExpHall_x = fExpHall_y = fExpHall_z = 10.0*m;
  fTank_x    = fTank_y    = fTank_z    =  5.0*m;

  //Tank, World, Surface
  //TankMaterial, WordlMaterial
  fTank = nullptr;//what?
  fTank_LV  = nullptr;//what?. LogicalVolume
  fWorld_LV = nullptr;//what?. LogicalVolume

  fTankMPT    = new G4MaterialPropertiesTable();
  fWorldMPT   = new G4MaterialPropertiesTable();
  fSurfaceMPT = new G4MaterialPropertiesTable();

  fTankMaterial  = G4NistManager::Instance()->FindOrBuildMaterial("G4_WATER");
  fWorldMaterial = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");
  fSurface = new G4OpticalSurface("Surface");
  fSurface->SetType(dielectric_dielectric);
  fSurface->SetFinish(ground);
  fSurface->SetModel(unified);
  fSurface->SetMaterialPropertiesTable(fSurfaceMPT);

  fDetectorMessenger = new DetectorMessenger(this);
}

DetectorConstruction::~DetectorConstruction()
{
  delete fDetectorMessenger;
}

//======================================================================================================================================================

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  fTankMaterial->SetMaterialPropertiesTable(fTankMPT);
  fTankMaterial->GetIonisation()->SetBirksConstant(0.126*mm/MeV);
  fWorldMaterial->SetMaterialPropertiesTable(fWorldMPT);

  // ------------- Volumes --------------------------------------------------------------------------------------------------------------------
  // The experimental Hall
  G4Box* world_box = new G4Box("World", fExpHall_x, fExpHall_y, fExpHall_z);
  fWorld_LV = new G4LogicalVolume(world_box, fWorldMaterial, "World", 0, 0, 0);
  G4VPhysicalVolume* world_PV = new G4PVPlacement(0, G4ThreeVector(), fWorld_LV, "World", 0, false, 0);//Physical Volume

  // The tank
  G4Box* tank_box = new G4Box("Tank", fTank_x, fTank_y, fTank_z);
  fTank_LV  = new G4LogicalVolume(tank_box, fTankMaterial, "Tank", 0, 0, 0);
  fTank     = new G4PVPlacement(0, G4ThreeVector(), fTank_LV, "Tank", fWorld_LV, false, 0);

  // ------------- Surface --------------------------------------------------------------------------------------------------------------------
  G4LogicalBorderSurface* surface = new G4LogicalBorderSurface("Surface", fTank, world_PV, fSurface);
  G4OpticalSurface* opticalSurface = dynamic_cast <G4OpticalSurface*> (surface->GetSurface(fTank,world_PV)->GetSurfaceProperty());//what?
  
  return world_PV;
}

//=======================================================================================================================================================

void DetectorConstruction::SetSurfaceSigmaAlpha(G4double v) {
  fSurface->SetSigmaAlpha(v);
  G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

void DetectorConstruction::SetSurfacePolish(G4double v) {
  fSurface->SetPolish(v);
  G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

//--------------------------------------------------------------------------------------------------------------------------

void DetectorConstruction::AddTankMPV(const char* c,
                                     G4MaterialPropertyVector* mpv) {
  fTankMPT->AddProperty(c, mpv);//솔직히 AddProperty가 어떤 특성을 더하는 건지 전혀 모르겠다. 
}

void DetectorConstruction::AddWorldMPV(const char* c,
                                       G4MaterialPropertyVector* mpv) {
  fWorldMPT->AddProperty(c, mpv);
}

void DetectorConstruction::AddSurfaceMPV(const char* c,
                                         G4MaterialPropertyVector* mpv) {
  fSurfaceMPT->AddProperty(c, mpv);
}

//-------------------------------------------------------------------------------------------------------------------------

void DetectorConstruction::AddTankMPC(const char* c, G4double v) {
  fTankMPT->AddConstProperty(c, v);//솔직히 AddConstProperty가 어떤 특성을 더하는 건지 전혀 모르겠다. 
}

void DetectorConstruction::AddWorldMPC(const char* c, G4double v) {
  fWorldMPT->AddConstProperty(c, v);
}

void DetectorConstruction::AddSurfaceMPC(const char* c, G4double v) {
  fSurfaceMPT->AddConstProperty(c, v);
}

//--------------------------------------------------------------------------------------------------------------------------

//World Material
void DetectorConstruction::SetWorldMaterial(const G4String& mat) {//material
  G4Material* pmat = G4NistManager::Instance()->FindOrBuildMaterial(mat);//material->Physical material
  if (pmat && fWorldMaterial != pmat) {//complex grammer
    fWorldMaterial = pmat;
    if (fWorld_LV) {
      fWorld_LV->SetMaterial(fWorldMaterial);//this is what we want
      fWorldMaterial->SetMaterialPropertiesTable(fWorldMPT);//this is what we want
    }
    G4RunManager::GetRunManager()->PhysicsHasBeenModified();
  }
}

//Tank Material
void DetectorConstruction::SetTankMaterial(const G4String& mat) {
  G4Material* pmat = G4NistManager::Instance()->FindOrBuildMaterial(mat);
  if (pmat && fTankMaterial != pmat) {
    fTankMaterial = pmat;
    if (fTank_LV) {
      fTank_LV->SetMaterial(fTankMaterial);
      fTankMaterial->SetMaterialPropertiesTable(fTankMPT);
      fTankMaterial->GetIonisation()->SetBirksConstant(0.126*mm/MeV);
    }
    G4RunManager::GetRunManager()->PhysicsHasBeenModified();
  }
}
