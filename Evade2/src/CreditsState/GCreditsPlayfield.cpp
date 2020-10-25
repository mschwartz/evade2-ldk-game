#include "GCreditsPlayfield.h"

GCreditsPlayfield::GCreditsPlayfield(GGameState *aGameState) {
  mGameState = aGameState;
  gSoundPlayer.PlayMusic(S00_INTRO_XM);
  mTitle = new GVectorTitle(60, 70, "CREDITS");
}

GCreditsPlayfield::~GCreditsPlayfield() {
  gResourceManager.ReleaseBitmapSlot(MAIN_MENU_SLOT1);
}

void GCreditsPlayfield::Animate() {
  mTitle->Move();
}

void GCreditsPlayfield::Render() {
  mTitle->Render();
}
